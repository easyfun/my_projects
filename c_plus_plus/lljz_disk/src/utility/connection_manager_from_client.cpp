#include "connection_manager_from_client.hpp"
#include "response_packet.hpp"

namespace lljz {
namespace disk {

ConnectionManagerFromClient::ConnectionManagerFromClient()
: stop_(false) {

}

ConnectionManagerFromClient::~ConnectionManagerFromClient() {

}

int RegisterConnection::Register(
uint64_t id, tbnet::Connection* conn) {
    ConnFromClientMap::iterator it;
    tbnet::Connection* conn_exist;
    rw_lock_.wrlock();
    it=conn_from_client_.find(id);
    if (conn_from_client_.end() != it) {
        conn_exist=it->second();
        if (conn_exist==conn) {
            rw_lock_.unlock();
            return 0;
        }
        if (conn_exist->isConnectState()) {
            rw_lock_.unlock();
            return -1; //已注册
        }
    }
    conn_from_client_[id]=conn;
    rw_lock_.unlock();
    return 0;
}

bool ConnectionManagerFromClient::IsRegister(uint64_t id) {
    rw_lock_.rdlock();
    if (conn_from_client_.end()==conn_from_client_.find(id)) {
        rw_lock_.unlock();
        return false;
    }
    rw_lock_.unlock();
    return true;
}


void ConnectionManagerFromClient::PostPacket(
uint64_t id, tbnet::Packet* packet) {
    tbnet::Connection* conn;
    ConnFromClientMap::iterator it;
    rw_lock_.rdlock();
    it=conn_from_client_.find(id);
    if (conn_from_client_.end()==it) {
        rw_lock_.unlock();

        mutex_.lock();
        resend_queue_.push(packet);
        mutex_.unlock();
        return;
    }

    conn=it->second();
    if (conn && conn->isConnectState()) {
        if (false==conn->postPacket(packet)) {
            rw_lock_.unlock();

            mutex_.lock();
            resend_queue_.push(packet);
            mutex_.unlock();

            rw_lock_.wrlock();
            conn_from_client_.erase(it);
            rw_lock_.unlock();
            return;
        }
    }
    rw_lock_.unlock();
}


bool ConnectionManagerFromClient::start() {
    timer_thread_.start(this,NULL);
    return true;
}

bool ConnectionManagerFromClient::stop() {
    stop_=true;
    return true;
}

bool ConnectionManagerFromClient::wait() {
    timer_thread_.join();
    destroy();
    return true;
}

void ConnectionManagerFromClient::destroy() {
    tbnet::Packet* packet;
    mutex_.lock();
    while(resend_queue_.size()) {
        packet=resend_queue_.pop();
        delete packet;
    }
    mutex_.unlock();
    return true;
}

void ConnectionManagerFromClient::run(
tbsys::CThread* thread, void* arg) {
    int timer_check_resend=0;
    while (!stop_) {
        if (0==timer_check_resend) {
            CheckResend();
        }

        if (2==++timer_check_resend) {
            timer_check_resend=0;
        }

        usleep(1000000); //1s
    }
}

void ConnectionManagerFromClient::CheckResend() {
    tbnet::Connection* conn;
    ConnFromClientMap::iterator it;

    PacketQueue copy_queue;
    PacketQueue fail_queue;
    mutex_.lock();
    resend_queue_.moveTo(&copy_queue);
    mutex_.unlock();

    int64_t now_time=tbsys::CTimeUtil::getTime();
    tbnet::Packet* packet;
    while(copy_queue.size()) {
        if (stop_) {
            break;
        }
        packet=copy_queue.pop();

        ResponsePacket* resp=(ResponsePacket* )packet;
        if (now_time - resp->get_recv_time() >= 30000) {
            delete packet;
            return;
        }

        rw_lock_.rdlock();
        it=conn_from_client_.find(resp->dest_id_);
        if (conn_from_client_.end()==it) {
            fail_queue.push(packet);
            rw_lock_.unlock();
            continue;
        }

        conn=it->second();
        if (conn && conn->isConnectState()) {
            if (false==conn->postPacket(packet)) {
                rw_lock_.unlock();

                fail_queue.push(packet);

                rw_lock_.wrlock();
                conn_from_client_.erase(it);
                rw_lock_.unlock();
                continue;
            }
        }
        rw_lock_.unlock();
    }

    if (copy_queue.size()) {
        mutex_.lock();
        copy_queue.moveTo(&resend_queue_);
        mutex_.unlock();
    }

    if (fail_queue.size()) {
        mutex_.lock();
        fail_queue.moveTo(&resend_queue_);
        mutex_.unlock();
    }
}


}
}