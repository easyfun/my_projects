#include "connection_manager_to_server.hpp"
//#include "json/json.h"

namespace lljz {
namespace disk {

ConnectionManagerToServer::ConnectionManagerToServer(
tbnet::Transport* transport,
tbnet::IPacketStreamer* packet_streamer,
IBusinessPacketHandler* bph)
 : transport_(transport)
 , packet_handler_to_server_(this, bph)
 , packet_streamer_(packet_streamer)
 , stop_(false) {
    conn_to_config_server_=NULL;
}

ConnectionManagerToServer::~ConnectionManagerToServer() {
    destroy();
}

bool ConnectionManagerToServer::start() {

    const char * str_config_value = TBSYS_CONFIG.getString("server","config_server",NULL);
    if (NULL == str_config_value) {
        TBSYS_LOG(ERROR,"%s","config error,config_server url error");
        return false;
    }
    memset(config_server_spec_,0,
        sizeof(config_server_spec_));
    strcat(config_server_spec_,str_config_value);

    self_server_type_=TBSYS_CONFIG.getInt("server","self_server_type",
        SERVER_TYPE_ACCESS_SERVER);
    self_server_id_=TBSYS_CONFIG.getInt("server","self_server_id",0);
    str_config_value=TBSYS_CONFIG.getString("server","self_server_spec","");
    if (0==strlen(str_config_value)) {
        TBSYS_LOG(ERROR,"%s","config error,self_server_spec error");
        return false;
    }
    memset(self_server_spec_,0,
        sizeof(self_server_spec_));
    strcat(self_server_spec_,str_config_value);


    conn_to_config_server_=transport_->connect(
        config_server_spec_,packet_streamer_,true);
    if (NULL==conn_to_config_server_) {
        TBSYS_LOG(ERROR,"%s","connect to config_server error");
        return false;
    }
    conn_to_config_server_->setDefaultPacketHandler(this);

    timer_thread_.start(this,NULL);
    return true;
}

bool ConnectionManagerToServer::stop() {
    stop_=true;
    return true;
}

bool ConnectionManagerToServer::wait() {
    timer_thread_.join();
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
    RequestPacket* req=new RequestPacket;
    req->src_type_=self_server_type_;
    req->src_id_=self_server_id_;
    req->dest_type_=SERVER_TYPE_CONFIG_SERVER;
    req->dest_id_=0;
    req->msg_id_=CONFIG_SERVER_GET_SERVICE_LIST_REQ;

    Json::Value json_req;
    json_req["spec"]=self_server_spec_;
    json_req["srv_type"]=self_server_type_;
    json_req["srv_id"]=self_server_id_;
    Json::Value json_dep_srv_type=Json::Value(Json::arrayValue);
    Json::Value json_srv_type;
    int size=depend_server_type_.size();
    for (int i=0;i<size;i++) {
        json_srv_type=depend_server_type_[i];
        json_dep_srv_type.append(json_srv_type);
    }
    json_req["dep_srv_type"]=json_dep_srv_type;
    Json::FastWriter writer;
    std::string req_data=writer.write(json_req);
    strcat(req->data_,req_data.c_str());

    if (false==conn_to_config_server_->postPacket(req
        &packet_handler_to_server_,req)) {
        delete req;
    }
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
    return lcm->sendPacket(packet,&packet_handler_to_server_,args)
}

void ConnectionManagerToServer::DisToReConnect(
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

    lcm->DisToReConnect(server_id);
}


tbnet::HPRetCode ConnectionManagerToServer::handlePacket(
Packet *packet, void *args) {
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
            //discard
        } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
            assert(false);
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }

    return IPacketHandler::FREE_CHANNEL;
}

bool ConnectionManagerToServer::GetServiceListResp(
tbnet::Packet * apacket, void *args) {
    ResponsePacket* resp=(ResponsePacket* )apacket;
    if (0!=resp->error_code) {
        return true;
    }

    std::string spec;
    uint32_t server_type;
    uint32_t server_id;
    std::vector<ServerURL*> buff_server_url;
    bool server_changed;

    Json::Value json_data_root;
    Json::Reader reader;
    if (!reader.parse(resp->data_, json_data_root, false)) {
        return true;
    }

    int size=json_data_root["total"];
    int svr_url_size=server_url_.size();

    server_changed=false;
    //server_url_ check remove
    for (int i=0;i<svr_url_size;i++) {
        for (int j=0;j<size;j++) {
            spec=json_data_root["srv_info"][j]["spec"];
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
        spec=json_data_root["srv_info"][i]["spec"].asString();
        server_type=json_data_root["srv_info"][i]["server_type"].asInt();
        server_id=json_data_root["srv_info"][i]["server_id"].asInt();

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

    mutex_.lock();
    size=buff_server_url.size();
    for (int i=0;i<size;i++) {
        server_url_.push(buff_server_url[i])
    }
    buff_server_url.clear();

    if (!server_changed) {
        mutex_.unlock();
        return true;
    }

    //refresh connect manager
    __gnu_cxx::hash_map<uint16_t,
        LoadConnectionManager*>::iteraotr it;
    LoadConnectionManager* load_conn_manager=NULL;

    for (int i=0;i<server_url_.size();i++) {
        if (1==server_url_[i]->changed_) {
            //add a connection
            server_url_[i]->changed_=0;

            it=conn_manager_.find(
                server_url_[i]->server_type_);

            if (conn_manager_.end()!=it) {
                //load_conn_manager=it->second;
                assert(false);
                continue;
            }

            load_conn_manager=new LoadConnectionManager(
                transport_,
                packet_streamer_,
                &packet_handler_to_server_);//后端连接断开处理
            conn_manager_[server_url_[i]->server_type_]=
                load_conn_manager;
            load_conn_manager->connect(
                server_url_[i]->server_id_,false);
            }
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
            delete server_url_[i];
            server_url_.erase(server_url_.begin()+i);
            continue;
        }
        i++;
    }
    mutex_.unlock();

    return true;
}


}//end disk
}//end lljz