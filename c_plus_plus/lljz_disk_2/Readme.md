#####云盘服务系统
    存储文件，图片的服务系统
    
    主流云盘系统：360云盘，腾讯云盘，百度云盘，华为云文件系统，七牛云存储

部署图:<br/>
![部署图](./deployment.jpg)

部署环境：<br/>
* 只支持Linux系统

网络框架：<br/>
* 使用的tbsdk（tbnet，tbsys）
* tbsdk是一个Linux下的C++服务端开发框架，结构简单清晰，性能极高，应用层不用处理粘包问题，支持自定义编解码，事件回调极其灵活，可扩展性极强
* [tbsdk源码地址](http://code.taobao.org/p/tb-common-utils/src/trunk/tbnet/)
* [tbsdk框架解析](http://blog.chinaunix.net/uid-20196318-id-3142050.html)

数据库：
* redis

文件系统：
* 淘宝TFS文件系统
* [淘宝TFS首页](http://tfs.taobao.org/)

Json解析：<br/>
  RapidJson（腾讯开源）
  

#####源码结构
lljz_disk_2<br/>
----src<br/>
--------access_server   （接入服务器）<br/>
--------account_server  （账号服务器）<br/>
--------file_server     （文件服务器） <br/>
--------client          （C++版测试客户端，可以进行压力测试，只能在linux下使用） <br/>
--------py_client       （python版客户端，用于功能测试，网络使用boost-python和tbsdk做的扩展，只能在linux下使用）<br/>
--------packets         （公共请求、应用类型定义，协议接口定义）<br/>
--------public          （公共类型定义）<br/>
--------public_handler  （公共消息处理）<br/>
--------redis           （基于hiredis，封装业务使用的redis命令）<br/>

#####缺点
* 大文件分片存储，大文件不利于分析

#####TODO_LIST
    1.安全认证
    2.对文件取指纹，增加引用计数，秒传
    3.Windows版测试客户端
    4.文件元信息按照hash类型存储在redis，数据结构分散，需要进一步设计
    5.Makfile自动依赖，目前逐个文件编写
    6.流量控制
    7.tcp连接监控，便于服务器间网络调试


