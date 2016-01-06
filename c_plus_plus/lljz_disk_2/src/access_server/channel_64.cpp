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
#include "channel_64.h"

namespace lljz {
namespace disk {

/*
 * 构造函数
 */
Channel64::Channel64() {
    _prev = NULL;
    _next = NULL;
    _expireTime = 0;
}

/*
 * 设置
 *
 * @param   chid    AccessChannelId
 */
void Channel64::setId(uint64_t id) {
    _id = id;
}

/*
 * 得到ID
 */
uint64_t Channel64::getId() {
    return _id;
}

void Channel64::setArgs(void *args) {
    _args = args;
}

void *Channel64::getArgs() {
    return _args;
}

void Channel64::setHandler(IPacketHandler *handler) {
    _handler = handler;
}

IPacketHandler *Channel64::getHandler() {
    return _handler;
}

/*
 * 设置过期时间
 *
 * @param milliseconds 毫秒数, 0为永不过期
 */
void Channel64::setExpireTime(int64_t expireTime) {
    _expireTime = expireTime;
}

}
}