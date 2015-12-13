#include "handler_router.hpp"

namespace lljz {
namespace disk {

static HandlerRouter g_handler_router;

HandlerRouter::HandlerRouter() {

}

HandlerRouter::~HandlerRouter() {

}

bool HandlerRouter::RegisterHandler(
uint32_t msg_id, Handler handler) {
    bool ret=true;
    HandlerMap::iterator it;
    it=handler_map_.find(msg_id);
    if (handler_map_.end() == it) {
        ret=false;
    }
    handler_map_[msg_id]=handler;
    return ret;
}

Handler HandlerRouter::GetHandler(uint32_t msg_id) {
    Handler handler=NULL;
    HandlerMap::iterator it;
    it=handler_map_.find(msg_id);
    if (handler_map_.end() != it) {
        handler=it->second;
    }
    return handler;
}

HandlerRouter& HandlerRouter::GetHandlerRouter() {
    return g_handler_router;
}


}
}