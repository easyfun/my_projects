#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
import MySQLdb
from datetime import datetime, date
from _mysql import result

time_format='%y-%m-%d-%H-%M-%S'
now_string=datetime.now().strftime(time_format)

new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest')
new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)

def check_financial_invest():
    
    invest_title=('financial_plan_id','investment_id','financial_plan_investment_id','asset_id','asset_type',
                  'asset_pool','asset_state','asset_property','asset_name','annual_rate',
                  'add_rate','phase_total','phase_mode','repay_mode','contract_no',
                  'debt_id','debt_name','investor_uid','borrower_uid','amount',
                  'valid_amount','percentage','init_percentage','conpon_id','conpon_type',
                  'state','debt_property','from_device','rest_phase','expect_principal',
                  'expect_interest','expect_add_interest','expect_pay_platform','received_principal','received_interest',
                  'received_add_interest','actual_pay_platform','received_money','next_payoff_day','lock_day',
                  'update_time','create_time','finish_time','full_time')
    title=''
    for t in invest_title:
        title+= '%30s,' % t
    print(title)
    
    for i in range(100):
        sql="select * from financial_plan.t_investment_%02d where asset_type=1 and state in (2) and date(create_time)>'2016-11-17';" %(
            i,)
        
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        for result in results:
            data=''
            for t in invest_title:
                data+='%30s,' % str(result[t])
            print(data)

if '__main__'==__name__:
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    check_financial_invest()
    end=datetime.now()
    
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))