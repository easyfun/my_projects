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

/*
 * modified by lljzfly
 */

#ifndef LLJZ_DISK_CHANNEL_POOL_64_H_
#define LLJZ_DISK_CHANNEL_POOL_64_H_

#define CHANNEL_CLUSTER_SIZE_64 25
namespace lljz {
namespace disk {

class Channel64;

class ChannelPool64 {

public:
    /*
     * 构造函数
     */
    ChannelPool64();

    /*
     * 析构函数
     */
    ~ChannelPool64();

    /*
     * 得到一个新的channel
     *
     * @return 一个Channel
     */
    Channel64 *allocChannel();

    /*
     * 释放一个channel
     *
     * @param channel: 要释放的channel
     * @return
     */
    bool freeChannel(Channel64 *channel);
    bool appendChannel(Channel64 *channel);

    /*
     * 查找一下channel
     *
     * @param id: channel id
     * @return Channel64
     */
    Channel64* offerChannel(uint64_t id);

    /*
     * 从useList中找出超时的channel的list,并把hashmap中对应的删除
     *
     * @param now: 当前时间
     */
    Channel64* getTimeoutList(int64_t now);

    /*
     * 把addList的链表加入到freeList中
     *
     * @param addList被加的list
     */
    bool appendFreeList(Channel64 *addList);

    /*
     * 被用链表的长度
     */
    int getUseListCount() {
        return static_cast<int32_t>(_useMap.size());
    }

    void setExpireTime(Channel64 *channel, int64_t now); 

private:
    __gnu_cxx::hash_map<uint64_t, Channel64*> _useMap; // 使用的map
    std::list<Channel64*> _clusterList;                // cluster list
    tbsys::CThreadMutex _mutex;

    Channel64 *_freeListHead;             // 空的链表
    Channel64 *_freeListTail;
    Channel64 *_useListHead;              // 被使用的链表
    Channel64 *_useListTail;
    int _maxUseCount;                   // 被用链表的长度

    static uint64_t _globalChannelId;   // 生成统一的id
    static uint64_t _globalTotalCount;
};

}
}
#endif
