#include "account_server_handler.h"

namespace lljz {
namespace disk {

void LoginReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);

    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);

    //检查account
    if (!req_doc.HasMember("account") || !req_doc["account"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
    if (0==req_doc["account"].GetStringLength()) {
        resp->error_code_=20001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    //检查password
    if (!req_doc.HasMember("password") || !req_doc["password"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
   
    if (0==req_doc["password"].GetStringLength()) {
        resp->error_code_=20001;
        resp_error_msg="password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    RedisClient* rc=REDIS_CLIENT_MANAGER.GetRedisClient();
    char cmd[512];
    redisReply* reply;
    sprintf(cmd,"HGET %s password", req_doc["account"].GetString());
    int cmd_ret=Rhget(rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        REDIS_CLIENT_MANAGER.ReleaseRedisClient(rc,cmd_ret);
        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有字段password
        //记录异常日志
        REDIS_CLIENT_MANAGER.ReleaseRedisClient(rc,cmd_ret);
        resp->error_code_=20002;
        resp_error_msg="redis database server status error";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    if (0!=strcmp(req_doc["password"].GetString(),reply->str)) {
        REDIS_CLIENT_MANAGER.ReleaseRedisClient(rc,cmd_ret);
        resp->error_code_=20005;
        resp_error_msg="login password is wrong";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        freeReplyObject(reply);
        return;
    }
    freeReplyObject(reply);

    REDIS_CLIENT_MANAGER.ReleaseRedisClient(rc,true);

    resp->error_code_=0;
    resp_error_msg="";
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

    resp_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());
}


}
}