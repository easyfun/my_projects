账号表
account     账号
password    密码
img_url     头像
home_page   主页


M:ACCOUNT_SERVER_REGISTER_ACCOUNT_REQ
功能说明:注册账号
请求消息id:1000
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
    }
应答消息id:1001
应答:
    error_code
    {
        "error_msg":""
    }
备注:


M:ACCOUNT_SERVER_SET_ACCOUNT_INFO_REQ
功能说明:设置账号信息
请求消息id:1002
请求:
    {
        "img_url":"",//头像
        "home_page":""//主页
    }
应答消息id:1003
应答:
    error_code
    {
        "error_msg":""
    }
备注:


M:ACCOUNT_SERVER_MODIFY_ACCOUNT_PASSWORD_REQ
功能说明:修改账号密码
请求消息id:1004
请求:
    {
        "old_password":"",//旧密码
        "new_password":""//新密码
    }
应答消息id:1005
应答:
    error_code
    {
        "error_msg":""
    }
备注:


M:ACCOUNT_SERVER_LOGIN_REQ
功能说明:登陆
请求消息id:1006
请求:
    {
        "account":"",//账号
        "password":""//密码
    }
应答消息id:1007
应答:
    error_code
    {
        "error_msg":""
    }
备注:


M:ACCOUNT_SERVER_LOGOUT_REQ
功能说明:退出系统
请求消息id:1008
请求:
    {
        "account":"",//账号
        "password":""//密码
    }
应答消息id:1009
应答:
    error_code
    {
        "error_msg":""
    }
备注:
