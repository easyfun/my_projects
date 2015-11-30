#include "load_connection_manager.hpp"

namespace lljz {
namespace disk {

LoadConnectionManager::LoadConnectionManager(
tbnet::Transport *transport, 
tbnet::IPacketStreamer *streamer, 
tbnet::IPacketHandler *packetHandler) {
    assert(transport != NULL);
    assert(streamer != NULL);
    assert(packetHandler != NULL);
    _transport = transport;
    _streamer = streamer;
    _packetHandler = packetHandler;
    _queueLimit = 256;
    _queueTimeout = 5000;
    _status = 0;
    atomic_set(&last_server_id_index_,0);
}

LoadConnectionManager::~LoadConnectionManager() {

}

tbnet::Connection *ConnectionManager::connect(
uint64_t serverId,bool autoConn) {
    if (_status == 1) {
        return NULL;
    }
    if (serverId == 0) {
        return NULL;
    }

    rw_simple_lock_.wrlock();

    TBNET_CONN_MAP::iterator it ;
    it = _connectMap.find(serverId);
    if (it != _connectMap.end()) {
        return it->second;
        rw_simple_lock_.unlock();
    }

    // ´ÓtransportÖÐ»ñÈ¡
    char spec[64];
    sprintf(spec, "tcp:%s", 
        tbsys::CNetUtil::addrToString(serverId).c_str());
    tbnet::Connection *conn = _transport->connect(
        spec, _streamer, autoConn);
    if (!conn) {
        TBSYS_LOG(WARN, "Á¬½Óµ½·þÎñÆ÷Ê§°Ü: %s", spec);
        rw_simple_lock_.unlock();
        return NULL;
    } else {
        conn->setDefaultPacketHandler(_packetHandler);
        conn->setQueueLimit(_queueLimit);
        conn->setQueueTimeout(_queueTimeout);
        _connectMap[serverId] = conn;
        server_id_.push_back(serverId);
    }

    rw_simple_lock_.unlock();
    return conn;
}

void LoadConnectionManager::disconnect(
uint64_t serverId) {
    rw_simple_lock_.wrlock();
    TBNET_CONN_MAP::iterator it = 
        _connectMap.find(serverId);
    if (it != _connectMap.end()) {
        if (it->second) {
            _transport->disconnect(it->second);
        }
        _connectMap.erase(it);
    }

    int size=server_id_.size();
    for (int i=0;i<size;) {
        if (serverId==server_id_[i]) {
            server_id_.erase(server_id_.begin()+i);
            size=server_id_.size();
            continue;
        }
        i++;
    }
    rw_simple_lock_.unlock();
}

void LoadConnectionManager::setDefaultQueueLimit(
int queueLimit) {
    _queueLimit = queueLimit;
}

void LoadConnectionManager::setDefaultQueueTimeout(
int queueTimeout) {
    _queueTimeout = queueTimeout;
}

void LoadConnectionManager::cleanup() {
    _status = 1;
    rw_simple_lock_.wrlock();
    TBNET_CONN_MAP::iterator it;
    for (it = _connectMap.begin(); 
    it != _connectMap.end(); 
    ++it) {
        _transport->disconnect(it->second);
    }
    _connectMap.clear();
    server_id_.clear();
    rw_simple_lock_.unlock();

    reconn_mutex_.lock();
    reconn_server_id_.clear();
    reconn_mutex_.unlock();
}

bool LoadConnectionManager::sendPacket(
tbnet::Packet* packet, 
tbnet::IPacketHandler *packetHandler, 
void *args, bool noblocking) {
    bool ret=false;
    tbnet::Connection *conn=NULL;
    uint64_t server_id;
    int index;
    rw_simple_lock_.rdlock();
    int size=server_id_.size();
    std::vector<uint64_t> delete_server_id;
    std::vector<uint64_t> reconn_server_id;
    for (int i=0;i<size;i++) {
        index=atomic_add_return(1,
            &last_server_id_index_);
        if (index >= size) {
            atomic_set(&last_server_id_index_,0);
            index=0;
        }
        server_id=server_id_[index];

        TBNET_CONN_MAP::iterator it ;
        it = _connectMap.find(serverId);
        if (it == _connectMap.end()) {
            delete_server_id.push_back(serverId);
            continue;
        }
        conn=it->second;

        if (conn->postPacket(
            packet,
            packetHandler,
            args,
            noblocking)) {
            rw_simple_lock_.unlock();
            ret=true;
            break;
        }
        //remove to reconn list
        reconn_server_id.push_back(server_id);
    }
    rw_simple_lock_.unlock();

    if (0==delete_server_id.size()
    && 0==reconn_server_id.size()) {
        return ret;
    }

    rw_simple_lock_.wrlock();
    size=delete_server_id.size();
    for (int i=0;i<size;i++) {
        int s_size=server_id_.size();
        for (int j=0;j<s_size;) {
            if (delete_server_id[i]==server_id_[j]) {
                server_id_.erase(server_id_.begin()+j);
                s_size=server_id_.size();
                continue;
            }
            j++;
        }
    }
    delete_server_id.clear();

    reconn_mutex_.lock();
    size=reconn_server_id.size();
    for (int i=0;i<size;i++) {
        int s_size=server_id_.size();
        for (int j=0;j<s_size;) {
            if (reconn_server_id[i]==server_id_[j]) {
                ReconnectInfo reconn_info(reconn_server_id[i]);
                reconn_server_id_.push_back(reconn_info);
                server_id_.erase(server_id_.begin()+j);
                s_size=server_id_.size();
                continue;
            }
            j++;
        }
    }
    reconn_mutex_.unlock();
    reconn_server_id.clear();

    rw_simple_lock_.unlock();
    return ret;
}

void LoadConnectionManager::CheckReconnect() {
    char spec[64];
    RequestPacket* req;
    reconn_mutex_.lock();
    int size=reconn_server_id_.size();
    for (int i=0;i<size;i++) {
        if (1==reconn_server_id_[i].status_) {
            continue;
        }
        sprintf(spec, "tcp:%s", 
            tbsys::CNetUtil::addrToString(
                reconn_server_id_[i].server_id_).c_str());
        tbnet::Connection* conn=transport->connect(
            spec,_streamer,false);
        if (NULL==conn) {
            continue;
        }
        req=new RequestPacket;
        if (false == conn->postPacket(req,
                            this,
                            conn,
                            false)) {
            delete req;
            continue;
        }
        reconn_server_id_[i].status_=1;
    }
    reconn_mutex_.unlock();
}

// 服务注册结果
tbnet::HPRetCode LoadConnectionManager::handlePacket(
Packet *packet, void *args) {
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
            reconn_mutex_.lock();
            int size=reconn_server_id_.size();
            tbnet::Connection* conn=(tbnet::Connection* )args;
            for (int i=0;i<size;i++) {
                if (reconn_server_id_[i].server_id_!=conn->getServerId()) {
                    continue;
                }
                reconn_server_id_[i].status_=0;
                _transport.disconnect(conn);
                break;
            }
            reconn_mutex_.unlock();

        }
        return IPacketHandler::FREE_CHANNEL;
    }

    tbnet::Connection* conn=(tbnet::Connection* )args;
    ResponsePakcet* resp=(ResponsePacket* )packet;
    int size=reconn_server_id_.size();
    uint64_t server_id=0;
    reconn_mutex_.lock();
    for (int i=0;i<size;i++) {
        if (reconn_server_id_[i].server_id_!=conn->getServerId()) {
            continue;
        }
        if (0==resp->error_code) {
            server_id=reconn_server_id_[i].server_id_;
            reconn_server_id_.erase(reconn_server_id_.begin()+i);
        } else {
            reconn_server_id_[i].status_=0;
            _transport.disconnect(conn);
        }
        break;
    }
    reconn_mutex_.unlock();

    if (i!=size) {
        rw_simple_lock_.wrlock();
        assert(_connectMap.end()==_connectMap.find(server_id));
        _connectMap[server_id] = conn;
        rw_simple_lock_.unlock();
    } else {
        _transport.disconnect(conn);
    }

    return IPacketHandler::FREE_CHANNEL;
}


}
}