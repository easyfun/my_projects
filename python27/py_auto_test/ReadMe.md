	服务端自动测试工具
	服务端通讯使用C++开发的客户端模块
	测试业务使用Python
	利用boost.Python实现C++客户端对Python的扩展
	服务端部分通讯层基于TCP定制的，编码部分比较特殊，完全用Python开发相应客户端有些困难，直接采用C++开发的客户端提供给Python调用。利用boost.Python定义好少量几个接口提供给Python即可。
