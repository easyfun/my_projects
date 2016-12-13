#!/usr/bin/env python
# -*- coding:utf-8 -*-


import financial_investor
import MySQLdb
from datetime import datetime, date

time_format='%y-%m-%d-%H-%M-%S'
new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest')
new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)

def check_with_draw():
    uids=financial_investor.uids
    
    count=0
    for uid in uids:
        sql="select * from account.t_user_account_flow_%02d where uid=%d and operation=1 and create_time>'2016-11-03 22:00:00' and create_time<='2016-11-17 20:00:00'" %(
            uid%100,
            uid)
        
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        count += len(results)
    print(count)

if '__main__'==__name__:
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    check_with_draw()
    end=datetime.now()
    
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))