#ifndef LLJZ_DISK_ACCESS_SERVER_ENTRY_H_
#define LLJZ_DISK_ACCESS_SERVER_ENTRY_H_

namespace lljz {
namespace disk {

class ServerEntry : public IServerEntry {
public:
    ServerEntry();
    virtual ~ServerEntry();

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