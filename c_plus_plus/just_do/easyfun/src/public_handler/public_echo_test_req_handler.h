#ifndef PUBLIC_ECHO_TEST_REQ_HANDLER_H
#define PUBLIC_ECHO_TEST_REQ_HANDLER_H

namespace lljz {
namespace disk {

//PUBLIC_ECHO_TEST_REQ=0,

void publicEchoTestReqHandler(RequestPacket* req,
    void* args,ResponsePacket* resp);

}
}

#endif