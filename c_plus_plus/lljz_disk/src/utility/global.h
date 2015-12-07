#ifndef LLJZ_DISK_GLOBAL_H
#define LLJZ_DISK_GLOBAL_H

#include "tbsys.h"
#include "tbnet.h"

namespace lljz {
namespace disk {

int ParseAddr(char *src, char **args, int cnt);

uint64_t GetServerID(const char* spec);

}
}

#endif