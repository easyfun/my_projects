#!/usr/bin/env python
#-*- coding:utf-8 -*-

import os
import jinja2
import shutil

src_path = 'F:\\project\\python_tools\\create_java_backend\\template\\carOrderStats_one_datasource'
src_api_name = 'carOrderStatsApi'
src_api_path = os.path.join(src_path, src_api_name)
src_impl_name = 'carOrderStatsImpl'
src_impl_path = os.path.join(src_path, src_impl_name)
src_package_name = 'com.wanshun.carorderstats'

# 修改
root_project_name = 'carOrder'
project_name = 'carOrderConsole'
project_file_name = 'CarOrderConsole'
project_application_name = 'CAR_ORDER_CONSOLE_SERVER_NAME'
project_impl_main_class = '%sApplication' % project_file_name
project_data_generate = '%sDataGenerate' % project_file_name
project_motan_rpc_port = '10510'
project_http_port = '10500'
project_https_port = '10501'
parent_project_name = 'orderSystem'

dest_path = 'G:\\python_data'
project_path = os.path.join(dest_path, root_project_name)
project_api_name = '%sApi' % project_name
project_api_path = os.path.join(project_path, project_api_name)

project_impl_name = '%sImpl' % project_name
project_impl_path = os.path.join(project_path, project_impl_name)
project_package_name = 'com.wanshun.%s' % project_name.lower()
project_impl_full_main_class = '%s.springboot.%s' % (project_package_name, project_impl_main_class)
project_mybatis_resources = '/'.join(str(s) for s in project_package_name.split('.'))


project_module_constant = '%sModuleConstant' % project_file_name
project_rpc_client_config = 'Rpc%sClientConfig' % project_file_name
project_rpc_server_config = 'Rpc%sServerConfig' % project_file_name

rename_file = {
    'CarOrderStatsModuleConstant.java' : '%s.java' % project_module_constant,
    'RpcCarOrderStatsClientConfig.java' : '%s.java' % project_rpc_client_config,

    'CarOrderStatsApplication.java' : '%s.java' % project_impl_main_class,
    'RpcCarOrderStatsServerConfig.java' : '%s.java' % project_rpc_server_config,

    'DriverMirrorDataGenerate.java' : '%s.java' % project_data_generate
}

template_key_words = {
    'parent_project_name' : parent_project_name,
    'project_name' : project_name,
    'project_api_name' : project_api_name,
    'project_impl_name' : project_impl_name,

    'parent_project_api_name' : root_project_name,
    'parent_project_impl_name' : root_project_name,

    'project_package_name' : project_package_name,
    'project_module_constant' : project_module_constant,
    'project_rpc_client_config' : project_rpc_client_config,
    'project_rpc_server_config' : project_rpc_server_config,

    'project_application_name' : project_application_name,
    'project_impl_main_class' : project_impl_main_class,
    'project_impl_full_main_class' : project_impl_full_main_class,

    'app_id' : project_name,
    'project_data_generate' : project_data_generate,

    'project_motan_rpc_port' : project_motan_rpc_port,
    'project_http_port' : project_http_port,
    'project_https_port' : project_https_port,
    'project_mybatis_resources' : project_mybatis_resources

}
