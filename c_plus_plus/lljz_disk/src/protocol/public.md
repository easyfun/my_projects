tbnet自带echo性能测试
测试环境：
硬件配置：
    处理器:Intel(R) Pentium(R) CPU G3250 @ 3.20GHz 3.20GHz
    内存：8.00GB
    系统类型：64位 Window7 旗舰版
操作系统：
    VMware CentOS 64位
    虚拟内存: 2.5GB
    虚拟CPU：2 core
服务端，客户端都部署在同一台PC上

./echoserver tcp:127.0.0.1:8000
./echoclient tcp:127.0.0.1:8000 request_total conn_total
3次平均值
1条长连接，单个数据包512
1000笔 1900笔/s
5000笔 9800笔/s
10000笔 19700笔/s
50000笔 99000笔/s
80000笔 158200笔/s
90000笔 178000笔/s
100000笔 198000笔/s 90%  99000笔/s 10%
110000笔 109000笔/s
120000笔 119000笔/s
130000笔 129000笔/s


1条长连接，单个数据包512
account_server的echo性能测试
5000笔 35000笔/s
10000笔 45000笔/s
50000笔 50000笔/s


tbnet的packet结构
    包类型
    通道id，标识客户端发出的请求
    pcode,消息号
    包体长度
    包体

应用层packet结构，对tbnet的packet的二次规划
packet分为两大类：
    请求包，pcode=0
    应答包，pcode=1

tbnet的packet的包体再次划分包头，包体
请求包
    uint16_t src_type_; //源类型
    uint64_t src_id_;   //源id
    uint16_t dest_type_;//目标类型
    uint64_t dest_id_;  //目标id
    uint32_t msg_id_;   //消息id
    //请求id，继承自Packet::_packetHeader._chid
    uint32_t version_;  //消息版本号
    char data_[REQUEST_PACKET_MAX_SIZE]; //

应打包
    uint16_t src_type_; //源类型
    uint64_t src_id_;   //源id
    uint16_t dest_type_;//目标类型
    uint64_t dest_id_;  //目标id
    uint32_t msg_id_;   //消息id
    //请求id，继承自Packet::_packetHeader._chid
    //uint32_t version_;  //消息版本号
    uint32_t error_code_; //错误码
    char data_[RESPONSE_PACKET_MAX_SIZE]; //


功能模块:public
M:PUBLIC_REGISTER_REQ
功能说明:注册服务
请求消息id:2
请求:
    srv_id_
应答消息id:3
应答:
    error_code_=0
备注:


错误号
0 成功
//public 1~10000
1 服务已注册
2 服务未注册
3 server_type not supported
4 msg_id not supported

//config_server 10001~15000
10001 请求参数错误srv_id
10002 请求参数错误spec

//access_server 15001~20000


//account_server 20001~35000
20001 请求参数错误
20002 redis database不可用
20003 账号不存在
20004 操作redis database失败



py_client模块
Send函数错误号
1 连接无效
2 连接繁忙
3 发送请求失败
4 连接失败
5 发送超时
