#include "tbsys.h"
#include "tbnet.h"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "handler_router.h"
#include "public_echo_test_req_handler.h"

namespace lljz {
namespace disk {

void PublicEchoTestReqHandler(
RequestPacket* req,
void* args,ResponsePacket* resp) {
    //strcat(resp->data_,req->data_);
}

}
}