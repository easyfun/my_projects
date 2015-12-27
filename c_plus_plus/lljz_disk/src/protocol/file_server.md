文件库

root_dir
    account:
    name:
    parent_dir:
    file_num:
    dir_num:

    file
    sub_dir
        file
        sub_dir


root_dir
    account:(从name解出来，不要)
    name:type|full_path
    parent_dir:full_path(从name解出来，不要)
    file_num:(遍历统计，不要)
    dir_num:(遍历统计，不要)
    record_i:type|full_path

root_dir
    name:type|full_path
    record_num:
    record_i:type|full_path


文件表
account     账号


M:FILE_SERVER_CREATE_FOLDER_REQ
功能说明:新建文件夹
请求消息id:1000
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
        "folder_name":"dir1\dir2\dir3\new_folder"
    }
应答消息id:1001
应答:
    error_code
    {
        "error_msg":""
    }
备注:


M:FILE_SERVER_MODIFY_PROPERTY_REQ
功能说明:修改文件/文件夹属性
请求消息id:1002
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
        "type":0,//0目录，1文件
        "src_name":"/life/happy",
        "dest_name":"nice"
    }
应答消息id:1003
应答:
    error_code
    {
        "error_msg":""
    }
备注:


//file_seq_no_lnd_name
//文件tfs_name不消耗file_id_sets
M:FILE_SERVER_UPLOAD_FILE_REQ
功能说明:上传文件
请求消息id:1004
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
        //"type":1,
        "file_name":"/life/practice_life.md",
        //"tfs_name":"",
        "seq_no":1,//文件分片序号，每片24KB
        "length":100,//max 24KB
        "data":""
    }
应答消息id:1005
应答:
    error_code
    {
        "error_msg":""
    }
备注:


M:FILE_SERVER_DOWNLOAD_FILE_REQ
功能说明:下载文件
请求消息id:1006
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
        "tfs_file_name":"",
    }
应答消息id:1007
应答:
    error_code
    {
        "error_msg":"",
        "tfs_file_name":"",
        "length":100,
        "data":""
    }
备注:

M:FILE_SERVER_DOWNLOAD_FILE_GET_INFO_REQ
功能说明:下载文件
请求消息id:1008
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
        "file_name":""/["file_lnd_name":""]
    }
应答消息id:1009
应答:
    error_code
    {
        "error_msg":"",
        "file_lnd_name":"",
        //"length":100,
        "seq_no_size":100,
    }
备注:

M:FILE_SERVER_DOWNLOAD_FILE_BY_SEQ_NO_REQ
功能说明:下载文件
请求消息id:1010
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
        "file_lnd_name":"",
        "seq_no":0,
    }
应答消息id:1011
应答:
    error_code
    {
        "error_msg":"",
        "file_lnd_name":"",
        "seq_no":0,
        "length":100,
        "data":""
    }
备注:




/*
M:FILE_SERVER_DELETE_FILE_OR_FOLDER_REQ
功能说明:删除文件/文件夹
请求消息id:1010
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
        "type":0,
        "name":""
    }
应答消息id:1011
应答:
    error_code
    {
        "error_msg":""
    }
备注:



M:FILE_SERVER_SEARCH_FILE_REQ
功能说明:查找文件
请求消息id:1012
请求:
    {
        "account":"lljzfly",//账号名
        "password":"trxh",//密码
    }
应答消息id:1013
应答:
    error_code
    {
        "error_msg":""
    }
备注:
*/