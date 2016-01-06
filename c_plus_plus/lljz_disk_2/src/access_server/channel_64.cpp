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
 * ���캯��
 */
Channel64::Channel64() {
    _prev = NULL;
    _next = NULL;
    _expireTime = 0;
}

/*
 * ����
 *
 * @param   chid    AccessChannelId
 */
void Channel64::setId(uint64_t id) {
    _id = id;
}

/*
 * �õ�ID
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
 * ���ù���ʱ��
 *
 * @param milliseconds ������, 0Ϊ��������
 */
void Channel64::setExpireTime(int64_t expireTime) {
    _expireTime = expireTime;
}

}
}