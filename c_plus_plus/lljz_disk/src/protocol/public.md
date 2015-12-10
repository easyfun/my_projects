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
1 连接无效
2 连接繁忙
3 发送请求失败
4 连接失败
5 发送超时
-1 服务已注册
-2 服务未注册
