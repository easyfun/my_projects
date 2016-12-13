#!/usr/bin/env python
# -*- coding:utf-8 -*-


import financial_investor
import MySQLdb
from datetime import datetime, date
from pip._vendor.html5lib.treewalkers import pprint

time_format='%y-%m-%d-%H-%M-%S'
new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest', charset='utf8')
new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)

def check_account_flow():
    uids=financial_investor.uids
    difficult_uids=[]
    
    for uid in uids:
        sql="select count(0) as n from account.t_user_account_flow_%02d where uid=%d and create_time>'2016-11-03 22:00:00' and create_time<='2016-11-17 20:00:00'" %(
            uid%100,
            uid)
        
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        for r in results:
            n=r['n']
            if n>=100:
                print(uid,results)
                difficult_uids.append(uid)
                break
            
    print('\n\n\n-----------------------------------------------')
    print('account num=%d' % len(difficult_uids))
    for uid in difficult_uids:
        sql="select d.real_name,u.mobile from user.t_user u,user.t_user_detail d where d.userid=%d and u.id=%d" % (uid, uid)
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        for r in results:
            print("%d,%s,%s" % (uid, r['real_name'], r['mobile']))
    print('-----------------------------------------------\n\n\n')

if '__main__'==__name__:
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    check_account_flow()
    end=datetime.now()
    
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))