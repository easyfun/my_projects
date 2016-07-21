#ifndef LLJZ_DISK_PROTOCOL_H_
#define LLJZ_DISK_PROTOCOL_H_

namespace lljz {
namespace disk {

//uint16_t [1,65535]
//server type
enum ServerType {
   SERVER_TYPE_NONE=0,

   SERVER_TYPE_CONFIG_SERVER=1,   //配置服务
   SERVER_TYPE_ACCESS_SERVER=2,   //接入服务
   SERVER_TYPE_BUSINESS_SERVER=3, //业务服务

   SERVER_TYPE_CLIENT_LINUX=65000,  //linux客户端
   SERVER_TYPE_PYTHON_CLIENT_LINUX=65001  //linux python客户端
};

//public
enum PublicMsg {
   //回显调试
   PUBLIC_ECHO_TEST_REQ=0,
   PUBLIC_ECHO_TEST_RESP=1,

   //注册
   PUBLIC_REGISTER_REQ=2,
   PUBLIC_REGISTER_RESP=3
};

//account_server
enum BusinessServerMsg {
   //接入服务相关1000~4999
   
   //账号服务相关5000~24999
   ACCOUNT_SERVER_APP_LOGIN_REQ=5000,//
   ACCOUNT_SERVER_APP_LOGIN_RESP=5001,

   ACCOUNT_SERVER_REGISTER_ACCOUNT_REQ=5002,//注册账号
   ACCOUNT_SERVER_REGISTER_ACCOUNT_RESP=5003,

   ACCOUNT_SERVER_SET_ACCOUNT_INFO_REQ=5004,//设置账号信息
   ACCOUNT_SERVER_SET_ACCOUNT_INFO_RESP=5005,

   ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_REQ=5006,//修改登陆密码
   ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_RESP=5007,

   ACCOUNT_SERVER_LOGIN_REQ=5008,//登陆
   ACCOUNT_SERVER_LOGIN_RESP=5009,

   ACCOUNT_SERVER_LOGOUT_REQ=5060,//登出
   ACCOUNT_SERVER_LOGOUT_RESP=5061

};


}
}
#endif