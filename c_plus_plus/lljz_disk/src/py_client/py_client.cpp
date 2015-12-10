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
 *   lljzfly <1060887552@qq.com>
 *
 */

#include "tbnet.h"
//#include "json/json.h"
#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "packet_factory.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "py_client.h"

using namespace rapidjson;

namespace lljz {
namespace disk {

PyClient::PyClient()
: stop_(false) {

}

PyClient::~PyClient() {

}

bool PyClient::start() {
    stop_=false;
    packet_streamer_.setPacketFactory(&packet_factory_);
    loop_thread_.start(this,NULL);
    return true;
}

bool PyClient::stop() {
    stop_=true;
    transport_.stop();
    return true;
}

bool PyClient::wait() {
    loop_thread_.join();
    return true;
}

void PyClient::destroy() {
}

uint64_t PyClient::Connect(
std::string spec, bool auto_conn) {
    TBSYS_LOG(DEBUG,"spec=%s, auto_conn=%d",
        spec.c_str(), auto_conn);
    tbnet::Connection* conn=transport_.connect(
        spec.c_str(),
        &packet_streamer_,
        auto_conn);
    if (NULL==conn) {
        TBSYS_LOG(DEBUG,"%s","connect fail");
        return 0;
    }
    conn->setDefaultPacketHandler(this);
    conn->setQueueTimeout(30000);//30s
    conn->setQueueLimit(0);
    
    ConnectMap::iterator it;
    mutex_.lock();
    it=conn_map_.find((uint64_t)conn);
    if (conn_map_.end()==it) {
        conn_map_[(uint64_t)conn]=conn;
        mutex_.unlock();
        TBSYS_LOG(DEBUG,"1 connect success, conn_id=%llu",(uint64_t)conn);
        return (uint64_t)conn;
    }

    tbnet::Connection* old_conn=it->second;
    conn_map_[(uint64_t)conn]=conn;
    transport_.disconnect(old_conn);
    mutex_.unlock();
    TBSYS_LOG(DEBUG,"2 connect success, conn_id=%llu",(uint64_t)conn);
    return (uint64_t)conn;
}

void PyClient::Disconnect(uint64_t conn_id) {
    tbnet::Connection* conn;
    ConnectMap::iterator it;
    mutex_.lock();
    it=conn_map_.find(conn_id);
    if (conn_map_.end()==it) {
        mutex_.unlock();
        return;
    }

    conn=it->second;
    conn_map_.erase(it);
    transport_.disconnect(conn);
    mutex_.unlock();
}

/*
request
{
    "src_type":0,
    "src_id":0,
    "dest_type":0,
    "dest_id":0,
    "msg_id":0,
    "version":0,
    "data":{}
}

response
{
    "src_type":0,
    "src_id":0,
    "dest_type":0,
    "dest_id":0,
    "msg_id":0,
    "error_code":0,
    "data":{}
}
*/

std::string PyClient::Send(
uint64_t conn_id, std::string req_str) {
    std::string resp_str;
    tbnet::Connection* conn;
    ConnectMap::iterator it;

    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> writer(resp_buffer);

    Value resp_json(kObjectType);
    Value resp_json_src_type(kNumberType);
    Value resp_json_src_id(kNumberType);
    Value resp_json_dest_type(kNumberType);
    Value resp_json_dest_id(kNumberType);
    Value resp_json_msg_id(kNumberType);
    Value resp_json_error_code(kNumberType);
    Value resp_json_data(kStringType);
    RequestPacket* req_packet;
    req_packet=new RequestPacket();
    req_doc.Parse(req_str.c_str());
    req_packet->src_type_=req_doc["src_type"].GetInt();
    req_packet->src_id_=req_doc["src_id"].GetUint64();
    req_packet->dest_type_=req_doc["dest_type"].GetInt();
    req_packet->dest_id_=req_doc["dest_id"].GetUint64();
    req_packet->version_=req_doc["version"].GetInt();
    req_packet->msg_id_=req_doc["msg_id"].GetInt();
    std::string tmp_str=req_doc["data"].GetString();
    sprintf(req_packet->data_,"%s",tmp_str.c_str());

    resp_json_src_type=req_packet->dest_type_;
    resp_json_src_id=req_packet->dest_id_;
    resp_json_dest_type=req_packet->src_type_;
    resp_json_dest_id=req_packet->src_id_;
    resp_json_msg_id=req_packet->msg_id_+1;
    resp_json_error_code=0;

    mutex_.lock();
    it=conn_map_.find(conn_id);
    if (conn_map_.end()==it) {
        mutex_.unlock();
        delete req_packet;

        resp_json_error_code=1;
        resp_json.AddMember("src_type",resp_json_src_type,resp_allocator);
        resp_json.AddMember("src_id",resp_json_src_id,resp_allocator);
        resp_json.AddMember("dest_type",resp_json_dest_type,resp_allocator);
        resp_json.AddMember("dest_id",resp_json_dest_id,resp_allocator);
        resp_json.AddMember("msg_id",resp_json_msg_id,resp_allocator);
        resp_json.AddMember("error_code",resp_json_error_code,resp_allocator);
        resp_json.AddMember("data",resp_json_data,resp_allocator);
        resp_json.Accept(writer);
        resp_str=resp_buffer.GetString();
        return resp_str;
    }

    conn=it->second;
    mutex_.unlock();

    if (NULL==conn) {
        delete req_packet;

        resp_json_error_code=1;
        resp_json.AddMember("src_type",resp_json_src_type,resp_allocator);
        resp_json.AddMember("src_id",resp_json_src_id,resp_allocator);
        resp_json.AddMember("dest_type",resp_json_dest_type,resp_allocator);
        resp_json.AddMember("dest_id",resp_json_dest_id,resp_allocator);
        resp_json.AddMember("msg_id",resp_json_msg_id,resp_allocator);
        resp_json.AddMember("error_code",resp_json_error_code,resp_allocator);
        resp_json.AddMember("data",resp_json_data,resp_allocator);
        resp_json.Accept(writer);
        resp_str=resp_buffer.GetString();
        return resp_str;
    }

    ConnectCond conn_cond;
    conn_cond.cond_.lock();
    conn_cond.send_=0;
    if (false == conn_cond.conn_->postPacket(
                    req_packet,NULL,&conn_cond)) {
        conn_cond.cond_.unlock();
        delete req_packet;
        transport_.disconnect(conn);

        resp_json_error_code=3;
        resp_json.AddMember("src_type",resp_json_src_type,resp_allocator);
        resp_json.AddMember("src_id",resp_json_src_id,resp_allocator);
        resp_json.AddMember("dest_type",resp_json_dest_type,resp_allocator);
        resp_json.AddMember("dest_id",resp_json_dest_id,resp_allocator);
        resp_json.AddMember("msg_id",resp_json_msg_id,resp_allocator);
        resp_json.AddMember("error_code",resp_json_error_code,resp_allocator);
        resp_json.AddMember("data",resp_json_data,resp_allocator);
        resp_json.Accept(writer);
        resp_str=resp_buffer.GetString();
        return resp_str;
    }

    while (0==conn_cond.send_) {
        conn_cond.cond_.wait(1000);
    }
    conn_cond.cond_.unlock();

    if (2==conn_cond.send_) {
        resp_json_error_code=5;//发送超时
    } else {
        resp_json_error_code=conn_cond.resp_.error_code_;
    }
    resp_json_data.SetString(conn_cond.resp_.data_, 
        strlen(conn_cond.resp_.data_),resp_allocator);
    resp_json.AddMember("src_type",resp_json_src_type,resp_allocator);
    resp_json.AddMember("src_id",resp_json_src_id,resp_allocator);
    resp_json.AddMember("dest_type",resp_json_dest_type,resp_allocator);
    resp_json.AddMember("dest_id",resp_json_dest_id,resp_allocator);
    resp_json.AddMember("msg_id",resp_json_msg_id,resp_allocator);
    resp_json.AddMember("error_code",resp_json_error_code,resp_allocator);
    resp_json.AddMember("data",resp_json_data,resp_allocator);
    resp_json.Accept(writer);
    resp_str=resp_buffer.GetString();
    return resp_str;
}

std::string PyClient::Send(std::string req_str) {
    std::string resp_str;
    ConnectMap::iterator it;

    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> writer(resp_buffer);

    Value resp_json(kObjectType);
    Value resp_json_src_type(kNumberType);
    Value resp_json_src_id(kNumberType);
    Value resp_json_dest_type(kNumberType);
    Value resp_json_dest_id(kNumberType);
    Value resp_json_msg_id(kNumberType);
    Value resp_json_error_code(kNumberType);
    Value resp_json_data(kStringType);
    RequestPacket* req_packet;
    req_packet=new RequestPacket();
    req_doc.Parse(req_str.c_str());
    std::string spec=req_doc["spec"].GetString();
    req_packet->src_type_=req_doc["src_type"].GetInt();
    req_packet->src_id_=req_doc["src_id"].GetUint64();
    req_packet->dest_type_=req_doc["dest_type"].GetInt();
    req_packet->dest_id_=req_doc["dest_id"].GetUint64();
    req_packet->version_=req_doc["version"].GetInt();
    req_packet->msg_id_=req_doc["msg_id"].GetInt();
    std::string tmp_str=req_doc["data"].GetString();
    sprintf(req_packet->data_,"%s",tmp_str.c_str());

    resp_json_src_type=req_packet->dest_type_;
    resp_json_src_id=req_packet->dest_id_;
    resp_json_dest_type=req_packet->src_type_;
    resp_json_dest_id=req_packet->src_id_;
    resp_json_msg_id=req_packet->msg_id_+1;
    resp_json_error_code=0;

    //连接
    tbnet::Connection* conn=transport_.connect(
        spec.c_str(),&packet_streamer_,true);
    if (NULL==conn) {
        delete req_packet;

        resp_json_error_code=4;
        resp_json.AddMember("src_type",resp_json_src_type,resp_allocator);
        resp_json.AddMember("src_id",resp_json_src_id,resp_allocator);
        resp_json.AddMember("dest_type",resp_json_dest_type,resp_allocator);
        resp_json.AddMember("dest_id",resp_json_dest_id,resp_allocator);
        resp_json.AddMember("msg_id",resp_json_msg_id,resp_allocator);
        resp_json.AddMember("error_code",resp_json_error_code,resp_allocator);
        resp_json.AddMember("data",resp_json_data,resp_allocator);
        resp_json.Accept(writer);
        resp_str=resp_buffer.GetString();
        return resp_str;
    }
    conn->setDefaultPacketHandler(this);
    conn->setQueueTimeout(30000);//30s
    conn->setQueueLimit(0);

    ConnectCond conn_cond;
    conn_cond.cond_.lock();
    conn_cond.send_=0;
    if (false == conn_cond.conn_->postPacket(
                    req_packet,NULL,&conn_cond)) {
        conn_cond.cond_.unlock();
        delete req_packet;
        transport_.disconnect(conn);

        resp_json_error_code=3;
        resp_json.AddMember("src_type",resp_json_src_type,resp_allocator);
        resp_json.AddMember("src_id",resp_json_src_id,resp_allocator);
        resp_json.AddMember("dest_type",resp_json_dest_type,resp_allocator);
        resp_json.AddMember("dest_id",resp_json_dest_id,resp_allocator);
        resp_json.AddMember("msg_id",resp_json_msg_id,resp_allocator);
        resp_json.AddMember("error_code",resp_json_error_code,resp_allocator);
        resp_json.AddMember("data",resp_json_data,resp_allocator);
        resp_json.Accept(writer);
        resp_str=resp_buffer.GetString();
        return resp_str;
    }

    while (0==conn_cond.send_) {
        conn_cond.cond_.wait(1000);
    }
    conn_cond.cond_.unlock();

    transport_.disconnect(conn);
    if (2==conn_cond.send_) {
        resp_json_error_code=5;//发送超时
    } else {
        resp_json_error_code=conn_cond.resp_.error_code_;
    }
    resp_json_data.SetString(conn_cond.resp_.data_, 
                        strlen(conn_cond.resp_.data_),
                        resp_allocator);
    resp_json.AddMember("src_type",resp_json_src_type,resp_allocator);
    resp_json.AddMember("src_id",resp_json_src_id,resp_allocator);
    resp_json.AddMember("dest_type",resp_json_dest_type,resp_allocator);
    resp_json.AddMember("dest_id",resp_json_dest_id,resp_allocator);
    resp_json.AddMember("msg_id",resp_json_msg_id,resp_allocator);
    resp_json.AddMember("error_code",resp_json_error_code,resp_allocator);
    resp_json.AddMember("data",resp_json_data,resp_allocator);
    resp_json.Accept(writer);
    resp_str=resp_buffer.GetString();
    return resp_str;
}


//Runnable
void PyClient::run(tbsys::CThread* thread, void* arg) {
    transport_.start();
    transport_.wait();
}


tbnet::IPacketHandler::HPRetCode 
PyClient::handlePacket(
tbnet::Packet *packet, void *args) {
    ConnectCond* conn_cond;
    ResponsePacket* resp;
    
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
            TBSYS_LOG(DEBUG,"%s",
                "TestClientPacketHandler::handlePacket:DISCONN");
            return IPacketHandler::FREE_CHANNEL;
        } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
            TBSYS_LOG(DEBUG,"%s",
                "TestClientPacketHandler::handlePacket:TIMEOUT");
            conn_cond=(ConnectCond* )args;
            conn_cond->cond_.lock();
            conn_cond->send_=2;//发送超时
            conn_cond->cond_.signal();
            conn_cond->cond_.unlock();
        }

        return IPacketHandler::FREE_CHANNEL;
    }
    
    resp = (ResponsePacket*)packet;
    TBSYS_LOG(DEBUG,"resp:chanid=%u|pcode=%u|msg_id=%u|src_type=%u|"
        "src_id=%u|dest_type=%u|dest_id=%u|data=%s",
        resp->getChannelId(),resp->getPCode(),resp->msg_id_,
        resp->src_type_,resp->src_id_,resp->dest_type_,
        resp->dest_id_,resp->data_);
    conn_cond=(ConnectCond* )args;
    conn_cond->cond_.lock();
    conn_cond->send_=1;//发送成功
    conn_cond->resp_.src_type_=resp->src_type_;
    conn_cond->resp_.src_id_=resp->src_id_;
    conn_cond->resp_.dest_type_=resp->dest_type_;
    conn_cond->resp_.dest_id_=resp->dest_id_;
    conn_cond->resp_.msg_id_=resp->msg_id_;
    conn_cond->resp_.error_code_=resp->error_code_;
    sprintf(conn_cond->resp_.data_, "%s",resp->data_);
    conn_cond->cond_.signal();
    conn_cond->cond_.unlock();
       
    return IPacketHandler::FREE_CHANNEL;
}

}
}


