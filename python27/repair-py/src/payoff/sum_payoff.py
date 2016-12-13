#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
import MySQLdb
from datetime import datetime, date
from _mysql import result

time_format='%y-%m-%d-%H-%M-%S'
now_string=datetime.now().strftime(time_format)
payoff_file='check_payoffid_report_%s.data' % now_string 
fix_payoff_id_sqls_file='fix_payoffid_sqls_%s.data' % now_string

new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest')
new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)

def check_payoff():
    sum_principal_interest=0
    
    for i in range(100):
        sql = "select * from invest.t_investment_payoff_%02d where asset_type=0 and asset_pool=2 and asset_id=%d and repayment_id=%d" % (
            i,
            3810905692996879L,
            300000000101264079L)
        print(sql)
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        for r in results:
            print(r)
            sum_principal_interest += r['expect_principal']
            sum_principal_interest += r['expect_interest']
    
    print(sum_principal_interest)

    

if '__main__'==__name__:
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    check_payoff()
    end=datetime.now()
    
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))