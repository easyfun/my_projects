#include "account_server_handler.h"

namespace lljz {
namespace disk {

void SetAccountInfoReq(RequestPacket* req,
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
    std::string req_img_url;
    std::string req_home_page;

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

    //检查img_url
    if (req_doc.HasMember("img_url")) {
        if (!req_doc["img_url"].IsString()) {
            resp->error_code_=20001;
            resp_error_msg="img_url is invalid";
            resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

            resp_json.Accept(resp_writer);
            resp_data=resp_buffer.GetString();
            sprintf(resp->data_,"%s",resp_data.c_str());
            return;
        }
        req_img_url=req_doc["img_url"].GetString();
    }
    
    //检查home_page
    if (req_doc.HasMember("home_page")) {
        if (!req_doc["home_page"].IsString()) {
            resp->error_code_=20001;
            resp_error_msg="home_page is invalid";
            resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

            resp_json.Accept(resp_writer);
            resp_data=resp_buffer.GetString();
            sprintf(resp->data_,"%s",resp_data.c_str());
            return;
        }
        req_home_page=req_doc["home_page"].GetString();
    }

    if (req_img_url.empty() && req_home_page.empty()) {
        resp->error_code_=0;
        resp_error_msg="modify nothing";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }
    //判断账号是否存在
    RedisClient* rc=g_account_redis->GetRedisClient();
    char cmd[512];
    sprintf(cmd,"EXISTS %s", req_account.c_str());
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
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);

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

    g_account_redis->ReleaseRedisClient(rc,true);

    resp->error_code_=0;
    resp_error_msg="";
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

    resp_json.Accept(resp_writer);
    resp_data=resp_buffer.GetString();
    sprintf(resp->data_,"%s",resp_data.c_str());

}

}
}