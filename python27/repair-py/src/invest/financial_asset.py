#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
import MySQLdb
from datetime import datetime, date

repayment_file='check_repayment_id_report_%s.data' % datetime.now().strftime('%y-%m-%d-%H-%M-%S')
payoff_file='check_payoff_report_%s.data' % datetime.now().strftime('%y-%m-%d-%H-%M-%S')

repayment_file_content=''
payoff_file_content=''

def check_invest():
    new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest')
    new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)
    #new_cursor = new_conn.cursor()
    
    title=('payoff_id','financial_plan_id','investment_id','financial_plan_investment_id',
           'asset_id','asset_name','asset_type','asset_pool','investor_uid','borrower_uid',
           'amount','annual_rate','add_rate','repayment_id','phase','expect_principal',
           'expect_interest','expect_add_interest','expect_pay_platform','expect_date',
           'actual_principal','actual_interest','actual_add_interest','actual_pay_platform',
           'actual_date','state','property','update_time','create_time')
    
    payoffs=[]
    for i in range(100):
        sql='select * from invest.t_investment_payoff_%02d where repayment_id=%d' %(i,300000000101254126L)
        #print(sql)
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        payoffs.extend(results)
        
    for payoff in payoffs:
        for t in title:
            print '%20s,' % str(payoff[t]),
        print('')
    print('')
    actual_principal=0
    actual_interest=0
    for payoff in payoffs:
        actual_principal+=payoff['actual_principal']
        actual_interest+=payoff['actual_interest']
    print('actual_principal=',actual_principal)
    print('actual_interest=',actual_interest)
    
    new_conn.close()


def check_invest_by_repayment_id(new_conn, repayment):
    global payoff_file_content

    new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)
    #new_cursor = new_conn.cursor()
    
    payoff_title=('payoff_id','financial_plan_id','investment_id','financial_plan_investment_id',
           'asset_id','asset_name','asset_type','asset_pool','investor_uid','borrower_uid',
           'amount','annual_rate','add_rate','repayment_id','phase','expect_principal',
           'expect_interest','expect_add_interest','expect_pay_platform','expect_date',
           'actual_principal','actual_interest','actual_add_interest','actual_pay_platform',
           'actual_date','state','property','update_time','create_time')
    
    payoffs=[]
    for i in range(100):
        sql='select * from invest.t_investment_payoff_%02d where repayment_id=%d' %(i,repayment['repayment_id'])
        #print(sql)
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        payoffs.extend(results)
        
    payoff_file_content += str(repayment)
    payoff_file_content += '\n'
    
    for payoff in payoffs:
        for t in payoff_title:
            payoff_file_content += '%20s,' % str(payoff[t])
        payoff_file_content += '\n'
    
    actual_principal=0
    actual_interest=0
    
    actual_principal_from_debt=0
    actual_interest_from_debt=0
    
    for payoff in payoffs:
        if payoff['asset_id'] == 0:
            actual_principal_from_debt += payoff['actual_principal']
            actual_interest_from_debt += payoff['actual_interest'] 
            
        actual_principal += payoff['actual_principal']
        actual_interest += payoff['actual_interest']
    payoff_file_content += ('actual_principal_from_debt=%d, actual_principal=%d, diff=%d\n' 
                               % (actual_principal_from_debt, actual_principal, repayment['actual_principal']-actual_principal))
    payoff_file_content += ('actual_interest_from_debt=%d, actual_interest=%d, diff=%d\n' 
                               % (actual_interest_from_debt, actual_interest, repayment['actual_interest']-actual_interest))


def check_payoff():
    global repayment_file_content

    new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest')
    new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)
    
    repayment_ids=[]
    
    for i in range(100):
        sql='select * from product.t_repayment_%02d where asset_type=0 and asset_pool in (2,4) and actual_date<="2016-11-17 20:00:00" and actual_date>="2016-11-04 00:00:00"' % (i,)
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        for r in results:
            if r['repayment_id'] not in repayment_ids:
                print(r['repayment_id'])
                repayment_ids.append(r['repayment_id'])
                check_invest_by_repayment_id(new_conn, r)
                repayment_file_content += ("%d\n" % r['repayment_id'])
        
    with open(os.path.join(os.path.abspath(os.curdir),repayment_file), 'w') as f:
        f.write(repayment_file_content)
        
    with open(os.path.join(os.path.abspath(os.curdir),payoff_file), 'w') as f:
        f.write(payoff_file_content)


if '__main__'==__name__:
    #check_invest()
    
    start=datetime.now()
    print('开始时间=%s' % str(start))
    check_payoff()
    end=datetime.now()
    print('开始时间=%s' % str(start))
    print('结束时间=%s' % str(end))
    print('耗时=', end-start)
