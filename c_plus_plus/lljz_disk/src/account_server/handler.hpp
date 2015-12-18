#ifndef LLJZ_DISK_HANDLER_H
#define LLJZ_DISK_HANDLER_H

#include "base_packet.hpp"
#include "public_echo_test_req_handler.h"
#include "handler_router.hpp"
#include "account_server_handler.h"

namespace lljz {
namespace disk {

void RegisterHandler() {
    HANDLER_ROUTER.RegisterHandler(PUBLIC_ECHO_TEST_REQ,PublicEchoTestReqHandler);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_REGISTER_ACCOUNT_REQ,RegisterAccountReq);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_SET_ACCOUNT_INFO_REQ,SetAccountInfoReq);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_REQ,ModifyLoginPasswordReq);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_LOGIN_REQ,LoginReq);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_LOGOUT_REQ,LogoutReq);

}

}
}

#endif