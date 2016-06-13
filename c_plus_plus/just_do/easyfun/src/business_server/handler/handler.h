#ifndef LLJZ_DISK_ACCOUNT_HANDLER_H
#define LLJZ_DISK_ACCOUNT_HANDLER_H

/*
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "base_packet.hpp"
#include "public_echo_test_req_handler.h"
#include "handler_router.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "redis_client.h"
#include "redis_client_manager.h"
#include "global.h"

using namespace rapidjson;
*/

namespace lljz {
namespace disk {

/**************************************
 *global
**************************************/
extern RedisClientManager* g_account_redis;
extern RedisClientManager* g_file_redis;



/**************************************
 * module
**************************************/
void RegisterHandler();
bool InitDatabase();
bool InitHandler();
void WaitHandler();
void StopHandler();



/**************************************
 * handler
**************************************/
//M:ACCOUNT_SERVER_REGISTER_ACCOUNT_REQ
//注册账号
void registerAccountReq(RequestPacket* req,
    void* args,
    ResponsePacket* resp);


//M:ACCOUNT_SERVER_SET_ACCOUNT_INFO_REQ
//设置账号信息
void setAccountInfoReq(RequestPacket* req,
    void* args, 
    ResponsePacket* resp);


//M:ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_REQ
//修改登陆密码
void modifyLoginPasswordReq(RequestPacket* req,
    void* args, 
    ResponsePacket* resp);


//M:ACCOUNT_SERVER_LOGIN_REQ
//登陆
void loginReq(RequestPacket* req,
    void* args, 
    ResponsePacket* resp);


//M:ACCOUNT_SERVER_LOGOUT_REQ
//退出
void logoutReq(RequestPacket* req,
    void* args, 
    ResponsePacket* resp);


}
}

#endif