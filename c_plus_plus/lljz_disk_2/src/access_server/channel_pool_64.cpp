/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

#include "tbnet.h"
#include "channel_pool_64.h"
#include "channel_64.h"

namespace lljz {
namespace disk {

uint64_t ChannelPool64::_globalChannelId = 1;
uint64_t ChannelPool64::_globalTotalCount = 0;

/*
 * 构造函数
 */
ChannelPool64::ChannelPool64() {
    _freeListHead = NULL;
    _freeListTail = NULL;
    _useListHead = NULL;
    _useListTail = NULL;
    _maxUseCount = 0;
}

/*
 * 析构函数
 */
ChannelPool64::~ChannelPool64() {
    std::list<Channel64*>::iterator it = _clusterList.begin();

    for (;it != _clusterList.end(); it++) {
        delete[] *it;
    }
}

/*
 * 得到一个新的channel
 *
 * @return 一个Channel
 */
Channel64 *ChannelPool64::allocChannel() {
    Channel64 *channel = NULL;

    _mutex.lock();
    if (_freeListHead == NULL) { // 如果是空，新分配一些放到freeList中
        assert(CHANNEL_CLUSTER_SIZE_64>2);
        Channel64 *channelCluster = new Channel64[CHANNEL_CLUSTER_SIZE_64];
        TBSYS_LOG(DEBUG, "分配的Channel总数:%d (%d)", 
            ++ChannelPool64::_globalTotalCount,
             sizeof(Channel64));
        _clusterList.push_back(channelCluster);
        _freeListHead = _freeListTail = &channelCluster[1];
        for (int i = 2; i < CHANNEL_CLUSTER_SIZE_64; i++) {
            _freeListTail->_next = &channelCluster[i];
            channelCluster[i]._prev = _freeListTail;
            _freeListTail = _freeListTail->_next;
        }
        _freeListHead->_prev = NULL;
        _freeListTail->_next = NULL;
        channel = &channelCluster[0];   // 把第一个元素拿过来直接用, 不放到freelist中
    } else {
        // 从链头取出
        channel = _freeListHead;
        _freeListHead = _freeListHead->_next;
        if (_freeListHead != NULL) {
            _freeListHead->_prev = NULL;
        } else {
            _freeListTail = NULL;
        }
    }

    // 把channel放到_useList中
    channel->_prev = _useListTail;
    channel->_next = NULL;
    channel->_expireTime = TBNET_MAX_TIME+1;
    if (_useListTail == NULL) {
        _useListHead = channel;
    } else {
        _useListTail->_next = channel;
    }
    _useListTail = channel;

    // generate channel id
    uint64_t id = ++ChannelPool64::_globalChannelId;
    id = (id & 0xFFFFFFFFFFFFFFFF);
    if (id == 0)
    {
      id = 1;
      ChannelPool64::_globalChannelId = 1;
    }
    channel->_id = id;
    channel->_handler = NULL;
    channel->_args = NULL;

    // 把channel放到hashmap中
    _useMap[id] = channel;
    if (_maxUseCount < (int)_useMap.size()) {
        _maxUseCount = _useMap.size();
    }
    _mutex.unlock();

    return channel;
}

/*
 * 释放一个channel
 *
 * @param channel: 要释放的channel
 * @return
 */
bool ChannelPool64::freeChannel(Channel64 *channel) {
    __gnu_cxx::hash_map<uint64_t, Channel64*>::iterator it;

    _mutex.lock();
    it = _useMap.find(channel->_id);
    if (it == _useMap.end()) {
        _mutex.unlock();
        return false;
    }

    // 删除掉
    _useMap.erase(it);

    // 从_userList删除
    if (channel == _useListHead) { // head
        _useListHead = channel->_next;
    }
    if (channel == _useListTail) { // tail
        _useListTail = channel->_prev;
    }
    if (channel->_prev != NULL)
        channel->_prev->_next = channel->_next;
    if (channel->_next != NULL)
        channel->_next->_prev = channel->_prev;

    // 加入到_freeList
    channel->_prev = _freeListTail;
    channel->_next = NULL;
    if (_freeListTail == NULL) {
        _freeListHead = channel;
    } else {
        _freeListTail->_next = channel;
    }
    _freeListTail = channel;
    channel->_id = 0;
    channel->_handler = NULL;
    channel->_args = NULL;

    _mutex.unlock();

    return true;
}

bool ChannelPool64::appendChannel(Channel64 *channel) {
    _mutex.lock();

    // 加入到_freeList
    channel->_prev = _freeListTail;
    channel->_next = NULL;
    if (_freeListTail == NULL) {
        _freeListHead = channel;
    } else {
        _freeListTail->_next = channel;
    }
    _freeListTail = channel;
    channel->_id = 0;
    channel->_handler = NULL;
    channel->_args = NULL;
    _mutex.unlock();

    return true;
}

/*
 * 根据ID，找出一个Channel
 *
 * @param  id: 通道ID
 * @reutrn Channel64
 */
Channel64 *ChannelPool64::offerChannel(uint64_t id) {
    Channel64 *channel = NULL;

    __gnu_cxx::hash_map<uint64_t, Channel64*>::iterator it;
    _mutex.lock();
    it = _useMap.find(id);
    if (it != _useMap.end()) {
        channel = it->second;
        _useMap.erase(it);

        // 从_userList删除
        if (channel == _useListHead) { // head
            _useListHead = channel->_next;
        }
        if (channel == _useListTail) { // tail
            _useListTail = channel->_prev;
        }
        if (channel->_prev != NULL)
            channel->_prev->_next = channel->_next;
        if (channel->_next != NULL)
            channel->_next->_prev = channel->_prev;
        channel->_prev = NULL;
        channel->_next = NULL;
    }
    _mutex.unlock();

    return channel;
}

/*
 * 从useList中找出超时的channel的list,并把hashmap中对应的删除
 *
 * @param now: 当前时间
 */
Channel64* ChannelPool64::getTimeoutList(int64_t now) {
    Channel64 *list = NULL;

    _mutex.lock();
    if (_useListHead == NULL) { //是空
        _mutex.unlock();
        return list;
    }

    Channel64 *channel = _useListHead;
    while (channel != NULL) {
        if (channel->_expireTime >= now) break;
        _useMap.erase(channel->_id);
        channel = channel->_next;
    }

    // 有超时的list
    if (channel != _useListHead) {
        list = _useListHead;
        if (channel == NULL) {  // 全部超时
            _useListHead = _useListTail = NULL;
        } else {
            if (channel->_prev != NULL) {
                channel->_prev->_next = NULL;
            }
            channel->_prev = NULL;
            _useListHead = channel;
        }
    }

    _mutex.unlock();

    return list;
}

/*
 * 把addList的链表加入到freeList中
 *
 * @param addList被加的list
 */
bool ChannelPool64::appendFreeList(Channel64 *addList) {
    // 是空
    if (addList == NULL) {
        return true;
    }

    _mutex.lock();

    // 找tail
    Channel64 *tail = addList;
    while (tail->_next != NULL) {
        tail->_id = 0;
        tail->_handler = NULL;
        tail->_args = NULL;
        tail = tail->_next;
    }
    tail->_id = 0;
    tail->_handler = NULL;
    tail->_args = NULL;

    // 加入到_freeList
    addList->_prev = _freeListTail;
    if (_freeListTail == NULL) {
        _freeListHead = addList;
    } else {
        _freeListTail->_next = addList;
    }
    _freeListTail = tail;

    _mutex.unlock();
    return true;
}

void ChannelPool64::setExpireTime(Channel64 *channel, int64_t now)
{
    _mutex.lock();
    if (channel != NULL) {
        channel->_expireTime = now;
    } 
    _mutex.unlock();
}

}
}

