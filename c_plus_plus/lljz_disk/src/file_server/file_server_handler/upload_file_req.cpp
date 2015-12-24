#include "account_server_handler.h"

namespace lljz {
namespace disk {

void UploadFileReq(RequestPacket* req,
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

    if (!req_doc.HasMember("file_name") 
        || !req_doc["file_name"].IsString() 
        || !req_doc["file_name"].GetStringLength()) {
        SetErrorMsg(35001,"file_name is invalid",resp);
        return;        
    }

    if (!req_doc.HasMember("seq_no") 
        || !req_doc["seq_no"].IsInt()
        || req_doc["seq_no"]<=0) {
        SetErrorMsg(35001,"seq_no is invalid",resp);
        return;        
    }

    if (!req_doc.HasMember("length") 
        || !req_doc["length"].IsInt() 
        || req_doc["length"].GetInt()<=0
        || req_doc["length"].GetInt()>21504) {
        SetErrorMsg(35001,"length is invalid",resp);
        return;        
    }

    if (!req_doc.HasMember("data") 
        || !req_doc["data"].IsString() 
        || req_doc["data"].GetStringLength() != req_doc["length"].GetInt()) {
        SetErrorMsg(35001,"data is invalid",resp);
        return;        
    }

    //获得文件名，不包含路径
    const char* full_name=req_doc["file_name"].GetString();
    char value[200];
    char file_name[200];
    int file_n=0;
    int folder_num=GetCharCount(full_name, '/')+1;
    int i=0;
    for (i=0;i<folder_num;i++) {
        GetStrValue(full_name, '/', i+1, value);
        if (value[0]=='\0')
            continue;
        sprintf(file_name,value);
        file_n++;
    }
    
    if (0==file_n) {
        SetErrorMsg(35003,"file_name is invalid",resp);
        return;
    }

    //是否存在根目录，父目录
    char father_name[200];
    char lnd_name[200];
    sprintf(father_name,"folder_%s",req_doc["account"].GetString());
    sprintf(cmd,"HGET %s %c_%s", father_name,0x02,"create_time");
    cmd_ret=Rhget(file_rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(35002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        SetErrorMsg(35003,"path of root folder does not exist",resp);
        return;
    }

    int counter=0;
    for (i=0;i<folder_num;i++) {
        GetStrValue(full_name, '/', i+1, value);
        if (value[0]=='\0')
            continue;
        counter++;
        if (counter >= file_n)
            break;
        sprintf(cmd,"HGET %s %s", father_name,value);
        cmd_ret=Rhget(file_rc,cmd,reply,false);
        if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
            g_file_redis->ReleaseRedisClient(file_rc,false);
            SetErrorMsg(35002,"redis database server is busy",resp);
            return;
        } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,true);
            freeReplyObject(reply);
            SetErrorMsg(35003,"path of parent folder does not exist",resp);
            return;
        }
        //
        if (0==reply->len) {
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,true);
            freeReplyObject(reply);
            SetErrorMsg(35003,"path of parent folder does not exist",resp);
            return;
        }
        sprintf(father_name,"%s",reply->str);
        freeReplyObject(reply);
    }

    if (counter != file_n-1) {
        g_file_redis->ReleaseRedisClient(file_rc,true);
        SetErrorMsg(35003,"path of parent folder does not exist",resp);
        return;
    }

    //文件不存在
    //文件存在,是目录
    //文件存在，属于一个分片
    //文件存在，不属于一个分片
    sprintf(cmd, "HGET %s %s", father_name, file_name);
    cmd_ret=Rhget(file_rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(35002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//文件不存在
/*
        g_file_redis->ReleaseRedisClient(file_rc,true);
        freeReplyObject(reply);
        SetErrorMsg(35003,"path of parent folder does not exist",resp);
*/
        freeReplyObject(reply);
        char tfs_file_name[TFS_FILE_LEN];
        if (!WriteFileToTFS(req_doc["data"].GetString(),
            req_doc["data"].GetStringLength(),
            tfs_file_name)) {
            
            SetErrorMsg(35009,"write file failed");
            g_file_redis->ReleaseRedisClient(file_rc,true);
            return;
        }

        printf(lnd_name,"%d%c%s%c%s",1,0x01,
            req_doc["account"].GetString(),
            0x01,tfs_file_name);
        sprintf(cmd,"HSETNX %s %s %s",father_name,
            file_name, lnd_name);
        cmd_ret=Rhsetnx(file_rc,cmd,reply);
        if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
            g_file_redis->ReleaseRedisClient(file_rc,false);
            SetErrorMsg(35002,"redis database server is busy",resp);
            return;
        } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,true);
            freeReplyObject(reply);
            SetErrorMsg(35009,"file has existed",resp);
            return;
        }
        freeReplyObject(reply);

        sprintf(cmd,"ZADD file_seq_no_zsets_%s %s %s", lnd_name,
            req_doc["seq_no"].GetInt(), tfs_file_name)
        cmd_ret=Rzadd(file_rc,cmd,reply);
        if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,false);
            SetErrorMsg(35002,"redis database server is busy",resp);
            return;
        } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,true);
            freeReplyObject(reply);
            SetErrorMsg(35009,"create file failed",resp);
            return;
        }
        freeReplyObject(reply);

        SetErrorMsg(0,"",resp);
        g_file_redis->ReleaseRedisClient(file_rc,true);
        return;
    }

    if (0==reply->len) {
        g_file_redis->ReleaseRedisClient(file_rc,true);
        freeReplyObject(reply);
        SetErrorMsg(35008,"redis database data status error",resp);
        return;
    }
    //文件存在,是目录
    if ('0'==reply->str[0]) {
        g_file_redis->ReleaseRedisClient(file_rc,true);
        freeReplyObject(reply);
        SetErrorMsg(35009,"folder which name is same has existed",resp);
        return;
    } else if ('1' != reply->str[0]) {
        //数据库数据异常
        g_file_redis->ReleaseRedisClient(file_rc,true);
        freeReplyObject(reply);
        SetErrorMsg(35008,"redis database data status error",resp);
        return;
    }
    //检查分片是否已存在
    //file_lnd_name:10x01account0x01tfs_name
    sprintf(lnd_name, "file_seq_no_zsets_%s", reply->str);
    freeReplyObject(reply);
    sprintf(cmd, "ZRANGEBYSCORE %s %d %d", lnd_name,
        req_doc["seq_no"].GetInt(), 
        req_doc["seq_no"].GetInt());
    cmd_ret=Rzrangebyscore(file_rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(35002,"redis database server is busy",resp);
        return;
    } else if (SUCCESS_ACTIVE==cmd_ret) {//文件存在，分片已存在，不属于一个分片
        g_file_redis->ReleaseRedisClient(file_rc,true);
        freeReplyObject(reply);
        SetErrorMsg(35009,"file has existed",resp);
        return;
    }
    //文件存在，属于一个分片
    char tfs_file_name[TFS_FILE_LEN];
    if (!WriteFileToTFS(req_doc["data"].GetString(),
        req_doc["data"].GetStringLength(),
        tfs_file_name)) {
        
        SetErrorMsg(35009,"write file failed");
        g_file_redis->ReleaseRedisClient(file_rc,true);
        return;
    }

    sprintf(cmd,"ZADD %s %d %s",lnd_name,
        req_doc["seq_no"].GetInt(),tfs_file_name);
    cmd_ret=Rzadd(file_rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(35002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        freeReplyObject(reply);
        SetErrorMsg(35009,"create file failed",resp);
        return;
    }
    SetErrorMsg(0,"",resp);
    g_file_redis->ReleaseRedisClient(file_rc,true);
}


}
}