#include "shared.h"
#include "access_server.h"

namespace lljz {
namespace disk {

AccessServer::AccessServer() {

}

AccessServer::~AccessServer() {

}

bool AccessServer::start() {
    manager_client_.set_manager_server(&manager_server_);
    manager_server_.set_manager_client(&manager_client_);

    manager_client_.set_channel_pool(&channel_pool_);
    manager_server_.set_channel_pool(&channel_pool_);

    if (!manager_client_.start()) {
        return false;
    }

    if (!manager_server_.start()) {
        manager_client_.stop();
        manager_client_.wait();
        return false;
    }

    manager_client_.wait();
    manager_server_.wait();

    return true;
}

bool AccessServer::stop() {
    manager_client_.stop();
    manager_server_.stop();
    return true;
}


}
}