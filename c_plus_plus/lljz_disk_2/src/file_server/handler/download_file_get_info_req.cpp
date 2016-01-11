#include "shared.h"

namespace lljz {
namespace disk {

void DownloadFileGetInfoReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;

    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);
    Value resp_file_lnd_name(kStringType);
    Value resp_size(kNumberType);

    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);

    if (!CheckAuth(req,resp))
        return;

    char file_lnd_name[512];
    bool ret=false;
    if (!req_doc.HasMember("file_name")) {
        //利用lnd_name取

        if (!req_doc.HasMember("file_lnd_name") 
            || !req_doc["file_lnd_name"].IsString() 
            || !req_doc["file_lnd_name"].GetStringLength()) {
            SetErrorMsg(35001,"file_lnd_name is invalid",resp);
            return;
        }
        sprintf(file_lnd_name,"%s",
            req_doc["file_lnd_name"].GetString());
    } else {//利用file_name取,取file_lnd_name
        if (!req_doc["file_name"].IsString() 
            || !req_doc["file_name"].GetStringLength()) {
            SetErrorMsg(35001,"file_name is invalid",resp);
            return;
        }
        ret=true;
    }

    RedisClient* file_rc=g_file_redis->GetRedisClient();
    char cmd[200];
    int cmd_ret;
    redisReply* reply;
    if (ret) {
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
            sprintf(file_name,"%s",value);
            file_n++;
        }
    
        if (0==file_n) {
            g_file_redis->ReleaseRedisClient(file_rc,true);
            SetErrorMsg(35003,"file_name is invalid",resp);
            return;
        }

        //是否存在根目录，父目录
        char father_name[200];
        char lnd_name[200];
        sprintf(father_name,"folder_%s",req_doc["account"].GetString());
        sprintf(cmd,"HGET %s %c_%s", father_name,0x02,"create_time");

        RedisClient* file_rc=g_file_redis->GetRedisClient();
        cmd_ret=Rhget(file_rc,cmd,reply);
        if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
            g_file_redis->ReleaseRedisClient(file_rc,false);
            SetErrorMsg(35002,"redis database server is busy",resp);
            return;
        } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,true);
            SetErrorMsg(35003,"...cmd,path of root folder does not exist",resp);
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
                SetErrorMsg(35003,"++++,path of parent folder does not exist",resp);
                return;
            }
            //
            if (0==reply->len) {
                //记录异常日志
                g_file_redis->ReleaseRedisClient(file_rc,true);
                freeReplyObject(reply);
                SetErrorMsg(35003,"path of parent folder does not exist,data error status",resp);
                return;
            }
            sprintf(father_name,"%s",reply->str);
            freeReplyObject(reply);
        }   

        if (counter != file_n) {
            g_file_redis->ReleaseRedisClient(file_rc,true);
            SetErrorMsg(35003,"path of parent folder does not exist...",resp);
            return;
        }

        sprintf(cmd, "HGET %s %s", father_name, file_name);
        cmd_ret=Rhget(file_rc,cmd,reply,false);
        if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
            g_file_redis->ReleaseRedisClient(file_rc,false);
            SetErrorMsg(35002,"redis database server is busy",resp);
            return;
        } else if (FAILED_ACTIVE==cmd_ret) {//文件不存在
            freeReplyObject(reply);
            SetErrorMsg(35009,"file does not existed",resp);
            g_file_redis->ReleaseRedisClient(file_rc,true);

        }

        sprintf(file_lnd_name, "file_seq_no_zsets_%s", reply->str);
        freeReplyObject(reply);
    }

    sprintf(cmd,"ZCARD %s", file_lnd_name);
    cmd_ret=Rzcard(file_rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(35011,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        SetErrorMsg(35011,"no seq_no_zsets",resp);
        return;
    }
    resp_size.SetInt(reply->integer);
    freeReplyObject(reply);
    g_file_redis->ReleaseRedisClient(file_rc,true);

    resp_error_msg="";
    resp_file_lnd_name.SetString(file_lnd_name,
        strlen(file_lnd_name),resp_allocator);
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);
    resp_json.AddMember("file_lnd_name",
        resp_file_lnd_name,resp_allocator);
    resp_json.AddMember("size",resp_size,resp_allocator);
    resp_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());
}


}
}