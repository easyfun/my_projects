#!/usr/bin/env python
# -*- coding:utf-8 -*-

import time
import datetime
import json
import py_client

def RunTest():
    #print(help(py_client))
    if not py_client.Init():
        print("init fail")
        return

    #连接文件服务器
    conn_id=py_client.Connect("tcp:127.0.0.1:10030",True)
    print("conn_id=%d" % conn_id)

    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":4,
        "dest_id":0,
        "msg_id":2,
        "version":0,
        "data":{}
    }
    req_str=json.dumps(request)
    #print('req:%s' % req_str)
    resp_str=py_client.Send(conn_id,req_str)
    #print("resp:%s" % resp_str)
    resp_json=json.loads(resp_str)
    if resp_json['function_return']:
        print('register fail,error_msg=%s' 
            % resp_json['function_return'])
        return

    if 0!=resp_json['error_code']:
        print('register fail,error_code=%d' %
            resp_json['error_code'])
        return

    account="lljzfly"
    password="123456"
    #上传文件
    print("------test UploadFileReq")
    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":4,
        "dest_id":0,
        "msg_id":1004,
        "version":0,
        "data":{
            "account":account,
            "password":password,
            "file_name":"my_thinking",
            "seq_no":1,
            "length":41,
            "data":"follow nature,not force,that is my heart."
        }
    }
        
    req_str=json.dumps(request)
    print("req_str:%s" % req_str)
    resp_str=py_client.Send(conn_id,req_str)
    print('resp:%s' % resp_str)
    resp_json=json.loads(resp_str)
    if resp_json['function_return']:
        print('request fail,error_msg=%s'
            % (resp_json['function_return']))
    if 0!=resp_json['error_code']:
        print('request fail,error_code=%d' %
            resp_json['error_code'])

    py_client.Disconnect(conn_id)

    py_client.Stop()
    py_client.Wait()


if "__main__"==__name__:
    RunTest()