#ifndef LLJZ_DISK_FILE_SERVER_HANDLER_H
#define LLJZ_DISK_FILE_SERVER_HANDLER_H

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "tfs_client_api.h"

#include "public_echo_test_req_handler.h"
#include "handler_router.hpp"
#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "redis_client.h"
#include "redis_client_manager.h"

using namespace rapidjson;
using namespace tfs::client;
using namespace tfs::common;

namespace lljz {
namespace disk {

/**************************************
 *global
**************************************/
extern RedisClientManager* g_account_redis;
extern RedisClientManager* g_file_redis;
extern TfsClient* g_tfs_client;


/**************************************
 * module
**************************************/
void RegisterHandler();
bool InitDatabase();
bool InitHandler();
void WaitHandler();
void StopHandler();


/**************************************
 * 
**************************************/
bool CheckAuth(RequestPacket* req,
    ResponsePacket* resp);

void SetErrorMsg(uint32_t error_code, 
    const char* error_msg, ResponsePacket* resp);

int GetCharCount(const char* str, char c);

void GetStrValue(const char* str, 
    char c,int index, char* value);

//上传文件到TFS服务器
bool WriteFileToTFS(const char* data, int len, 
    char* tfs_file_name);

//从TFS服务器下载文件
bool DownloadFileFromTFS(const char* tfs_file_name,
    char* data,int& len);

/**************************************
 * handler
**************************************/
//新建文件夹
//M:FILE_SERVER_CREATE_FOLDER_REQ=1000
void CreateFolderReq(RequestPacket* req,
    void* args,
    ResponsePacket* resp);

//修改文件/文件夹属性
//FILE_SERVER_MODIFY_PROPERTY_REQ=1002,
void ModifyPropertyReq(RequestPacket* req,
    void* args,
    ResponsePacket* resp);

//上传文件
//FILE_SERVER_UPLOAD_FILE_REQ=1004,
void UploadFileReq(RequestPacket* req,
    void* args,
    ResponsePacket* resp);
/*
//下载文件
//FILE_SERVER_DOWNLOAD_FILE_REQ=1006,
void DownloadFileReq(RequestPacket* req,
    void* args,
    ResponsePacket* resp);

//删除文件/文件夹
//FILE_SERVER_DELETE_FILE_OR_FOLDER_REQ=1008,
void DeleteFileOrFolderReq(RequestPacket* req,
    void* args,
    ResponsePacket* resp);
*/

}
}
#endif