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

    #连接
    conn_id=py_client.Connect("tcp:127.0.0.1:10020",True)
    print("conn_id=%d" % conn_id)

    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":3,
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

    #回显
    '''
    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":3,
        "dest_id":0,
        "msg_id":0,
        "version":0,
        "data":{
            "message":"helloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworld"
        }
    }
    req_str=json.dumps(request)
    start=datetime.datetime.now()
    n=0
    while n < 10000:
        n+=1
        resp_str=py_client.Send(conn_id,req_str)
        resp_json=json.loads(resp_str)
        #print('resp:%s' % resp_json)
        if resp_json['function_return']:
            print('request fail,error_msg=%s, n=%d'
                % (resp_json['function_return'],n))
            break;
        if 0!=resp_json['error_code']:
            print('request fail,error_code=%d' %
                resp_json['error_code'])
            break

    end=datetime.datetime.now()
    print("time=%d" % (end-start).seconds)
    '''

    #注册
    print("------test Register")
    password="123456"
    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":3,
        "dest_id":0,
        "msg_id":1000,
        "version":0,
        "data":{
            "account":"",
            "password":password
        }
    }
        
    account="yangjinbin_%08d" % time.time()
    request['data']['account']=account

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

    #设置账号信息
    print("------test SetAccountInfoReq");
    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":3,
        "dest_id":0,
        "msg_id":1002,
        "version":0,
        "data":{
            "account":account,
            "img_url":"http://www.lljz.com/img_server/123.png",
            "home_page":"http://www.lljz.com/"
        }
    }
    req_str=json.dumps(request)
    print("req_str:%s" % req_str)
    resp_str=py_client.Send(conn_id,req_str)
    resp_json=json.loads(resp_str)
    print('resp:%s' % resp_json)
    if resp_json['function_return']:
        print('request fail,error_msg=%s'
            % (resp_json['function_return']))
    if 0!=resp_json['error_code']:
        print('request fail,error_code=%d' %
            resp_json['error_code'])

    #修改登陆密码
    print("------test ModifyLoginPasswordReq");
    new_password="234567"
    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":3,
        "dest_id":0,
        "msg_id":1004,
        "version":0,
        "data":{
            "account":account,
            "old_password":password,
            "new_password":"234567"
        }
    }
    req_str=json.dumps(request)
    print("req_str:%s" % req_str)
    resp_str=py_client.Send(conn_id,req_str)
    resp_json=json.loads(resp_str)
    print('resp:%s' % resp_json)
    if resp_json['function_return']:
        print('request fail,error_msg=%s'
            % (resp_json['function_return']))
    if 0!=resp_json['error_code']:
        print('request fail,error_code=%d' %
            resp_json['error_code'])

    #登陆
    print("------test LoginReq");
    print("++++++wrong password")
    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":3,
        "dest_id":0,
        "msg_id":1006,
        "version":0,
        "data":{
            "account":account,
            "password":"wrong_password"
        }
    }
    req_str=json.dumps(request)
    print("req_str:%s" % req_str)
    resp_str=py_client.Send(conn_id,req_str)
    resp_json=json.loads(resp_str)
    print('resp:%s' % resp_json)
    if resp_json['function_return']:
        print('request fail,error_msg=%s'
            % (resp_json['function_return']))
    if 0!=resp_json['error_code']:
        print('request fail,error_code=%d' %
            resp_json['error_code'])

    print("++++++right password")
    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":3,
        "dest_id":0,
        "msg_id":1006,
        "version":0,
        "data":{
            "account":account,
            "password":new_password
        }
    }
    req_str=json.dumps(request)
    print("req_str:%s" % req_str)
    resp_str=py_client.Send(conn_id,req_str)
    resp_json=json.loads(resp_str)
    print('resp:%s' % resp_json)
    if resp_json['function_return']:
        print('request fail,error_msg=%s'
            % (resp_json['function_return']))
    if 0!=resp_json['error_code']:
        print('request fail,error_code=%d' %
            resp_json['error_code'])

    #登出
    print("------test LogoutReq");
    print("++++++wrong password")
    request = {
        "src_type":65001,
        "src_id":1,
        "dest_type":3,
        "dest_id":0,
        "msg_id":1008,
        "version":0,
        "data":{
            "account":account,
            "password":new_password
        }
    }
    req_str=json.dumps(request)
    print("req_str:%s" % req_str)
    resp_str=py_client.Send(conn_id,req_str)
    resp_json=json.loads(resp_str)
    print('resp:%s' % resp_json)
    if resp_json['function_return']:
        print('request fail,error_msg=%s'
            % (resp_json['function_return']))
    if 0!=resp_json['error_code']:
        print('request fail,error_code=%d' %
            resp_json['error_code'])

    py_client.Stop()
    py_client.Wait()


if "__main__"==__name__:
    RunTest()