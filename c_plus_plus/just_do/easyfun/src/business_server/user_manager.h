#ifndef USER_MANAGER_H
#define USER_MANAGER_H

namespace lljz {
namespace disk {

#include <ext/hash_map>
typedef __gnu_cxx::hash_map<const char*,
    UserInfo*> UserInfoMap;

enum UserStatus {
    None=0,
    AppLogin=1,
    UserLogin=2
};

class UserInfo{
public:
    UserInfo() {
        memset(name,0,sizeof(name));
        //memset(accessSpec,0,sizeof(accessSpec));
        accessServId=0;
        status=None;
        lastUsedTime=0;
        memset(key,0,sizeof(key));
        bfKey=0;
    }
    char name[40];
    //char accessSpec[64];
    uint64_t accessServId;
    UserStatus status;
    uint32_t lastUsedTime;
    char key[40];
    uint32_t method;//encrypt
    BF_KEY* bfKey;
};

class UserManager {
public:
    UserManager();
    ~UserManager();

    void getUuid(char* uuid);
    UserInfo* getUserInfo(const char* name);
    bool insertUserInfo(UserInfo* ui);

private:
    UserInfoMap usersMap;
    tbsys::CThreadMutex mutex;
};

}
}

#endif