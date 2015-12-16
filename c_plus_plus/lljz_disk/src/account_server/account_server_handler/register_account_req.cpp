#include "account_server_handler.h"

namespace lljz {
namespace disk {

void RegisterAccountReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);
    std::string resp_data;

    req_doc.Parse(req->data_);
    std::string req_account;
    std::string req_password;

    //检查account
    if (!req_doc.HasMember("account")) {
        resp->error_code_=20001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    if (!req_doc["account"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }
    req_account=req_doc["account"].GetString();

    //检查password
    if (!req_doc.HasMember("password")) {
        resp->error_code_=20001;
        resp_error_msg="password is invalid"
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }
    if (!req_doc["password"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="password is invalid"
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }
    req_password=req_doc["password"].GetString();

    if (req_account.empty() || req_password.empty()) {
        resp->error_code_=20001;
        resp_error_msg="account,password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    //增加账号
    RedisClient* rc=REDIS_CLIENT_MANAGER.GetRedisClient();
    if (NULL==rc || NULL==rc->redis_content_) {
        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    char cmd[200];
    sprintf(cmd,"HSETNX %s account %s", 
        req_account.c_str(), req_account.c_str());
    redisReply* reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        manager->ReleaseRedisClient(rc,false);

        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    if( !(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok")==0)) {
        freeReplyObject(reply);
        manager->ReleaseRedisClient(rc,false);

        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    sprintf(cmd,"HMSET %s password \"%s\" img_url \"\" home_page \"\"",
        req_account.c_str(),req_password.c_str());
    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        manager->ReleaseRedisClient(rc,false);

        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    if( !(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok")==0)) {
        freeReplyObject(reply);
        manager->ReleaseRedisClient(rc,false);

        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    freeReplyObject(reply);
    manager->ReleaseRedisClient(rc,true);

    resp_json.Accept(resp_writer);
    resp_data=resp_buffer.GetString();
    sprintf(resp->data_,"%s",resp_data.c_str());
}

}
}
