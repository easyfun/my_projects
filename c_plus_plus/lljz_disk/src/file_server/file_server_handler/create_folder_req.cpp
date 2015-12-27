#include "file_server_handler.h"

namespace lljz {
namespace disk {

/*
 * lnd_name:lljz_net_disk_name
*/
void CreateFolderReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;
/*
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);
*/

    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);
    
    if (!CheckAuth(req,resp))
        return;

    if (!req_doc.HasMember("folder_name") 
        || !req_doc["folder_name"].IsString() 
        || !req_doc["folder_name"].GetStringLength()) {
        SetErrorMsg(35001,"folder_name is invalid",resp);
        return;        
    }

    const char* folder_name=req_doc["folder_name"].GetString();
    char value[200];
    char file_name[200];
    int file_n=0;
    int num=GetCharCount(folder_name, '/')+1;
    int i=0;
    //  /a/b//c//d
    for (i=0;i<num;i++) {
        GetStrValue(folder_name, '/', i+1, value);
        if (value[0]=='\0')
            continue;
        sprintf(file_name,"%s",value);
        file_n++;
    }
    if (0==file_n) {
        SetErrorMsg(35003,"path of new folder is invalid",resp);
        return;
    }

    RedisClient* file_rc=g_file_redis->GetRedisClient();
    char cmd[512];
    redisReply* reply;
    int cmd_ret;
    //是否存在根目录，父目录，新目录
    char father_name[200];
    sprintf(father_name,"folder_%s",req_doc["account"].GetString());
    sprintf(cmd,"HGET %s %c_%s", father_name,0x02,"create_time");
    cmd_ret=Rhget(file_rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
        SetErrorMsg(35002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
        SetErrorMsg(35003,"path of root folder does not exist",resp);
        return;
    }

    int counter=0;
    for (i=0;i<num;i++) {
        GetStrValue(folder_name, '/', i+1, value);
        if (value[0]=='\0')
            continue;
        counter++;
        if (counter>=file_n)
            break;
        sprintf(cmd,"HGET %s %s", father_name,value);
        cmd_ret=Rhget(file_rc,cmd,reply,false);
        if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
            g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
            SetErrorMsg(35002,"redis database server is busy",resp);
            return;
        } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
            freeReplyObject(reply);
            SetErrorMsg(35003,"path of parent folder does not exist",resp);
            return;
        }
        //
        if (0==reply->len) {
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
            freeReplyObject(reply);
            SetErrorMsg(35003,"path of parent folder does not exist",resp);
            return;
        }
        sprintf(father_name,"%s",reply->str);
        freeReplyObject(reply);
    }

    //增加新文件夹，lnd_name不能冲突
    //取得file_id
    sprintf(cmd,"SPOP file_id_sets_%s",
        req_doc["account"].GetString());
    cmd_ret=Rspop(file_rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
        SetErrorMsg(35002,"get file_id fail,redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
        freeReplyObject(reply);
        SetErrorMsg(35006,"get file_id fail,file_id is used out",resp);
        return;
    }

    //在父目录增加记录
    char lnd_name[512];
    sprintf(lnd_name,"%d%c%s%c%s", 0,0x01,
        req_doc["account"].GetString(),0x01,reply->str);
    freeReplyObject(reply);
    sprintf(cmd,"HSETNX %s %s %s",father_name,file_name, lnd_name);
    cmd_ret=Rhsetnx(file_rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(35002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        SetErrorMsg(35006,"create folder fail,folder has existed",resp);
        return;
    }

    //为新文件夹增加hash表
    sprintf(cmd,"HSETNX %s %c_create_time %lld",
        lnd_name,0x02, tbsys::CTimeUtil::getTime());
    cmd_ret=Rhsetnx(file_rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(35002,"create folder fail,redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        SetErrorMsg(35006,"file_id has used,redis database server status error",resp);
        return;
    }
    g_file_redis->ReleaseRedisClient(file_rc,true);

    SetErrorMsg(0,"",resp);
}


}
}