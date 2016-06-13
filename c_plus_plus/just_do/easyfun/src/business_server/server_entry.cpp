#include "shared.h"
#include "server_entry.h"

namespace lljz {
namespace disk {

ServerEntry::ServerEntry() {

}

ServerEntry::~ServerEntry() {

}

bool ServerEntry::start() {
    if (!manager_client_.start()) {
        return false;
    }

    manager_client_.wait();
    return true;
}

bool ServerEntry::stop() {
    manager_client_.stop();
    return true;
}


}
}