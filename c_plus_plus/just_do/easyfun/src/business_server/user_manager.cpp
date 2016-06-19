#include "user_manager.h"

UserManager::user_manager(){

}

UserManager::~UserManager(){
    UserInfoMap::iterator it=usersMap.begin();
    while(it==usersMap.end()){
        delete it->second;
        it++;
    }
    usersMap.clear();
}

void UserManager::getUuid(char* uuid){

}

UserInfo* UserManager::getUser(const char* name){
    UserInfo* ui=0;
    tbsys::CThreadGuard guard(&_mutex);
    UserInfoMap::iterator it=usersMap.find(name);
    if (it != usersMap.end()){
        ui=it->second;
    }
    return ui;
}

bool UserManager::insertUser(UserInfo* ui){
    tbsys::CThreadGuard guard(&_mutex);
    UserInfoMap::iterator it=usersMap.find(ui->name);
    if (it!=usersMap.end()){
        UserInfo* dui=it->second;
        usersMap.erase(it);
        delete dui;
    }

    usersMap[ui->name]=ui;
    return true;
}