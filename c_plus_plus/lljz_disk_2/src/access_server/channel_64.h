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
     * ���캯��
     */
    Channel64();

    /*
     * ����
     *
     * @param   chid    ChannelID
     */
    void setId(uint64_t id);

    /*
     * �õ�Channel ID
     */
    uint64_t getId();

    /*
     * ���ûش�����
     */
    void setArgs(void *args);

    /*
     * ȡ���ش�����
     */
    void *getArgs();

    /*
     * ����packethandler�ľ��
     */
    void setHandler(IPacketHandler *handler);

    /*
     * �õ����
     */
    IPacketHandler *getHandler();

    /*
     * ���ù���ʱ��, ����ʱ��
     *
     * @param   expireTime
     */
    void setExpireTime(int64_t expireTime);

    /* ����ʱ�� */
    int64_t getExpireTime() {
        return _expireTime;
    }

    /*
     * ��һ��
     */
    Channel64 *getNext() {
        return _next;
    }

private:
    uint64_t _id;      // channel id
    void *_args;    // �ش�����
    IPacketHandler *_handler;
    int64_t _expireTime; // ����ʱ��

private:
    Channel64 *_prev;     // ��������
    Channel64 *_next;
};
}
}
#endif
