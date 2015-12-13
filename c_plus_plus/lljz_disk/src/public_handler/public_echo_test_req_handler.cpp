#include "public_echo_test_req_handler.h"

namespace lljz {
namespace disk {

void PublicEchoTestReqHandler(
RequestPacket* req,
void* args,ResponsePacket* resp) {
    strcat(resp->data_,req->data_);
}

}
}