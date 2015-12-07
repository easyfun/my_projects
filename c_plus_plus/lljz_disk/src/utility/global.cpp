#include "global.h"

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
    
}
}
