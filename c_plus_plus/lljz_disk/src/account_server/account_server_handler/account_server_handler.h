#ifndef LLJZ_DISK_ACCOUNT_HANDLER_H
#define LLJZ_DISK_ACCOUNT_HANDLER_H

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "redis_client.h"
#include "redis_client_manager.h"

using namespace rapidjson;

namespace lljz {
namespace disk {

//M:ACCOUNT_SERVER_REGISTER_ACCOUNT_REQ
//注册账号
void RegisterAccountReq(RequestPacket* req,
    void* args,
    ResponsePacket* resp);


//M:ACCOUNT_SERVER_SET_ACCOUNT_INFO_REQ
//设置账号信息
void SetAccountInfoReq(RequestPacket* req,
    void* args, 
    ResponsePacket* resp);

//M:ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_REQ
//修改登陆密码
void ModifyLoginPasswordReq(RequestPacket* req,
    void* args, 
    ResponsePacket* resp);

//M:ACCOUNT_SERVER_LOGIN_REQ
//登陆
void LoginReq(RequestPacket* req,
    void* args, 
    ResponsePacket* resp);

//M:ACCOUNT_SERVER_LOGOUT_REQ
//退出
void LogoutReq(RequestPacket* req,
    void* args, 
    ResponsePacket* resp);



//获取文件列表
//上传文件
//下载文件
//删除文件
//修改文件名称
//新建文件夹

}
}

#endif