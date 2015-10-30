#!/usr/bin/env python
#-*- coding:utf-8 -*-

import json
import logging
import utility_redis
import utility_mysql

def reg_new_acc(conn_handle,disconn_handle,send_handle,reg_acc_req=""):
    conn_id=conn_handle('esb.rs.com.hx_acc.main')
    if not reg_acc_req:
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
                'nick':'',
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
        logging.info('gen_acc error')
        return
    reg_acc_req['jsonReq']['acc']=acc
    json_req_packet=json.dumps(reg_acc_req)
    send_result=send_handle(conn_id,json_req_packet)
    disconn_handle(conn_id)
    reg_acc_rsp=json.loads(send_result)
    if 0>reg_acc_rsp['status']:
        logging.info(pack_msg(reg_acc_req,None,'reg acc failed,network error'))
        return
    if 0<reg_acc_rsp['status']:
        logging.info(pack_msg(reg_acc_req,reg_acc_rsp,'reg acc error.status=%s' % hex(reg_acc_rsp['status'])))
        return
    return (reg_acc_req, reg_acc_rsp)
        
