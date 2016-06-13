#ifndef LLJZ_DISK_ACCOUNT_SERVER_ENTRY_H_
#define LLJZ_DISK_ACCOUNT_SERVER_ENTRY_H_

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
};

}
}

#endif