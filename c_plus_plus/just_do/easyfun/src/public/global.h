#ifndef LLJZ_DISK_GLOBAL_H
#define LLJZ_DISK_GLOBAL_H

/*
#include "tbsys.h"
#include "tbnet.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"

using namespace rapidjson;
*/

namespace lljz {
namespace disk {

int ParseAddr(char *src, char **args, int cnt);

uint64_t GetServerID(const char* spec);

void SetErrorMsg(uint32_t error_code, 
    const char* error_msg, ResponsePacket* resp);

int GetCharCount(const char* str, char c);

void GetStrValue(const char* str, 
    char c,int index, char* value);

//openssl


}
}

#endif