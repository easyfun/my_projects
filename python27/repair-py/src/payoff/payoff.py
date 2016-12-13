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
    error_payoffs=[]
    for i in range(100):
        sql = "select * from invest.t_investment_payoff_%02d where asset_type=0 and asset_pool=2 and (date(create_time)='2016-11-03' or date(create_time)='2016-11-04')" % (i,)
        print(sql)
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        for r in results:
            #print(r)
            if r['payoff_id']%100 != r['investor_uid']%100:
                error_payoffs.append(r)

    
    payoff_title=('payoff_id','financial_plan_id','investment_id','financial_plan_investment_id',
           'asset_id','asset_name','asset_type','asset_pool','investor_uid','borrower_uid',
           'amount','annual_rate','add_rate','repayment_id','phase','expect_principal',
           'expect_interest','expect_add_interest','expect_pay_platform','expect_date',
           'actual_principal','actual_interest','actual_add_interest','actual_pay_platform',
           'actual_date','state','property','update_time','create_time')
    
    payoff_file_content=''
    fix_payoff_id_sqls_file_content=''
    for payoff in error_payoffs:
        for t in payoff_title:
            payoff_file_content += '%20s,' % str(payoff[t])
        payoff_file_content += '\n'
        #print(payoff_file_content)
        #payoff_id=%d and financial_plan_id=%d and investment_id=%d and financial_plan_investment_id=%d and asset_id=%d and investor_uid=%d
        #10000000000000
        state=3
        if payoff['actual_date'] != None and (payoff['actual_interest']>0 or payoff['actual_principal']>0) and payoff['actual_principal']+payoff['actual_interest'] == payoff['expect_principal']+payoff['expect_interest']:
            state=3
        else:
            state=0
            
        fix_sql="update invest.t_investment_payoff_%02d set payoff_id=%d, state=%d where asset_pool=2 and asset_type=0 and payoff_id=%d and financial_plan_id=%d and investment_id=%d and financial_plan_investment_id=%d and asset_id=%d and investor_uid=%d and expect_principal+expect_interest=actual_interest+actual_principal;" % (
            payoff['investor_uid']%100,
            long('900%d%02d' % (payoff['payoff_id']%10000000000000L,payoff['investor_uid']%100)),
            state,
            payoff['payoff_id'],
            payoff['financial_plan_id'],
            payoff['investment_id'],
            payoff['financial_plan_investment_id'],
            payoff['asset_id'],
            payoff['investor_uid'])
        fix_sql+='\n'
        fix_payoff_id_sqls_file_content += fix_sql
        
    with open(os.path.join(os.path.abspath(os.curdir),payoff_file), 'w') as f:
        f.write(payoff_file_content)

    with open(os.path.join(os.path.abspath(os.curdir),fix_payoff_id_sqls_file), 'w') as f:
        f.write(fix_payoff_id_sqls_file_content)



if '__main__'==__name__:
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    check_payoff()
    end=datetime.now()
    
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))