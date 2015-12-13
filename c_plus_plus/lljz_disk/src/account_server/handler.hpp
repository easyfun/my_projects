#ifndef LLJZ_DISK_HANDLER_H
#define LLJZ_DISK_HANDLER_H

#include "base_packet.hpp"
#include "public_echo_test_req_handler.h"
#include "handler_router.hpp"

namespace lljz {
namespace disk {

void RegisterHandler() {
    HANDLER_ROUTER.RegisterHandler(
        PUBLIC_ECHO_TEST_REQ,PublicEchoTestReqHandler);
}

}
}

#endif