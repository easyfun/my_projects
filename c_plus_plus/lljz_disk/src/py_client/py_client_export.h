#ifndef LLJZ_DISK_PY_CLIENT_EXPORT_H
#define LLJZ_DISK_PY_CLIENT_EXPORT_H

#include "tbsys.h"
#include "tbnet.h"

//#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>

namespace lljz {
namespace disk {

bool Init();
void Stop();
void Wait(); 

uint64_t Connect(std::string spec, bool auto_conn);
void Disconnect(uint64_t conn_id);
std::string Send(uint64_t conn_id, std::string req_str);
std::string SendOnce(std::string req);

}
}
#endif