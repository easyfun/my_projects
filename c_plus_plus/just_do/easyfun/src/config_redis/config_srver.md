####配置服务

#####redis key
服务名字列表

key:server_name_list

type:set

value:"access","business"

服务id列表

key:server_id_list:{server_name}

type:set

value:server_id1,server_id2

服务信息

key:server_info:{server_id}

type:hash

fileds:
    type
    server_id
    name
    host
    front-port
    back-port

####从配置服务获取服务列表的流程
1.连接配置服务redis

2.获取服务列表

3.与依赖服务建立连接

4.剔除下线的服务

####配置服务信息管理
监控工具

架构：B/S

功能：

1.增加服务

2.修改服务信息

3.删除服务


