#include "account_server_handler.h"

namespace lljz {
namespace disk {

void RegisterAccountReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);
    std::string resp_data;

    req_doc.Parse(req->data_);
    std::string req_account;
    std::string req_password;
    if (!req_doc.HasMember("account")) {
        resp->error_code_=20001;
        resp_error_msg="account invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    if (!req_doc["account"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="account invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }
    req_account=req_doc["account"].GetString();

    if (!req_doc.HasMember("password")) {
        resp->error_code_=20001;
        resp_error_msg="password invalid"
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }
    if (!req_doc["password"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="password invalid"
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }
    req_password=req_doc["password"].GetString();

    if (req_account.empty() || req_password.empty()) {
        resp->error_code_=20001;
        resp_error_msg="account,password can not be empty";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        resp_data=resp_buffer.GetString();
        sprintf(resp->data_,"%s",resp_data.c_str());
        return;
    }

    

    resp_json.Accept(resp_writer);
    resp_data=resp_buffer.GetString();
    sprintf(resp->data_,"%s",resp_data.c_str());
}

}
}
