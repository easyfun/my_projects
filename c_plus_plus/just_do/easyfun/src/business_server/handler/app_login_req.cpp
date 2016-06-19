#include "handler.h"

namespace lljz {
namespace disk {

void appLoginReq(RequestPacket* req,
void* args,
ResponsePacket* resp) {
    int req_data_len;
    char* req_data=base64_decode(req_data_len,
        req->data_, strlen(req->data_));
    if (0==req_data) {
        SetErrorMsg(-1,"base64 decode failed",resp);
        return;
    }

    if (0==g_serv_pri_rsa){
        SetErrorMsg(-1,"server get private key failed",resp);
        free(req_data);
        return;
    }
    char* decrypt_req_data=new char[g_serv_pri_rsa_len+1];
    if (0==decrypt_req_data) {
        free(req_data);
        SetErrorMsg(-1,"Server used memmory",resp);
        return;
    }

    if (req_data_len > g_serv_pri_rsa_len-11) {
        free(req_data);
        SetErrorMsg(-1,"request packet length error", resp);
        return;
    }
    int decrypt_req_data_len=RSA_private_decrypt(req_data_len,
        (unsigned char *)req_data,
        (unsigned char*)decrypt_req_data,
        g_serv_pri_rsa,
        RSA_PKCS1_PADDING);
    if(decrypt_req_data_len<0){
        free(req_data);
        SetErrorMsg(-1,"decrypt request packet failed",resp);
        return;
    }
    free(req_data);

    Document req_doc;

    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_value_json(kObjectType);
    Value resp_value_rand_num(kStringType);

    req_doc.Parse(decrypt_req_data);
    if (!req_doc.HasMember("rand") 
        || !req_doc["rand"].IsUint()) {
        free(decrypt_req_data);
        SetErrorMsg(-1,"request invalid",resp);
        return;
    }

    if (!req_doc.HasMember("data") 
        || !req_doc["data"].IsString()
        || 0==req_doc["data"].GetStringLength()) {
        free(decrypt_req_data);
        SetErrorMsg(-1,"request invalid",resp);
        return;
    }

    int json_data_len;
    char* json_data=base64_decode(
        json_data_len,
        req_doc["data"].GetString(),
        req_doc["data"].GetStringLength());
    if (0==json_data){
        free(decrypt_req_data);
        SetErrorMsg(-1,"request invalid",resp);
        return;
    }

    uint32_t rand_num=req_doc["rand"].GetUint();
    char rand_num_bits[4];
    rand_num_bits[0]=rand_num & 0x000000FF;
    rand_num_bits[1]=(rand_num & 0x0000FF00)>>8;
    rand_num_bits[2]=(rand_num & 0x00FF0000)>>16;
    rand_num_bits[3]=(rand_num & 0xFF000000)>>24;
    for (int i=0;i<json_data_len;i++)
    {
        json_data[i]^=rand_num_bits[i%4];
    }

    Document json_data_doc;
    json_data_doc.Parse(json_data);
    if (!json_data_doc.HasMember("type") 
        || !json_data_doc["type"].IsString()
        || 0==json_data_doc["type"].GetStringLength()) {
        free(decrypt_req_data);
        free(json_data);
        SetErrorMsg(-1,"request invalid",resp);
        return;
    }

    if (!json_data_doc.HasMember("version") 
        || !json_data_doc["version"].IsString()
        || 0==json_data_doc["version"].GetStringLength()) {
        free(decrypt_req_data);
        free(json_data);
        SetErrorMsg(-1,"request invalid",resp);
        return;
    }

    if (!json_data_doc.HasMember("key") 
        || !json_data_doc["key"].IsString()
        || 0==json_data_doc["key"].GetStringLength()) {
        free(decrypt_req_data);
        free(json_data);
        SetErrorMsg(-1,"request invalid",resp);
        return;
    }

    if (!json_data_doc.HasMember("method")
        || !json_data_doc["method"].IsUint()){
        free(decrypt_req_data);
        free(json_data);
        SetErrorMsg(-1,"request invalid",resp);
        return;
    }
    uint32_t method=json_data_doc["method"].GetUint();

    resp_value_rand_num.SetUint(rand_num);
    resp_value_json.AddMember("rand_num",
        resp_value_rand_num,
        resp_allocator);
    resp_value_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());

    char resp_data_blowfish[512]={0};
    int resp_data_blowfish_len=strlen(resp->data_);
    UserInfo* user_info=new UserInfo();
    g_userManager->getUuid(user_info->name);
    user_info->accessServId=resp->dest_id_;
    user_info->status=AppLogin;
    strcat(user_info->key,json_data_doc["key"].GetString();
    if (1==method) {
        user_info->method=1;
        user_info->bfKey=new BF_KEY();

        BF_set_key(key,
            json_data_doc["key"].GetStringLength(),
            json_data_doc["key"].GetString());
        char iv[8];
        int n=0;
        memcpy(iv,g_blowfish_cbc_iv,8);
        BF_cfb64_encrypt((unsigned char *)resp->data_,
            resp_data_blowfish,
            resp_data_blowfish_len,&key,
            iv,&n,BF_ENCRYPT);
    } else {
        free(decrypt_req_data);
        free(json_data);
        delete user_info;
        user_info=0;
        SetErrorMsg(-1,"request invalid",resp);
        return;
    }
    user_info->lastUsedTime=tbsys::CTimeUtil::getTime();
    g_userManager->insertUser(user_info);

    char* resp_data_base64_encode=base64_encode(
            resp_data_blowfish,
            resp_data_blowfish_len);
    int resp_data_base64_encode_len=
        strlen(resp_data_base64_encode);
    memcpy(resp->data_,resp_data_base64_encode,
        resp_data_base64_encode_len);
    resp->data_[resp_data_base64_encode_len]=0;
    free(resp_data_base64_encode);
    free(decrypt_req_data);
    free(json_data);

    Document resp_access_doc;
    Document::AllocatorType& resp_access_allocator=
        resp_access_doc.GetAllocator();
    StringBuffer resp_access_buffer;
    Writer<StringBuffer> resp_access_writer(resp_access_buffer);
    Value resp_access_value_json(kObjectType);
    Value resp_access_value_name(kStringType);
    Value resp_access_value_data(kStringType);

    resp_access_value_name.SetString(user_info->name,
        strlen(user_info->name),
        resp_access_allocator);
    resp_access_value_data.SetString(resp->data_,
        strlen(resp->data_),
        resp_access_allocator);

    resp_access_value_json.AddMember("name",
        resp_access_value_name,
        resp_allocator);
    resp_access_value_json.AddMember("data",
        resp_access_value_data,
        resp_access_allocator);
    resp_access_value_json.Accept(resp_access_writer);
    sprintf(resp->data_,"%s",resp_access_buffer.GetString());
}

}
}