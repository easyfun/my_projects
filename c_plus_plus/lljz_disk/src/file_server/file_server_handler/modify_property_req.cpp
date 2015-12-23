#include "file_server_handler.h"

namespace lljz {
namespace disk {

void ModifyPropertyReq(RequestPacket* req,
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

    if (!req_doc.HasMember("type")
        || !req_doc["type"].IsInt()) {
        SetErrorMsg(35001,"type is invalid",resp);
        return;        
    }

    if (!req_doc.HasMember("src_name") 
        || !req_doc["src_name"].IsString() 
        || !req_doc["src_name"].GetStringLength()) {
        SetErrorMsg(35001,"src_name is invalid",resp);
        return;        
    }

    if (!req_doc.HasMember("dest_name") 
        || !req_doc["dest_name"].IsString() 
        || !req_doc["dest_name"].GetStringLength()) {
        SetErrorMsg(35001,"dest_name is invalid",resp);
        return;        
    }

    const char* src_name=req_doc["src_name"].GetString();
    char value[200];
    char file_name[200];
    int file_n=0;
    int folder_num=GetCharCount(src_name, '/')+1;
    int i=0;
    for (i=0;i<folder_num;i++) {
        GetStrValue(src_name, '/', i+1, value);
        if (value[0]=='\0')
            continue;
        sprintf(file_name,value);
        file_n++;
    }
    
    if (0==file_n) {
        SetErrorMsg(35003,"src_name is invalid",resp);
        return;
    }

    RedisClient* file_rc=g_file_redis->GetRedisClient();
    char cmd[512];
    redisReply* reply;
    int cmd_ret;

    switch (req_doc["type"].GetInt()) {
    case 0://目录
        {
            //是否存在根目录，父目录，旧目录
            char father_name[200];
            char lnd_name[200];
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
            for (i=0;i<folder_num;i++) {
                GetStrValue(src_name, '/', i+1, value);
                if (value[0]=='\0')
                    continue;
                counter++;
                if (counter > file_n)
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
                if (counter < file_n) {
                    sprintf(father_name,"%s",reply->str);
                } else {
                    sprintf(lnd_name,"%s",reply->str);
                }

                freeReplyObject(reply);
            }

            //修改文件夹名称 添加-删除
            sprintf(cmd,"HSETNX %s %s %s",father_name,
                req_doc["dest_name"].GetString(), lnd_name);
            cmd_ret=Rhsetnx(file_rc,cmd,reply);
            if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
                g_file_redis->ReleaseRedisClient(file_rc,false);
                SetErrorMsg(35002,"redis database server is busy",resp);
                return;
            } else if (FAILED_ACTIVE==cmd_ret) {
                //记录异常日志
                g_file_redis->ReleaseRedisClient(file_rc,true);
                SetErrorMsg(35006,"modify folder fail,folder has existed",resp);
                return;
            }
            
            sprintf(cmd,"HDEL %s %s",father_name,file_name);
            cmd_ret=Rhdel(file_rc,cmd,reply);
            if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
                g_file_redis->ReleaseRedisClient(file_rc,false);
                SetErrorMsg(35002,"redis database server is busy",resp);
                return;
            } else if (FAILED_ACTIVE==cmd_ret) {
                //记录异常日志
                g_file_redis->ReleaseRedisClient(file_rc,true);
                SetErrorMsg(35006,"modify folder fail,delete old folder fail",resp);
                return;
            }
        }
        break;

    case 1://文件
        {

        }
        break;

    default:
        {
            SetErrorMsg(35001,"type is invalid",resp);
            g_file_redis->ReleaseRedisClient(file_rc,true);
            return;
        }
        break;
    }
    g_file_redis->ReleaseRedisClient(file_rc,true);

    SetErrorMsg(0,"",resp);
}


}
}