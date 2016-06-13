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
 * Modified by lljzfly
 */

#ifndef LLJZ_DISK_CHANNEL_64_H_
#define LLJZ_DISK_CHANNEL_64_H_

namespace lljz {
namespace disk {

class Channel64 {
    friend class ChannelPool64;

public:
    /*
     * 构造函数
     */
    Channel64();

    /*
     * 设置
     *
     * @param   chid    ChannelID
     */
    void setId(uint64_t id);

    /*
     * 得到Channel ID
     */
    uint64_t getId();

    /*
     * 设置回传参数
     */
    void setArgs(void *args);

    /*
     * 取到回传参数
     */
    void *getArgs();

    /*
     * 设置packethandler的句柄
     */
    void setHandler(IPacketHandler *handler);

    /*
     * 得到句柄
     */
    IPacketHandler *getHandler();

    /*
     * 设置过期时间, 绝对时间
     *
     * @param   expireTime
     */
    void setExpireTime(int64_t expireTime);

    /* 过期时间 */
    int64_t getExpireTime() {
        return _expireTime;
    }

    /*
     * 下一个
     */
    Channel64 *getNext() {
        return _next;
    }

private:
    uint64_t _id;      // channel id
    void *_args;    // 回传参数
    IPacketHandler *_handler;
    int64_t _expireTime; // 到期时间

private:
    Channel64 *_prev;     // 用在链表
    Channel64 *_next;
};
}
}
#endif
