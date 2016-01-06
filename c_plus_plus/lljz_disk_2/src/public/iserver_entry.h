#ifndef LLJZ_DISK_ISERVER_ENTRY_H_
#define LLJZ_DISK_ISERVER_ENTRY_H_

namespace lljz {
namespace disk {

class IServerEntry {
public:
    IServerEntry() {}
    virtual ~IServerEntry() {}
    virtual bool start()=0;
    virtual bool stop()=0;
};

}
}

#endif