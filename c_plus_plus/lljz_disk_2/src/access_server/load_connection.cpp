#include "load_connection.h"

namespace lljz {
namespace disk {

LoadConnection::LoadConnection(uint16_t server_type,
tbnet::Transport* transport, 
tbnet::IPacketStreamer* streamer, 
tbnet::IPacketHandler* packetHandler)
:server_type_(server_type)
,_transport(transport)
,_streamer(streamer)
,_packetHandler(packetHandler)
,_queueLimit(0)
,_queueTimeout(15000) {//15s 
    memset(conns_,0,sizeof(conns_));
    conn_num_=0;
}

LoadConnection::~LoadConnection() {

}

bool LoadConnection::Connect() {
    char value[100];
    sprintf(value,"server_spec_%u",server_type_);
    std::vector<const char*> server_specs=
        TBSYS_CONFIG.getStringList("depend_server", value);
    if (server_specs.size() > MAX_SERVER_NUM) {
        TBSYS_LOG(ERROR,"%s is more than %d",value,MAX_SERVER_NUM);
        return false;
    }

    for (int i=0;i<server_specs.size();i++) {
        conns_[i]=_transport->connect(server_specs[i],
            _streamer,true);
        if (NULL==conns_[i]) {
            TBSYS_LOG(ERROR,"connect to %s fail",conns_[i]);
            return false;
        }
        conns_[i]->setDefaultPacketHandler(_packetHandler);
        conns_[i]->setQueueLimit(_queueLimit);
        conns_[i]->setQueueTimeout(_queueTimeout);
        conn_num_++;
    }
    assert(conn_num_);
    return true;
}

void LoadConnection::DisConnect() {
    int conn_num=conn_num_;
    conn_num_=0;
    for (int i=0;i<conn_num;i++) {
        _transport->disconnect(conns_[i]);
    }
}

bool LoadConnection::postPacket(tbnet::Packet *packet, 
tbnet::IPacketHandler *packetHandler,
void *args, 
bool noblocking) {
    if (conn_num_ <= 0) {
        return false;
    } else if (1 == conn_num_) {
        return conns_[0]->postPacket(packet,packetHandler,args,noblocking);
    }

    //10000000次/0.3s
    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> u(0, conn_num_-1);
    int i=u(e);
    return conns_[i]->postPacket(packet,packetHandler,args,noblocking);
}


}
}