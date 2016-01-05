#include "load_connection.h"

namespace lljz {
namespace disk {

LoadConnection::LoadConnection(uint16_t server_type)
:server_type_(server_type) {

}

LoadConnection::~LoadConnection() {

}

bool LoadConnection::Connect(tbnet::Transport* transport) {
    return true;
}



}
}