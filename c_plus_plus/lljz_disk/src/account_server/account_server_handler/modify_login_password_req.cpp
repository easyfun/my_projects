#include "account_server_handler.h"

namespace lljz {
namespace disk {

void ModifyLoginPasswordReq(RequestPacket* req,
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

    //检查old_password
    if (!req_doc.HasMember("old_password") || !req_doc["old_password"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="old_password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
    
    //检查new_password
    if (!req_doc.HasMember("new_password") || !req_doc["new_password"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="new_password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    if (0==req_doc["new_password"].GetStringLength() 
        || 0==req_doc["old_password"].GetStringLength()) {
        resp->error_code_=20001;
        resp_error_msg="new_password,old_password can not be empty";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    //判断账号是否存在
    RedisClient* rc=g_account_redis->GetRedisClient();
    char cmd[512];
    sprintf(cmd,"EXISTS %s", req_doc["account"].GetString());
    redisReply* reply;
    int cmd_ret=Rexists(rc,cmd,reply);
    if (SUCCESS_ACTIVE != cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);

        if (FAILED_ACTIVE==cmd_ret) {
            resp->error_code_=20003;
            resp_error_msg="account does not exist";
        } else {
            resp->error_code_=20002;
            resp_error_msg="redis database server is busy";
        }
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    sprintf(cmd,"HGET %s password", req_doc["account"].GetString());
    cmd_ret=Rhget(rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);
        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有字段password，允许直接设置密码
    } else {
        if (0!=strcmp(req_doc["old_password"].GetString(),reply->str)) {
            g_account_redis->ReleaseRedisClient(rc,cmd_ret);
            resp->error_code_=20005;
            resp_error_msg="login password is wrong";
            resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

            resp_json.Accept(resp_writer);
            sprintf(resp->data_,"%s",resp_buffer.GetString());
            freeReplyObject(reply);
            return;
        }
    }
    freeReplyObject(reply);
    if (0==strcmp(req_doc["old_password"].GetString(),
                req_doc["new_password"].GetString()) ) {//新密码不变
        g_account_redis->ReleaseRedisClient(rc,true);

        resp->error_code_=0;
        resp_error_msg="";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,"HSET %s password %s",
        req_doc["account"].GetString(),
        req_doc["new_password"].GetString());
    cmd_ret=Rhset(rc,cmd,reply);
    if (SUCCESS_ACTIVE != cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);

        resp->error_code_=20006;
        resp_error_msg="modify login password fail";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    g_account_redis->ReleaseRedisClient(rc,true);

    resp->error_code_=0;
    resp_error_msg="";
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

    resp_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());
}

}
}