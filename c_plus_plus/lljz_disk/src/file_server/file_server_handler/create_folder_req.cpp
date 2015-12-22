#include "account_server_handler.h"

namespace lljz {
namespace disk {

int GetCharCount(const char* str, char c) {
    int n=0;
    while (str) {
        if (*str==c)
            n++;
        str++;
    }
    return n;
}

//from 1
void GetStrValue(const char* str, char c,int index, char* value) {
    int n=0;
    const char* head,tail;
    head=str;
    tail=NULL;
    value[0]='\0';
    while (str) {
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
    if (tail) {
        memcpy(value,head,tail-head);
        value[tail-head]='\0';
    }
}

void CreateFolderReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);

    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);

    //检查account
    if (!req_doc.HasMember("account") || !req_doc["account"].IsString()) {
        resp->error_code_=35001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
    if (0==req_doc["account"].GetStringLength()) {
        resp->error_code_=35001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    //检查password
    if (!req_doc.HasMember("password") || !req_doc["password"].IsString()) {
        resp->error_code_=35001;
        resp_error_msg="password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
   
    if (0==req_doc["password"].GetStringLength()) {
        resp->error_code_=35001;
        resp_error_msg="password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    if (!req_doc.HasMember("folder_name") || !req_doc["folder_name"].IsString()) {
        resp->error_code_=35001;
        resp_error_msg="folder_name is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;        
    }
    if (0==req_doc["folder_name"].GetStringLength()) {
        resp->error_code_=35001;
        resp_error_msg="folder_name is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    RedisClient* rc=g_account_redis->GetRedisClient();
    char cmd[512];
    redisReply* reply;
    int cmd_ret;
    //检查认证
    sprintf(cmd,"HGET %s password", req_doc["account"].GetString());
    cmd_ret=Rhget(rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);
        resp->error_code_=35002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有字段password
        //记录异常日志
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);
        resp->error_code_=35002;
        resp_error_msg="redis database server status error";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        freeReplyObject(reply);
        return;
    }

    if (0!=strcmp(req_doc["password"].GetString(),reply->str)) {
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);
        resp->error_code_=35007;
        resp_error_msg="login password is wrong";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        freeReplyObject(reply);
        return;
    }
    freeReplyObject(reply);
    g_account_redis->ReleaseRedisClient(rc,true);

    RedisClient* file_rc=g_file_redis->GetRedisClient();
    const char* folder_name=req_doc["folder_name"].GetString();
    char value[200];
    char file_name[200];
    int file_n=0;
    int num=GetCharCount(folder_name, '/')+1;
    int i=0;
    for (i=0;i<num;i++) {
        GetStrValue(folder_name, '/', i+1, value);
        if (value[0]=='\0')
            continue;
        strcat(file_name,value);
        file_n++;
    }
    if (0==file_n) {
        g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
        resp->error_code_=35003;
        resp_error_msg="path of new folder is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
    //是否存在父目录，新目录
    char father_name[200];
    sprintf(father_name,"%s",req_doc["account"].GetString());
    int counter=0;
    for (i=0;i<num;i++) {
        GetStrValue(folder_name, '/', i+1, value);
        if (value[0]=='\0')
            continue;
        counter++;
        if (counter==file_n-1)
            break;
        sprintf(cmd,"HGET %s %s", father_name,value);
        cmd_ret=Rhget(file_rc,cmd,reply,false);
        if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
            g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
            resp->error_code_=35002;
            resp_error_msg="redis database server is busy";
            resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

            resp_json.Accept(resp_writer);
            sprintf(resp->data_,"%s",resp_buffer.GetString());
            return;
        } else if (FAILED_ACTIVE==cmd_ret) {//没有父目录
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
            resp->error_code_=35003;
            resp_error_msg="path of parent folder does not exist";
            resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

            resp_json.Accept(resp_writer);
            sprintf(resp->data_,"%s",resp_buffer.GetString());
            freeReplyObject(reply);
            return;
        }
        //
        if (0==reply->len) {
            //记录异常日志
            g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
            resp->error_code_=35003;
            resp_error_msg="path of parent folder does not exist";
            resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

            resp_json.Accept(resp_writer);
            sprintf(resp->data_,"%s",resp_buffer.GetString());
            freeReplyObject(reply);
            return;
        }
        freeReplyObject(reply);
        if (i<file_n-1) {
            sprintf(father_name,"%s",reply->str);
        }
    }

    //增加新文件夹，lnd_name不能冲突
    //取得file_id
    sprintf(cmd,"SPOP %s_file_id_sets",
        req_doc["account"].GetString());
    cmd_ret=Rspop(file_rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
        resp->error_code_=35002;
        resp_error_msg="get file_id fail,redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,cmd_ret);
        resp->error_code_=35006;
        resp_error_msg="get file_id fail,file_id is used out";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        freeReplyObject(reply);
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
        resp->error_code_=35002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        resp->error_code_=35006;
        resp_error_msg="create folder fail,folder has existed";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    //为新文件夹增加hash表
    sprintf(cmd,"HSETNX %s create_time %lld", 
        lnd_name, tbsys::CTimeUtil::getTime());
    int cmd_ret=Rhsetnx(file_rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        resp->error_code_=35002;
        resp_error_msg="create folder fail,redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有字段password
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        resp->error_code_=35006;
        resp_error_msg="file_id has used,redis database server status error";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    resp->error_code_=0;
    resp_error_msg="";
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

    resp_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());
}


}
}