#ifndef PUBLIC_ECHO_TEST_REQ_HANDLER_H
#define PUBLIC_ECHO_TEST_REQ_HANDLER_H

#include "handler_router.hpp"

namespace lljz {
namespace disk {

//PUBLIC_ECHO_TEST_REQ=0,

void PublicEchoTestReqHandler(RequestPacket* req,
    void* args,ResponsePacket* resp);

}
}

#endif