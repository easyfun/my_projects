#/usr/bin/env python
#*- coding:utf-8 -*-
import random
import redis

'''
r=redis.StrictRedis(host='192.168.20.130',port=6379,db=0)
ret=r.hmget('100000',['acc','pwd','sex','src','phone','email'])
print(ret)
'''

redis_pool=redis.ConnectionPool(host='192.168.241.129', port=6379, db=0)
redis_obj=redis.Redis(connection_pool=redis_pool)

def get_user(type,id):
    if type not in ('uid','acc','phone','email'):
        return None
    if not id:
        return None

    user=None
    values=None
    uid=None
    if type=='uid':
        uid=id
    elif type=='acc':
        uid=redis_obj.hget('acc_uid',id)
    elif type=='phone':
        uid=redis_obj.hget('phone_uid',id)
    elif type=='email':
        uid=redis_obj.hget('email_uid',id)
    if uid:
        user=redis_obj.hgetall(uid)
    if user:
        if isinstance(user['cert_type'],str):
            user['cert_type']=int(user['cert_type'])
        if isinstance(user['cert_lv'],str):
            user['cert_lv']=int(user['cert_lv'])
    return user

'''
not use
'''
user_fields=('acc','pwd','sex','src','phone','email','rname','idcard','cert_type','cert_lv')
def get_userex(type,id):
    if type not in ('uid','acc','phone','email'):
        return None
    if not id:
        return None

    user=None
    values=None
    uid=None
    if type=='uid':
        uid=id
    elif type=='acc':
        uid=redis_obj.hget('acc_uid',id)
    elif type=='phone':
        uid=redis_obj.hget('phone_uid',id)
    elif type=='email':
        uid=redis_obj.hget('email_uid',id)
    if uid:
        values=redis_obj.hmget(uid,user_fields)
    if values:
        user=dict(zip(user_fields,values))
        if isinstance(user['cert_type'],str):
            user['cert_type']=int(user['cert_type'])
        if isinstance(user['cert_lv'],str):
            user['cert_lv']=int(user['cert_lv'])
    return user

def set_user(type,id,**mapping):
    if type not in ('uid','acc','phone','email'):
        return False
    if not id:
        return False
    uid=None
    if type=='uid':
        uid=id
    elif type=='acc':
        uid=redis_obj.hget('acc_uid',id)
    elif type=='phone':
        uid=redis_obj.hget('phone_uid',id)
    elif type=='email':
        uid=redis_obj.hget('email_uid',id)
    if uid:
        return redis_obj.hmset(uid,mapping)

def is_user_exist(type,id):
    if type not in ('uid','acc','phone','email','cert'):
        return False
    if not id:
        return False
    if type=='uid':
        uid=id
        return False
    if type=='acc':
        return redis_obj.hexists('acc_uid',id)
    if 'cert'==type:
        return redis_obj.hexists('idcard_uid',id)

#acc有效字符
ACC_CHAR_SET = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789"
ACC_CHAR_SET_LEN = len(ACC_CHAR_SET)-1

def get_acc():
    acc_len = random.randint(5,19)
    acc = ACC_CHAR_SET[random.randint(0,ACC_CHAR_SET_LEN-10)]
    n = 1
    while n < acc_len:
        n += 1
        i = random.randint(0,ACC_CHAR_SET_LEN)
        acc += ACC_CHAR_SET[i]
    acc+='9'
    return acc

def gen_acc(try_count=20):
    acc=''
    count=0    
    while count < try_count:
        count+=1
        acc=get_acc()
        if not is_user_exist('acc',acc):
            break
        acc=''
    return acc

#cert
CERT_CHAR_SET="aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789"
CERT_CHAR_SET_LEN=len(CERT_CHAR_SET)-1

def get_cert():
    cert_len = random.randint(10,20)
    cert = ''
    n = 0
    while n < cert_len:
        n += 1
        i = random.randint(0,CERT_CHAR_SET_LEN)
        cert += CERT_CHAR_SET[i]
    return cert

def gen_cert(try_count=20):
    cert=''
    count=0
    while count < try_count:
        count+=1
        cert=get_cert()
        if not is_user_exist('cert',cert):
            break
        cert=''
    return cert

if __name__=='__main__':
    print(redis_obj.hgetall('100000'))
    exit(1)
    print(get_user('acc','efmldq652617cfk'))
    print(get_user('uid','100000'))
    print(get_user('phone',None))












