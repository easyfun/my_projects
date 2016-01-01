#!/usr/bin/env python
# -*- coding:utf-8 -*-

import time
import datetime
import json
import py_client

'''
request
{
    "src_type":0,
    "src_id":0,
    "dest_type":0,
    "dest_id":0,
    "msg_id":0,
    "version":0,
    "data":{}
}

response
{
    "src_type":0,
    "src_id":0,
    "dest_type":0,
    "dest_id":0,
    "msg_id":0,
    "error_code":0,
    "data":{}
}
'''

def RunTest():
    #print(help(py_client))
    if not py_client.Init():
        print("init fail")
        return

    #连接config_server
    conn_id=py_client.Connect("tcp:192.168.1.101:10000",True)
    print("conn_id=%d" % conn_id)

    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":4,
        "dest_id":0,
        "msg_id":1000,
        "version":0,
        "data":{
            "spec":"tcp:192.168.1.100:8888",
            "srv_type":65001,
            "srv_id":999,
            "dep_srv_type":[0]
        }
    }
    req_str=json.dumps(request)
    print('req:%s' % req_str)
    resp_str=py_client.Send(conn_id,req_str)
    print("resp:%s" % resp_str)
    resp_json=json.loads(resp_str)
    if resp_json['function_return']:
        print('register fail,error_msg=%s' 
            % resp_json['function_return'])
        return

    if 0!=resp_json['error_code']:
        print('register fail,error_code=%d' %
            resp_json['error_code'])
        return

if '__main__'==__name__:
    RunTest()