账号表
account     账号
password    密码
img_url     头像
home_page   主页
timestamp    时间戳


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

M:ACCOUNT_SERVER_CHECK_SYSTEM_STATUS_REQ
request:
    {
        "rand":8,//1bytes unsigned long
        "method":1,//1-blowfish
        "data":"rand fix, base64",
        //{
        //    "type":"",//anroid_app,iOS_app,winPhone_app,
        //            //desktop_win_app,desktop_ubuntu_app,
        //            //web_app
        //    "version":"beta_1.0",
        //    "key":""
        //}
    }//rsa encrypt,base64
response:
    error_code
    {
        "error_msg":""
    }