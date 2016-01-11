#include "shared.h"

namespace lljz {
namespace disk {

void DeleteFileOrFolderReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    SetErrorMsg(0,"",resp);
}


}
}