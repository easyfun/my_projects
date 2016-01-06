#include "tbsys.h"
#include "tbnet.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "global.h"

using namespace rapidjson;

namespace lljz {
namespace disk {

/*
 * 把[upd|tcp]:ip:port分开放在args中
 *
 * @param src: 源格式
 * @param args: 目标数组
 * @param   cnt: 数组中最大个数
 * @return  返回的数组中个数
 */
int ParseAddr(char *src, char **args, int cnt) {
    int index = 0;
    char *prev = src;

    while (*src) {
        if (*src == ':') {
            *src = '\0';
            args[index++] = prev;

            if (index >= cnt) { // 数组满了,返回
                return index;
            }

            prev = src + 1;
        }

        src ++;
    }

    args[index++] = prev;

    return index;
}

uint64_t GetServerID(const char* spec) {
    uint64_t server_id=0;

    char tmp[1024];
    char *args[32];
    strncpy(tmp, spec, 1024);
    tmp[1023] = '\0';

    if (ParseAddr(tmp, args, 32) != 3) {
        return server_id;
    }

    if (strcasecmp(args[0], "tcp") == 0) {
        char *host = args[1];
        int port = atoi(args[2]);
        char c;
        const char *p = host;
        bool isIPAddr = true;
        // 是ip地址格式吗?
        while ((c = (*p++)) != '\0') {
            if ((c != '.') && (!((c >= '0') && (c <= '9')))) {
                isIPAddr = false;
                break;
            }
        }

        if (isIPAddr) {
            server_id = port;
            server_id <<= 32;
            server_id |= inet_addr(host);
        }
    }

    return server_id;
}


void SetErrorMsg(uint32_t error_code, 
const char* error_msg, ResponsePacket* resp) {
    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);

    resp->error_code_=error_code;
    resp_error_msg.SetString(error_msg,strlen(error_msg),resp_allocator);
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

    resp_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());
}

int GetCharCount(const char* str, char c) {
    int n=0;
    while (*str) {
        if (*str==c)
            n++;
        str++;
    }
    return n;
}

//from 1
void GetStrValue(const char* str, char c,int index, char* value) {
    int n=0;
    const char* head;
    const char* tail;
    head=str;
    tail=NULL;
    value[0]='\0';
    while (*str) {
        if (*str++ != c) {
            continue;
        }
        n++;
        if (n==index-1) {
            head=str;
        } else if (n==index) {
            tail=str-1;
            break;
        } else if (n>index) {
            break;
        }
    }
    if (0==n) {
        tail=str;
    } else if (n < index) {
        if (n < index-1)
            head=str;
        tail=str;
    }
    if (tail) {
        memcpy(value,head,tail-head);
        value[tail-head]='\0';
    }
}


}
}
