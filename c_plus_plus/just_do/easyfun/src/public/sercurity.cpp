#include <stdio.h>
#include <string.h>

#include <openssl/evp.h>  
#include <openssl/bio.h>  
#include <openssl/buffer.h> 

#include <openssl/md5.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "sercurity.h"

namespace lljz {
namespace disk {

char * base64_encode(const char * input, 
int length, 
bool with_new_line=false)  
{  
    BIO * bmem = NULL;  
    BIO * b64 = NULL;  
    BUF_MEM * bptr = NULL;  
  
    b64 = BIO_new(BIO_f_base64());  
    if(!with_new_line) {  
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);  
    }  
    bmem = BIO_new(BIO_s_mem());  
    b64 = BIO_push(b64, bmem);  
    BIO_write(b64, input, length);  
    BIO_flush(b64);  
    BIO_get_mem_ptr(b64, &bptr);  
  
    char * buff = (char *)malloc(bptr->length + 1);  
    memcpy(buff, bptr->data, bptr->length);  
    buff[bptr->length] = 0;  
  
    BIO_free_all(b64);  
  
    return buff;  
}  
  
char * base64_decode(int& decode_length, 
const char * input, 
int length, 
bool with_new_line=false)  
{  
    BIO * b64 = NULL;  
    BIO * bmem = NULL;  
    char * buffer = (char *)malloc(length);  
    memset(buffer, 0, length);  
  
    b64 = BIO_new(BIO_f_base64());  
    if(!with_new_line) {  
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);  
    }  
    bmem = BIO_new_mem_buf(input, length);  
    bmem = BIO_push(b64, bmem);  
    decode_length=BIO_read(bmem, buffer, length);  
  
    BIO_free_all(bmem);  
  
    return buffer;  
} 

}
}

/*
using namespace lljz::disk;
int main(int argc, char *argv[])
{
    char src[10]="ac";
    src[2]=0;
    src[3]='b';
/*
    printf("src:%s\n", src);
    int src_len=4;//strlen(src);
    char *base64_encode_src=base64_encode(src,src_len);
    int base64_encode_src_len=strlen(base64_encode_src);
    printf("base64 encode: %s\n", base64_encode_src);
    int base64_decode_src_len;
    char* base64_decode_src=base64_decode(base64_decode_src_len,
        base64_encode_src, base64_encode_src_len);
    printf("base64_decode:%s,len=%d\n", 
        base64_decode_src,
        base64_decode_src_len);
    printf("%s\n", src);
* /
    char *p_de;
    RSA *p_rsa;
    FILE *file;
    int rsa_len;
    if((file=fopen("pbu.key","r"))==NULL){
        perror("open key file error");
        return NULL;
    }
    if((p_rsa=PEM_read_RSAPrivateKey(file,NULL,NULL,NULL))==NULL){
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    rsa_len=RSA_size(p_rsa);
    printf("rsa_len=%d\n", rsa_len);
    p_de=(unsigned char *)malloc(rsa_len+1);
    memset(p_de,0,rsa_len+1);
    if(RSA_private_decrypt(rsa_len,(unsigned char *)str,(unsigned char*)p_de,p_rsa,RSA_PKCS1_PADDING)<0){
        return NULL;
    }
    RSA_free(p_rsa);
    fclose(file);
    return 0;
}*/
