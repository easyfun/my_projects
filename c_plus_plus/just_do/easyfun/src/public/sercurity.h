#ifndef SECURITY_H
#define SECURITY_H

namespace lljz {
namespace disk {

char * base64_encode(const char * input, 
int length, 
bool with_new_line=false);


char * base64_decode(int& decode_length, 
const char * input, 
int length, 
bool with_new_line=false);

}
}

#endif