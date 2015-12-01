#include "connection_manager_to_server.hpp"
//#include "json/json.h"

namespace lljz {
namespace disk {

/**********************************************************************/
// class PacketHandlerConnToServer
/**********************************************************************/
tbnet::HPRetCode PacketHandlerConnToServer::handlePacket(Packet *packet, void *args) {
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        //连接断开事件通知给连接管理器处理
        if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
            tbnet::Socket* socket=(tbnet::Socket* )args;
            //通知连接无效
            cmts_->DisConnect(socket->getId());
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }
    return IPacketHandler::FREE_CHANNEL;
}


/**********************************************************************/
// class ConnectionManagerToServer
/**********************************************************************/
ConnectionManagerToServer::ConnectionManagerToServer(
tbnet::Transport* transport,
tbnet::IPacketStreamer* packet_streamer,
tbnet::IPacketHandler* packet_handler)
:transport_(transport)
,packet_streamer_(packet_streamer)
,packet_handler_(packet_handler)
,stop_(false) {
    memset(config_server_spec_,0,
        sizeof(config_server_spec_));
    conn_to_config_server_=NULL;
}

ConnectionManagerToServer::~ConnectionManagerToServer() {
    destroy();
}

bool ConnectionManagerToServer::start() {

    const char * server_spec = TBSYS_CONFIG.getString("server","config_server",NULL);
    if (NULL == server_spec) {
        TBSYS_LOG(ERROR,"%s","config_server url error");
        return false;
    }
    memset(config_server_spec_,0,
        sizeof(config_server_spec_));
    strcat(config_server_spec_,server_spec);

    conn_to_config_server_=transport_->connect(
        config_server_spec_,packet_streamer_,true);
    if (NULL==conn_to_config_server_) {
        TBSYS_LOG(ERROR,"%s","connect to config_server error");
        return false;
    }
    conn_to_config_server_->setDefaultPacketHandler(this);

    run_thread_.start(this,NULL);
    task_queue_thread_.start();    
    return true;
}

bool ConnectionManagerToServer::stop() {
    stop_=true;
    return true;
}

bool ConnectionManagerToServer::wait() {
    run_thread_.join();
    destroy();
    return true;
}

void ConnectionManagerToServer::destroy() {

}

void ConnectionManagerToServer::run(
tbsys::CThread* thread, void* arg) {

    int check_config_server=0; // 60s
    int check_reconn_server=0; // 5s

    while (!stop_) {
/*        {
            printf("test::ConnectionManagerToServer::run\n");
            sleep(3);
            continue;
        }*/

        if (0==check_config_server) {
            CheckConfigServer();
        }

        if (2==check_reconn_server) {
            CheckReconnServer();
        }

        if (60 == ++check_config_server) {
            check_config_server=0;
        }

        if (7 == ++check_reconn_server) {
            check_reconn_server=2;
        }

        usleep(1000000);//1s
    }
}

void ConnectionManagerToServer::CheckConfigServer() {
    //get server url from config_server
    std::string spec;
    uint32_t server_type=0;
    uint32_t server_id=0;
    std::vector<ServerURL*> buff_server_url;
    bool server_changed=false;

    RequestPacket* req=new RequestPacket;

    char data[1024]={0};
    Json::Value data_root;
    Json::Reader reader;
    if (!reader.parse(data, data_root, false)) {
        return;
    }

    int size=data_root.size();
    int svr_url_size=server_url_.size();
    buff_server_url.clear();

    server_changed=false;
    //server_url_ check remove
    for (int i=0;i<svr_url_size;i++) {
        for (int j=0;j<size;j++) {
            spec=data_root[j]["spec"];
            if (0==strcmp(server_url_[i]->spec_,spec.c_str())) {
                break;
            }
        }
        if (j==size) {
            server_url_[i]->changed_=2;
            server_changed=true;
        }
    }

    //server_url_ check add
    for (int i=0;i<size;i++) {
        spec=data_root[i]["spec"].asString();
        server_type=data_root[i]["server_type"].asInt();
        server_id=data_root[i]["server_id"].asInt();

        for (int j=0;j<svr_url_size;j++) {
            if (0==strcmp(server_url_[j]->spec_, spec.c_str())) {
                break;
            }
        }
        
        if (j==svr_url_size) {
            ServerURL* svr_url=new ServerURL;
            strcat(svr_url->spec_,spec.c_str());
            svr_url->server_type_=server_type;
            svr_url->server_id_=server_id;
            svr_url->changed_=1;
            buff_server_url.push(svr_url);
            server_changed=true;
        }
    }

    size=buff_server_url.size();
    for (int i=0;i<size;i++) {
        server_url_.push(buff_server_url[i])
    }
    buff_server_url.clear();

    if (!server_changed) {
        return;
    }

    //refresh connect manager
    __gnu_cxx::hash_map<uint32_t,
        LoadConnectionManager*>::iteraotr it;
    __gnu_cxx::hash_map<uint64_t,uint16_t> it_type;
    LoadConnectionManager* load_conn_manager=NULL;
    mutex_.lock();
    for (int i=0;i<server_url_.size();i++) {
        if (1==server_url_[i]->changed_) {
            //add a connection
            server_url_[i]->changed_=0;

            it=conn_manager_.find(
                server_url_[i]->server_type_);
            if (conn_manager_.end()==it) {
                load_conn_manager=new LoadConnectionManager(
                    transport_,
                    packet_streamer_,
                    packet_handler_);
                conn_manager_[server_url_[i]->server_type_]=
                    load_conn_manager;
            } else {
                load_conn_manager=it->second;
            }
            load_conn_manager->connect(
                server_id_[i]->server_id_,
                NULL,0,0);
        } else if (2==server_url_[i]->changed_) {
            //remove a connection
            //server_url_[i]->changed_=0;

            it=conn_manager_.find(
                server_url_[i]->server_type_);
            if (conn_manager_.end() == it) {
                continue;
            }
            load_conn_manager=it->second;
            load_conn_manager->disconnect(
                server_url_[i]->server_id_);
        }
    }

    for (int i=0; i<server_url_.size();) {
        if (2==server_url_[i]->changed_) {
            server_url_.erase(server_url_.begin()+i);
            continue;
        }
        i++;
    }
    mutex_.unlock();
}

void ConnectionManagerToServer::CheckReconnServer() {
    __gnu_cxx::hash_map<uint32_t,
                        LoadConnectionManager*>::iteraotr it;
    LoadConnectionManager* lcm;

    mutex_.lock();
    for (it=conn_manager_.begin();
    it!=conn_manager_.end();
    it++) {
        lcm=it->second;
        lcm->CheckReconnect();
    }
    mutex_.unlock();
}

bool ConnectionManagerToServer::PostPacket(
uint16_t server_type,
tbnet::Packet* packet, 
void* args) {

    __gnu_cxx::hash_map<uint32_t,
                        LoadConnectionManager*>::iteraotr it;
    LoadConnectionManager* lcm=NULL;
    mutex_.lock();
    it=conn_manager_.find(server_type);
    if (conn_manager_.end()!=it) {
        lcm=it->second;
    }
    mutex_.unlock();

    if (lcm==NULL) {
        return false;
    }
    return lcm->sendPacket(packet,packet_handler_,args)
}

void ConnectionManagerToServer::DisConnect(
uint64 server_id) {

    __gnu_cxx::hash_map<uint32_t,
                        LoadConnectionManager*>::iteraotr it;
    LoadConnectionManager* lcm=NULL;
    uint16_t server_type;
    mutex_.lock();
    int size=server_url_.size();
    for (int i=0;i<size;i++) {
        if (server_id==server_url_[i]->server_id_) {
            server_type=server_url_[i]->server_type_;
            break;
        }
    }
    if (i==size) {
        mutex_.unlock();
        return;
    }

    it=conn_manager_.find(server_type);
    if (conn_manager_.end()!=it) {
        lcm=it->second;
    }
    mutex_.unlock();

    if (lcm==NULL) {
        return;
    }

    lcm->disconnect(server_id);
}


tbnet::HPRetCode ConnectionManagerToServer::handlePacket(
Packet *packet, void *args) {
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
        }
        return IPacketHandler::FREE_CHANNEL;
    }

    return IPacketHandler::FREE_CHANNEL;
}

}//end disk
}//end lljz