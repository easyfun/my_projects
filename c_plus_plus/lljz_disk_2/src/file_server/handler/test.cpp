#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "func.h"
#include "tfs_client_api.h"
using namespace tfs::client;
using namespace tfs::common;

TfsClient* g_tfs_client=NULL;

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

bool WriteFileToTFS(const char* data, int len, 
char* tfs_file_name) {
    int ret = 0;
    int fd = -1;

    // 创建tfs客户端，并打开一个新的文件准备写入 
    fd = g_tfs_client->open((char*)NULL, NULL, NULL, T_WRITE);
    if (fd <= 0) {
        TBSYS_LOG(ERROR,"create remote file error!");
        return false;
    }
    
    int wrote = 0;
    int left = len;
    while (left > 0) {
        // 将buffer中的数据写入tfs
        ret = g_tfs_client->write(fd, data + wrote, left);
        if (ret < 0 || ret >= left) {// 读写失败或完成
            break;
        } else {// 若ret>0，则ret为实际写入的数据量
            wrote += ret;
            left -= ret;
        }
    }

    // 读写失败   
    if (ret < 0) {
        TBSYS_LOG(ERROR,"write data error!");
        return false;
    }

    // 提交写入 
    ret = g_tfs_client->close(fd, tfs_file_name, TFS_FILE_LEN);
    if (ret != TFS_SUCCESS) {// 提交失败
        TBSYS_LOG(ERROR,"write remote file failed! ret: %s", ret);
        return false;
    } 
    return true;
}

bool DownloadFileFromTFS(const char* tfs_file_name,
char* data,int& len) {
    int ret = 0;
    int fd = -1;

    // 打开待读写的文件
    fd = g_tfs_client->open(tfs_file_name, NULL, T_READ);
    if (ret != TFS_SUCCESS) {
        TBSYS_LOG(ERROR,"open remote file %s error", tfs_file_name);
        return false;
    }

    // 获得文件属性
    TfsFileStat fstat;
    ret = g_tfs_client->fstat(fd, &fstat);
    if (ret != TFS_SUCCESS || fstat.size_ <= 0) {
        TBSYS_LOG(ERROR,"get remote file info error");
        return false;
    }

    if (fstat.size_ > 25165824) {//24KB
        TBSYS_LOG(ERROR,"remote file size more than 24KB,fatal error");
        return false;
    } else if (fstat.size_ > len) {//24KB
        TBSYS_LOG(ERROR,"the buffer is small");
        return false;
    }
    len=fstat.size_;
    
    int read = 0;
    uint32_t crc = 0;
    
    // 读取文件
    while (read < fstat.size_) {
        ret = g_tfs_client->read(fd, data + read, fstat.size_ - read);
        if (ret < 0) {
            break;
        } else {
            crc = Func::crc(crc, data + read, ret); // 对读取的文件计算crc值
            read += ret;
        }
    }

    if (ret < 0 || crc != fstat.crc_) {
        TBSYS_LOG(ERROR,"read remote file error!");
        return false;
    }

    ret = g_tfs_client->close(fd);
    if (ret < 0) {
        TBSYS_LOG(ERROR,"close remote file error!");
        return false;
    }
    return true;
}

int main(int argc,char* argv[]) {

    g_tfs_client=TfsClient::Instance();
    if (TFS_SUCCESS != g_tfs_client->initialize("192.168.241.136:8100")) {
        TBSYS_LOG(ERROR,"---------------%s","TfsClient::initialize fail");
        return 1;
    }

    TBSYS_LOG(ERROR,"---------------%s","TfsClient::initialize ok");
/*
    if (2!=argc) {
        printf("exe path\n");
        return 1;
    }
    const char* folder_name=argv[1];
        int file_n=0;
    int num=GetCharCount(folder_name, '/')+1;
    printf("num=%d\n", num);
    char file_name[200];
    char value[200];
    int i=0;

    for (i=0;i<10;i++) {
        GetStrValue(folder_name, '/', num+i, value);
        printf("i=%d, value=%s\n", num+i, value);
    }

    for (i=0;i<num;i++) {
        GetStrValue(folder_name, '/', i+1, value);
        printf("i=%d, value=%s\n", i,value);
        if (value[0]=='\0')
            continue;
        strcat(file_name,value);
        file_n++;
    }
    printf("file_n=%d\n", file_n);
*/
    return 0;
}
