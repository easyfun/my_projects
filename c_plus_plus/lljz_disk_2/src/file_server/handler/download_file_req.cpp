#include "shared.h"

namespace lljz {
namespace disk {

void DownloadFileReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;

    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);
    Value resp_tfs_file_name(kStringType);
    Value resp_length(kNumberType);
    Value resp_data(kStringType);

    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);

    if (!CheckAuth(req,resp))
        return;

    if (!req_doc.HasMember("tfs_file_name") 
        || !req_doc["tfs_file_name"].IsString() 
        || !req_doc["tfs_file_name"].GetStringLength()) {
        SetErrorMsg(35001,"tfs_file_name is invalid",resp);
        return;
    }

    char data[32768];//32KB
    int length=32768;
    if (!DownloadFileFromTFS(req_doc["tfs_file_name"].GetString(),
        data,length)) {
        SetErrorMsg(35010,"download file from tfs failed",resp);
        return;
    }

    resp_error_msg="";
    resp_tfs_file_name.SetString(req_doc["tfs_file_name"].GetString(),
        req_doc["tfs_file_name"].GetStringLength(),resp_allocator);
    resp_length=length;
    resp_data.SetString(data,length,resp_allocator);
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);
    resp_json.AddMember("tfs_file_name",resp_tfs_file_name,resp_allocator);
    resp_json.AddMember("length",resp_length,resp_allocator);
    resp_json.AddMember("data",resp_data,resp_allocator);
    resp_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());
}


}
}