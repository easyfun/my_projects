#!/usr/bin/env python
#-*- coding:utf-8 -*-

import json
import sys
import logging
import utility_manager
import utility_public
import utility_redis
import utility_mysql
from utility_log import pack_msg


'''
function:send_handle
return:{'status':0,'errMsg':'error','jsonData':'{}','pro':1,'msgID':1,dstID:1,'srcID':1,'type':1,'seq':1}
        status:!=0 error;-4 send time out
'''


@utility_manager.register_test_case('0x0217')
def Msg_SetUserRealNameReq(conn_handle,disconn_handle,send_handle):
    '''
    conn_id=conn_handle('esb.rs.com.hx_acc.main')
    print('conn_id=', conn_id)
    
    #register a new account
    reg_acc_req={
        'msgID':'0x0001','destID':'0x0010','srcID':'0x0000',
        'jsonReq':{
            'uid':0,
            'acc':'',
            'pwd':'123456',
            'phone':'',
            'email':'',
            'code':'',
            'genid':1,
            'ncd':1,
            'acmail':1,
            'nick':'abw',
            'img_url':'',
            'refer':'',
            'c_r':86,
            'src':1,
            'sys_id':32,
            'ip':'192.168.241.129',
            'mac':'aabbccddeeff',
            'dev':1
        }
    }
    acc=utility_redis.gen_acc()
    if not acc:
        #raise ValueError('gen_acc error')
        logging.info('gen_acc error')
        return
    reg_acc_req['jsonReq']['acc']=acc
    #print('req_packet=',reg_acc_req)
    json_req_packet=json.dumps(reg_acc_req)
    send_result=send_handle(conn_id,json_req_packet)
    #print('reg_acc_req_send_result=',send_result)
    reg_acc_rsp=json.loads(send_result)
    if 0>reg_acc_rsp['status']:
        #raise ValueError('network error')
        logging.info(pack_msg(reg_acc_req,None,'reg acc failed,network error'))
        return
    if 0<reg_acc_rsp['status']:
        #raise ValueError('request data error %s' % hex(reg_acc_rsp['status']))
        logging.info(pack_msg(reg_acc_req,reg_acc_rsp,'reg acc error.status=%s' % hex(reg_acc_rsp['status'])))
        return
    '''
    t=utility_public.reg_new_acc(conn_handle,disconn_handle,send_handle)
    if not t:
        return
    reg_acc_req=t[0]
    reg_acc_rsp=t[1]
    reg_acc_rsp_json_data=reg_acc_rsp['jsonData']

    conn_id=conn_handle('esb.rs.com.hx_acc.main')
    print('conn_id=', conn_id)
    #set real name
    req_packet={
        'msgID':'0x0217','destID':'0x0010','srcID':'0x0000',
        'jsonReq':{
            'uid':0,
            'rname':'Bill Lee',
            'cert':'456789123',
            'cert_type':1,
            'level':4,
            'sys_id':32,
            'ip':'192.168.241.129',
            'mac':'aabbccddeeff',
            'dev':1
        }
    }
    req_packet['jsonReq']['uid']=reg_acc_rsp_json_data['uid']
    cert=utility_redis.gen_cert()
    if not cert:
        #raise ValueError('gen_cert error')
        logging.info(pack_msg(reg_packet,None,'gen cert error'))
        return
    req_packet['jsonReq']['cert']=cert
    json_req_packet=json.dumps(req_packet)
    send_result=send_handle(conn_id,json_req_packet)
    req_rsp=json.loads(send_result)
    if 0>req_rsp['status']:
        #raise ValueError('network error')
        logging.info(pack_msg(req_packet,None,'network error'))
        return
    if 0<req_rsp['status']:
        #raise ValueError('request data error %s' % hex(req_rsp['status']))
        logging.info(pack_msg(req_packet,req_rsp,'status=%s' % hex(req_rsp['status'])))
        return
    #check db
    user=utility_redis.get_user('uid',reg_acc_rsp_json_data['uid'])
    if req_packet['jsonReq']['rname']!=user['rname'] or req_packet['jsonReq']['cert']!=user['idcard'] or req_packet['jsonReq']['cert_type']!=user['cert_type'] or req_packet['jsonReq']['level']!=user['cert_lv']:
        #raise ValueError('redis data does not keep same')
        logging.info(pack_msg(req_packet,req_rsp,'redis data does not keep same'))
        return
    
    disRet=disconn_handle(conn_id)
    conn_id=conn_handle('esb.rs.com.hx_acc.main')
    print('conn_id=', conn_id)
    #check <ec id="0x00100069" chs_desc="实名信息已认证不能被修改"  eng_desc="" />
    req_packet={
        'msgID':'0x0217','destID':'0x0010','srcID':'0x0000',
        'jsonReq':{
            'uid':0,
            'rname':'Joe Lee',
            'sys_id':32,
            'ip':'192.168.241.129',
            'mac':'aabbccddeeff',
            'dev':1
        }
    }
    req_packet['jsonReq']['uid']=reg_acc_rsp_json_data['uid']
    json_req_packet=json.dumps(req_packet)
    send_result=send_handle(conn_id,json_req_packet)
    req_rsp=json.loads(send_result)
    if 0>req_rsp['status']:
        #raise ValueError('network error')
        logging.info(pack_msg(req_packet,None,'network error'))
        return
    if int('0x00100069',16) != req_rsp['status']:
        #raise ValueError('account_system return status code error,excepted code is 00100069')
        logging.info(pack_msg(req_packet,req_rsp,'status=%s, not excepted status=00100069' % hex(req_rsp['status'])))
        return

    disRet=disconn_handle(conn_id)
    conn_id=conn_handle('esb.rs.com.hx_acc.main')
    print('conn_id=', conn_id)
    #check <ec id="0x00100069" chs_desc="实名信息已认证不能被修改"  eng_desc="" />
    req_packet={
        'msgID':'0x0217','destID':'0x0010','srcID':'0x0000',
        'jsonReq':{
            'uid':0,
            'rname':'',
            'cert':'X456719251023',
            'cert_type':1,
            'level':4,
            'sys_id':32,
            'ip':'192.168.241.129',
            'mac':'aabbccddeeff',
            'dev':1
        }
    }
    req_packet['jsonReq']['uid']=reg_acc_rsp_json_data['uid']
    json_req_packet=json.dumps(req_packet)
    send_result=send_handle(conn_id,json_req_packet)
    req_rsp=json.loads(send_result)
    if 0>req_rsp['status']:
        #raise ValueError('network error')
        logging.info(pack_msg(req_packet,None,'network error'))
        return
    if int('0x00100069',16) != req_rsp['status']:
        #raise ValueError('account_system return status code error,excepted code is 00100069')
        logging.info(pack_msg(req_packet,req_rsp,'status=%s, not excepted status=00100069' % hex(req_rsp['status'])))
        return

    disRet=disconn_handle(conn_id)
    conn_id=conn_handle('esb.rs.com.hx_acc.main')
    print('conn_id=', conn_id)
    #check <ec id="0x0010007C" chs_desc="实名信息已认证不能被修改"  eng_desc="" />
    req_packet={
        'msgID':'0x0217','destID':'0x0010','srcID':'0x0000',
        'jsonReq':{
            'uid':0,
            'rname':'Joe Lee',
            'cert':'X456719251023',
            'cert_type':1,
            'level':4,
            'sys_id':32,
            'ip':'192.168.241.129',
            'mac':'aabbccddeeff',
            'dev':1
        }
    }
    req_packet['jsonReq']['uid']=reg_acc_rsp_json_data['uid']
    json_req_packet=json.dumps(req_packet)
    send_result=send_handle(conn_id,json_req_packet)
    req_rsp=json.loads(send_result)
    if 0>req_rsp['status']:
        #raise ValueError('network error')
        logging.info(pack_msg(req_packet,None,'network error'))
        return
    if int('0x0010007C',16) != req_rsp['status']:
        #raise ValueError('account_system return status code error,excepted code is 0010007C')
        logging.info(pack_msg(req_packet,req_rsp,'status=%s,not excepted status=0010007C' % hex(req_rsp['status'])))
        return
    return

    disRet=disconn_handle(conn_id)
    conn_id=conn_handle('esb.rs.com.hx_acc.main')
    print('conn_id=', conn_id)
    #check set only rname
    if not utility_redis.set_user('uid',reg_acc_rsp_json_data['uid'],{'rname':'','cert':'','cert_type':'','level':''}):
        raise ValueError('redis set_user error')
    req_packet={
        'msgID':'0x0217','destID':'0x0010','srcID':'0x0000',
        'jsonReq':{
            'uid':0,
            'rname':'Tian he',
            'sys_id':32,
            'ip':'192.168.241.129',
            'mac':'aabbccddeeff',
            'dev':1
        }
    }
    req_packet['jsonReq']['uid']=reg_acc_rsp_json_data['uid']
    json_req_packet=json.dumps(req_packet)
    send_result=send_handle(conn_id,json_req_packet)
    req_rsp=json.loads(send_result)
    if 0>req_rsp['status']:
        #raise ValueError('network error')
        logging.info(pack_msg(req_packet,None,'network error'))
        return
    if 0 != req_rsp['status']:
        #raise ValueError('account_system return status code error,excepted code is 0')
        logging.info(pack_msg(req_packet,req_rsp,'status=%s,not excepted status=0' % hex(req_rsp['status'])))
        return

    disRet=disconn_handle(conn_id)
    conn_id=conn_handle('esb.rs.com.hx_acc.main')
    print('conn_id=', conn_id)
    #check set only cert
    if not utility_redis.set_user('uid',reg_acc_rsp_json_data['uid'],{'rname':'','cert':'','cert_type':'','level':''}):
        raise ValueError('redis set_user error')
    req_packet={
        'msgID':'0x0217','destID':'0x0010','srcID':'0x0000',
        'jsonReq':{
            'uid':0,
            'rname':'',
            'cert':'X456719251023',
            'cert_type':1,
            'level':4,
            'sys_id':32,
            'ip':'192.168.241.129',
            'mac':'aabbccddeeff',
            'dev':1
        }
    }
    req_packet['jsonReq']['uid']=reg_acc_rsp_json_data['uid']
    json_req_packet=json.dumps(req_packet)
    send_result=send_handle(conn_id,json_req_packet)
    req_rsp=json.loads(send_result)
    if 0>req_rsp['status']:
        #raise ValueError('network error')
        logging.info(pack_msg(req_packet,None,'network error'))
        return
    if 0 != req_rsp['status']:
        #raise ValueError('account_system return status code error,excepted code is 0')
        logging.info(pack_msg(req_packet,req_rsp,'status=%s,not excepted status=0' % hex(req_rsp['status'])))
        return

    disRet=disconn_handle(conn_id)

