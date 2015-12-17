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
    std::string resp_data;

    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);
    std::string req_account;
    std::string req_old_password;
    std::string req_new_password;

    //检查account
    if (!req_doc.HasMember("account") || !req_doc["account"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }
    req_account=req_doc["account"].GetString();
    if (req_account.empty()) {
        resp->error_code_=20001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    //检查old_password
    if (req_doc.HasMember("old_password")) {
        if (!req_doc["old_password"].IsString()) {
            resp->error_code_=20001;
            resp_error_msg="old_password is invalid";
            resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

            resp_json.Accept(resp_writer);
            resp_data=resp_buffer.GetString();
            sprintf(resp->data_,"%s",resp_data.c_str());
            return;
        }
        req_old_password=req_doc["old_password"].GetString();
    }
    
    //检查new_password
    if (req_doc.HasMember("new_password")) {
        if (!req_doc["new_password"].IsString()) {
            resp->error_code_=20001;
            resp_error_msg="new_password is invalid";
            resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

            resp_json.Accept(resp_writer);
            resp_data=resp_buffer.GetString();
            sprintf(resp->data_,"%s",resp_data.c_str());
            return;
        }
        req_new_password=req_doc["new_password"].GetString();
    }

    if (req_old_password.empty() || req_new_password.empty()) {
        resp->error_code_=20001;
        resp_error_msg="new_password,old_password can not be empty";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    //判断账号是否存在
    RedisClient* rc=REDIS_CLIENT_MANAGER.GetRedisClient();
    char cmd[512];
    sprintf(cmd,"EXISTS %s", req_account.c_str());
        redisReply* reply;
    int cmd_ret=Rexists(rc,cmd,reply);
    if (SUCCESS_ACTIVE != cmd_ret) {
        REDIS_CLIENT_MANAGER.ReleaseRedisClient(rc,cmd_ret);

        if (FAILED_ACTIVE==cmd_ret) {
            resp->error_code_=20003;
            resp_error_msg="account does not exist";
        } else {
            resp->error_code_=20002;
            resp_error_msg="redis database server is busy";
        }
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,"HMSET %s",req_account.c_str());
    char fv[512];
    if (!req_img_url.empty()) {
        sprintf(fv," img_url %s",req_img_url.c_str());
        strcat(cmd,fv);
    }

    if (!req_home_page.empty()) {
        sprintf(fv," home_page %s",req_home_page.c_str());
        strcat(cmd,fv);
    }

    sprintf(fv," timestamp %lld",tbsys::CTimeUtil::getTime());
    strcat(cmd,fv);
    cmd_ret=Rhmset(rc,cmd,reply);
    if (SUCCESS_ACTIVE != cmd_ret) {
        REDIS_CLIENT_MANAGER.ReleaseRedisClient(rc,cmd_ret);

        if (FAILED_ACTIVE==cmd_ret) {
            resp->error_code_=20004;
            resp_error_msg="set account info fail";
        } else {
            resp->error_code_=20002;
            resp_error_msg="redis database server is busy";
        }
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    REDIS_CLIENT_MANAGER.ReleaseRedisClient(rc,true);

    resp->error_code_=0;
    resp_error_msg="";
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

    resp_json.Accept(resp_writer);
    resp_data=resp_buffer.GetString();
    sprintf(resp->data_,"%s",resp_data.c_str());

}

}
}