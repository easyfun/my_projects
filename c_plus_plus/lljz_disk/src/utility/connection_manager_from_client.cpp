#include "connection_manager_from_client.hpp"

namespace lljz {
namespace disk {

ConnectionManagerFromClient::ConnectionManagerFromClient() {

}

ConnectionManagerFromClient::~ConnectionManagerFromClient() {

}

bool RegisterConnection::RegisterConnection(tbnet::Connection* connection) {
    __gnu_cxx::hash_map<tbnet::Connection*,ConnectionInfo*>::iterator it;
    ConnectionInfo* connInfo=NULL;

    connMapMutex_.lock();
    it=connectionMap_.find(connection);
    if (connectionMap_.end() != it) {
        connInfo=it->second;
        connMapMutex_.unlock();
        return true;
    }

    if (!connection->isConnectState())
        return false;

    connInfo=new ConnectionInfo;
    connInfo->connection_=connection;
    connInfo->state_=true;
    connInfo->addCount();
    connectionMap_[connection]=connInfo;

    connMapMutex_.unlock();

    return true;
}


bool ConnectionManagerFromClient::PostPacket(tbnet::Packet* packet) {
    bool ret=true;
    ConnectionInfo* connInfo=NULL:
    tbnet::Connection* conn=connInfo->connection_;
    tbnet::IOComponet* ioc=conn->getIOComponent();
    if (NULL != ioc) 
        ioc->addRef();
    if (false == (ret=connInfo->connection_->postPakcet(packet))) {
        if (NULL != ioc) 
            ioc->subRef();
    }
    if (NULL != ioc) 
        ioc->subRef();
    connInfo->subCount();
    connIDMutex_.unlock();

    return ret;
}


}
}