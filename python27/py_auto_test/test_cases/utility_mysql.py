#!/usr/bin/env python
#-*- coding:utf-8 -*-

import mysql.connector
# 注意把password设为你的root口令:
conn = mysql.connector.connect(user='root', password='trxhfly', database='account_system_db', use_unicode=True)

def get_user(type, id):
    if type not in ('uid','acc','phone','email'):
        return None
    if not id:
        return None

    user=None
    values=None
    uid=None
    query=None
    if type=='uid':
        query='select * from user_info where uid=%s' % id
    elif type=='acc':
        query='select * from user_info where acc="%s"' % id
    elif type=='phone':
        query='select * from user_info where phone="%s"' % id
    elif type=='email':
        query='select * from user_info where email="%s"' % id
    if query:
        print('query=',query)
        #values=redis_obj.hmget(uid,user_fields)
        cursor = conn.cursor(dictionary=True)
        cursor.execute(query)
        values = cursor.fetchall()
        # 关闭Cursor和Connection:
        cursor.close()
        if 1==len(values):
            user=values[0]
    return user

if '__main__'==__name__:
    '''
    user=get_user('uid','100000')
    print(user)
    print(user['uid'])
    print(user['acc'])
    '''

    user=get_user('acc','svqymidfc6m9')
    if user:
        print(user)
        print(user['uid'])
        print(user['acc'])










