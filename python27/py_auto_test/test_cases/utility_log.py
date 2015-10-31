#!/usr/bin/env python
#-*- coding:utf-8 -*-

import smtplib  
from email.mime.text import MIMEText

config_send_email=False

mail_host="smtp.ym.163.com"  #设置服务器
mail_user="test"    #用户名
mail_pass="test"   #口令 
mail_postfix="qq.com"  #发件箱的后缀

def send_mail(to_list,sub,content):  
    #me="hello"+"<"+mail_user+"@"+mail_postfix+">"
    me=mail_user
    msg = MIMEText(content,_subtype='plain',_charset='gb2312')
    msg['Subject'] = sub
    msg['From'] = me
    msg['To'] = ";".join(to_list)
    try:
        server = smtplib.SMTP()
        server.connect(mail_host)
        server.login(mail_user,mail_pass)
        server.sendmail(me, to_list, msg.as_string())
        server.close()
        return True
    except Exception, e:
        print str(e)
        return False

msg_with_worker=[
    {
        'name':'yangjinbin','email':'1060887552@qq.com','msgID':'0x0001'
    }
]

def send_email(msg_id,msg):
    to_list=[]
    for e in msg_with_worker:
        if e['msgID'].index(msg_id) >= 0:
            to_list.append(e['email'])
            break
            
    if to_list:
        send_mail(to_list,'software bug',msg)

def pack_msg(req_packet, rsp_packet, remarks,send_email_flag=False):
    msg=''
    if req_packet:
        msg=msg+'request: '+str(req_packet)+'\n'
    if rsp_packet:
        msg=msg+'response: '+str(rsp_packet)+'\n'
    if remarks:
        msg=msg+'remarks: '+str(remarks)+'\n'
    if config_send_email and req_packet:
        send_email(req_packet['msgID'],msg)

    return msg

if '__main__'==__name__:
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
    pack_msg(reg_acc_req,None,'reg acc failed')
    exit(1)
    if send_mail(mailto_list,"hello","hello world"):  
        print "发送成功"  
    else:  
        print "发送失败"  
