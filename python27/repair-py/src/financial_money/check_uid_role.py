#!/usr/bin/env python
#-*- coding:utf-8 -*-

import financial_investor
import MySQLdb
from datetime import datetime, date

time_format='%y-%m-%d-%H-%M-%S'
new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest')
new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)

def check_role():
    uids=financial_investor.uids
    
    for uid in uids:
        sql = 'select asset_id from product.t_assets where borrower_uid=%d' % uid
        print (sql)
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        if len(results)>0:
            print(sql)
            print(results)
        for r in results:
            if r['asset_id']>0:
                print('%d,投资人,借款人')
                break

if '__main__'==__name__:
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    check_role()
    end=datetime.now()
    
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))