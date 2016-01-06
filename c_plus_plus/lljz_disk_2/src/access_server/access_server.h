#ifndef LLJZ_DISK_ACCESS_SERVER_H_
#define LLJZ_DISK_ACCESS_SERVER_H_

namespace lljz {
namespace disk {

/*
class ManagerClient;
class ManagerServer;
class IServerEntry;
*/

class AccessServer : public IServerEntry {
public:
    AccessServer();
    virtual ~AccessServer();

    bool start();
    bool stop();

private:
    ManagerClient manager_client_;
    ManagerServer manager_server_;
    tbnet::ChannelPool channel_pool_;
};

}
}

#endif