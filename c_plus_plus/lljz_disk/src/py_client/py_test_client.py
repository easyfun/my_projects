#!/usr/bin/env python
# -*- coding:utf-8 -*-

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

    #连接
    conn_id=py_client.Connect("tcp:127.0.0.1:10020",True)
    print("conn_id=%d" % conn_id)

    #注册
    '''
    request = {
        "src_type":1001,
        "src_id":0,
        "dest_type":1,
        "dest_id":0,
        "msg_id":2,
        "version":0,
        "data":{
            "spec":"tcp:127.0.0.1:8000",
            "srv_type":1001,
            "srv_id":100,
            "dep_srv_type":[1,2,3]
        }
    }
    '''
    request = {
        "src_type":1001,
        "src_id":1,
        "dest_type":1,
        "dest_id":0,
        "msg_id":4,
        "version":0,
        "data":{}
    }
    req_str=json.dumps(request)
    resp_str=py_client.Send(conn_id,req_str)
    print("resp:%s" % resp_str)

    #回显
    request = {
        "src_type":1001,
        "src_id":1,
        "dest_type":1,
        "dest_id":0,
        "msg_id":0,
        "version":0,
        "data":{
            "message":"hello world"
        }
    }
    req_str=json.dumps(request)
    start=datetime.datetime.now()
    n=0
    while n < 10000:
        resp_str=py_client.Send(conn_id,req_str)
    end=datetime.datetime.now()
    print("time=%d" % (end-start).seconds)
    #print("resp:%s" % resp_str)

    py_client.Stop()
    py_client.Wait()


if "__main__"==__name__:
    RunTest()