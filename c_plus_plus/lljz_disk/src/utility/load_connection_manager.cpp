#include "load_connection_manager.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"

namespace lljz {
namespace disk {

LoadConnectionManager::LoadConnectionManager(
tbnet::Transport *transport, 
tbnet::IPacketStreamer *streamer, 
uint64_t self_server_id,
uint16_t dest_server_type,
tbnet::IPacketHandler *packetHandler) {
    assert(transport != NULL);
    assert(streamer != NULL);
//    assert(packetHandler != NULL);
    _transport = transport;
    _streamer = streamer;
    _packetHandler = packetHandler;
    _queueLimit = 0;//256;
    _queueTimeout = 60000;//5000;//ms
    _status = 0;
    atomic_set(&last_send_server_id_index_,0);
    self_server_id_=self_server_id;
    dest_server_type_=dest_server_type;
}

LoadConnectionManager::~LoadConnectionManager() {

}

tbnet::Connection* LoadConnectionManager::connect(
uint64_t serverId,bool autoConn) {
    if (_status == 1) {
        return NULL;
    }
    if (serverId == 0) {
        return NULL;
    }

    TBNET_CONN_MAP::iterator it ;
    send_conn_rw_lock_.wrlock();
    it = send_conn_map_.find(serverId);
    if (it != send_conn_map_.end()) {
        send_conn_rw_lock_.unlock();
        return it->second;
    }
    send_conn_rw_lock_.unlock();

    create_conn_mutex_.lock();
    int size=inconn_server_id_.size();
    int i;
    for (i=0;i<size;i++) {
        if (inconn_server_id_[i]==serverId) {
            create_conn_mutex_.unlock();
            return NULL;
        }
    }

    size=reconn_server_id_.size();
    for (i=0;i<size;i++) {
        if (reconn_server_id_[i]==serverId) {
            create_conn_mutex_.unlock();
            return NULL;
        }
    }
    create_conn_mutex_.unlock();

    char spec[64];
    sprintf(spec, "tcp:%s", 
        tbsys::CNetUtil::addrToString(serverId).c_str());
    TBSYS_LOG(ERROR,"--debug--spec=%s",spec);
    tbnet::Connection *conn = _transport->connect(
        spec, _streamer, autoConn);
    if (!conn) {
        TBSYS_LOG(WARN, "connect to server error: %s", spec);
        return NULL;
    }
    conn->setDefaultPacketHandler(this);
    conn->setQueueLimit(_queueLimit);
    conn->setQueueTimeout(_queueTimeout);

    RequestPacket* req=new RequestPacket();
    req->src_type_=SERVER_TYPE_ACCESS_SERVER;
    req->src_id_=self_server_id_;
    req->dest_type_=dest_server_type_;
    req->dest_id_=serverId;
    req->msg_id_=PUBLIC_REGISTER_REQ;

    if (conn->postPacket(req,
                        this,
                        (void*)conn,
                        false)) {
        create_conn_mutex_.lock();
        inconn_server_id_.push_back(serverId);
        create_conn_mutex_.unlock();
        return conn;
    }

    delete req;
    _transport->disconnect(conn);
    create_conn_mutex_.lock();
    reconn_server_id_.push_back(serverId);
    create_conn_mutex_.unlock();
    return NULL;
}

void LoadConnectionManager::disconnect(
uint64_t serverId) {
    TBNET_CONN_MAP::iterator it;
    int i;
    int size;
    send_conn_rw_lock_.wrlock();
    it = send_conn_map_.find(serverId);
    if (it != send_conn_map_.end()) {
        if (it->second) {
            _transport->disconnect(it->second);
        }
        send_conn_map_.erase(it);
    }

    size=send_server_id_.size();
    for (i=0;i<size;) {
        if (serverId==send_server_id_[i]) {
            send_server_id_.erase(send_server_id_.begin()+i);
            size=send_server_id_.size();
            continue;
        }
        i++;
    }
    send_conn_rw_lock_.unlock();

    create_conn_mutex_.lock();
    size=inconn_server_id_.size();
    for (i=0;i<size;) {
        if (inconn_server_id_[i]==serverId) {
            inconn_server_id_.erase(inconn_server_id_.begin()+i);
            size=inconn_server_id_.size();
            continue;
        }
        i++;
    }

    size=reconn_server_id_.size();
    for (i=0;i<size;i++) {
        if (reconn_server_id_[i]==serverId) {
            reconn_server_id_.erase(reconn_server_id_.begin()+i);
            size=reconn_server_id_.size();
            continue;
        }
    }    
    create_conn_mutex_.unlock();
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
    send_conn_rw_lock_.wrlock();
    TBNET_CONN_MAP::iterator it;
    for (it = send_conn_map_.begin(); 
    it != send_conn_map_.end(); 
    ++it) {
        _transport->disconnect(it->second);
    }
    send_conn_map_.clear();
    send_server_id_.clear();
    send_conn_rw_lock_.unlock();

    create_conn_mutex_.lock();
    reconn_server_id_.clear();
    inconn_server_id_.clear();
    create_conn_mutex_.unlock();
}

bool LoadConnectionManager::sendPacket(
tbnet::Packet* packet, 
tbnet::IPacketHandler *packetHandler, 
void *args, bool noblocking) {
    bool ret=false;
    tbnet::Connection *conn=NULL;
    uint64_t server_id;
    int index;
    int size;
    int i;
    TBNET_CONN_MAP::iterator it;
    
    send_conn_rw_lock_.rdlock();
    size=send_server_id_.size();
    std::vector<uint64_t> delete_server_id;
    std::vector<uint64_t> reconn_server_id;
    for (i=0;i<size;i++) {
        index=atomic_add_return(1,
            &last_send_server_id_index_);
        if (index >= size) {
            atomic_set(&last_send_server_id_index_,0);
            index=0;
        }
        server_id=send_server_id_[index];

        it = send_conn_map_.find(server_id);
        if (it == send_conn_map_.end()) {
            delete_server_id.push_back(server_id);
            continue;
        }
        conn=it->second;

        RequestPacket *req = (RequestPacket *) packet;
        TBSYS_LOG(ERROR,"req :chanid=%u|pcode=%u|msg_id=%u|src_type=%u|"
            "src_id=%llu|dest_type=%u|dest_id=%u|data=%s",
            req->getChannelId(),req->getPCode(),req->msg_id_,
            req->src_type_,req->src_id_,req->dest_type_,
            req->dest_id_,req->data_);

        if (conn->postPacket(packet,
            packetHandler,args,noblocking)) {
            ret=true;
            break;
        }
        //remove to reconn list
        reconn_server_id.push_back(server_id);
    }
    send_conn_rw_lock_.unlock();

    if (0==delete_server_id.size()
    && 0==reconn_server_id.size()) {
        return ret;
    }

    send_conn_rw_lock_.wrlock();
    size=delete_server_id.size();
    for (i=0;i<size;i++) {
        int s_size=send_server_id_.size();
        for (int j=0;j<s_size;) {
            if (delete_server_id[i]==send_server_id_[j]) {
                send_server_id_.erase(send_server_id_.begin()+j);
                s_size=send_server_id_.size();
                continue;
            }
            j++;
        }
        it=send_conn_map_.find(delete_server_id[i]);
        if (send_conn_map_.end()!=it) {
            _transport->disconnect(it->second);
            send_conn_map_.erase(it);
        }
    }
    delete_server_id.clear();

    size=reconn_server_id.size();
    for (i=0;i<size;i++) {
        int s_size=send_server_id_.size();
        for (int j=0;j<s_size;) {
            if (reconn_server_id[i]==send_server_id_[j]) {
                send_server_id_.erase(send_server_id_.begin()+j);
                s_size=send_server_id_.size();
                continue;
            }
            j++;
        }
        it=send_conn_map_.find(reconn_server_id[i]);
        if (send_conn_map_.end()!=it) {
            _transport->disconnect(it->second);
            send_conn_map_.erase(it);
        }
    }
    send_conn_rw_lock_.unlock();

    create_conn_mutex_.lock();
    size=reconn_server_id.size();
    for (i=0;i<size;i++) {
        reconn_server_id_.push_back(reconn_server_id[i]);
    }
    reconn_server_id.clear();
    create_conn_mutex_.unlock();

    return ret;
}

void LoadConnectionManager::DisToReconnect(
uint64_t server_id) {
    TBNET_CONN_MAP::iterator it;
    int size;
    int i;
    bool have=false;
    send_conn_rw_lock_.wrlock();
    it = send_conn_map_.find(server_id);
    if (it != send_conn_map_.end()) {
        if (it->second) {
            _transport->disconnect(it->second);
        }
        send_conn_map_.erase(it);
        have=true;
    }

    size=send_server_id_.size();
    for (i=0;i<size;) {
        if (server_id==send_server_id_[i]) {
            send_server_id_.erase(send_server_id_.begin()+i);
            size=send_server_id_.size();
            continue;
        }
        i++;
    }
    send_conn_rw_lock_.unlock();

    create_conn_mutex_.lock();
    size=inconn_server_id_.size();
    for (i=0;i<size;) {
        if (server_id==inconn_server_id_[i]) {
            inconn_server_id_.erase(inconn_server_id_.begin()+i);
            size=inconn_server_id_.size();
            have=true;
            continue;
        }
        i++;
    }
    if (have) {
        reconn_server_id_.push_back(server_id);
    }
    create_conn_mutex_.unlock();
}

void LoadConnectionManager::CheckReconnect() {
    char spec[64];
    RequestPacket* req;
    create_conn_mutex_.lock();
//    TBSYS_LOG(ERROR,"---in_conn_size=%d",inconn_server_id_.size());
//    TBSYS_LOG(ERROR,"---re_conn_size=%d",reconn_server_id_.size());
    int size=reconn_server_id_.size();
    for (int i=0;i<size;) {
        sprintf(spec, "tcp:%s", 
            tbsys::CNetUtil::addrToString(
                reconn_server_id_[i]).c_str());
        TBSYS_LOG(ERROR,"---size=%d,i=%d,spec=%s",
            size,i,spec);
        tbnet::Connection* conn=_transport->connect(
            spec,_streamer,false);
        if (NULL==conn) {
            TBSYS_LOG(ERROR,"====================1");
            i++;
            continue;
        }
        conn->setDefaultPacketHandler(this);
        conn->setQueueLimit(_queueLimit);
        conn->setQueueTimeout(_queueTimeout);

        req=new RequestPacket();
        req->src_type_=SERVER_TYPE_ACCESS_SERVER;
        req->src_id_=self_server_id_;
        req->dest_type_=dest_server_type_;
        req->dest_id_=reconn_server_id_[i];
        req->msg_id_=PUBLIC_REGISTER_REQ;

        if (false == conn->postPacket(req,
            this,conn,false)) {
            TBSYS_LOG(ERROR,"====================2");
            _transport->disconnect(conn);
            delete req;
            i++;
            continue;
        }
        inconn_server_id_.push_back(reconn_server_id_[i]);
        reconn_server_id_.erase(reconn_server_id_.begin()+i);
        size=reconn_server_id_.size();
    }
    create_conn_mutex_.unlock();
}

// 服务注册结果
tbnet::IPacketHandler::HPRetCode 
LoadConnectionManager::handlePacket(
tbnet::Packet *packet, void *args) {
    int size;
    int i;
    uint64_t server_id;
    tbnet::Connection* conn;

    if (!packet->isRegularPacket()) { // 是否正常的包
        //server close,not call this function
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
            tbnet::Socket* socket=(tbnet::Socket* )args;
            server_id=socket->getId();
            DisToReconnect(server_id);
        } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
            conn=(tbnet::Connection* )args;
            server_id=conn->getServerId();
            DisToReconnect(server_id);
        }
        return IPacketHandler::FREE_CHANNEL;
    }

    bool conn_ok=false;
    conn=(tbnet::Connection* )args;
    ResponsePacket* resp=(ResponsePacket* )packet;
    create_conn_mutex_.lock();
    size=inconn_server_id_.size();
    for (i=0;i<size;i++) {
        if (inconn_server_id_[i]!=conn->getServerId()) {
            continue;
        }
        if (0==resp->error_code_) {
            server_id=inconn_server_id_[i];
            inconn_server_id_.erase(inconn_server_id_.begin()+i);
            conn_ok=true;
        }
        break;
    }
    create_conn_mutex_.unlock();

    if (conn_ok) {
        send_conn_rw_lock_.wrlock();
        assert(send_conn_map_.end()==send_conn_map_.find(server_id));
        send_server_id_.push_back(server_id);
        send_conn_map_[server_id] = conn;
        send_conn_rw_lock_.unlock();
    } else {
        _transport->disconnect(conn);
        DisToReconnect(conn->getServerId());
    }

    return IPacketHandler::FREE_CHANNEL;
}


}
}