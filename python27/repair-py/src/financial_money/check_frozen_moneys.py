#!/usr/bin/env python
#-*- coding:utf-8 -*-

import MySQLdb
from datetime import datetime, date
import os
import financial_investor
import frozen_money_results
import pprint

time_format='%y-%m-%d-%H-%M-%S'
new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest')
new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)


def reversed_cmp(x, y):
    m1=x[1]
    m2=y[1]
    if m1 > m2:
        return -1
    if m1 < m2:
        return 1
    return 0    
    

def check1():
    uids=financial_investor.uids
    moneys=frozen_money_results.financial_frozen_moneys
    
    #检查uid是否漏
    for money in moneys:
        find=False
        for uid in uids:
            if uid==money[0]:
                find=True
        if False==find:
            print('uid不在复核列表中,uid=%d' % money[0])
            exit()
            
    #检查账上冻结资金是否足够
    total_frozen=0
    for money in moneys:
        uid=money[0]
        frozen=money[1]
        total_frozen += frozen
        sql='select * from account.t_user_account_%02d where type=2 and uid=%d' % (uid%100, uid)
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        for r in results:
            if frozen > r['frozen']:
                print('冻结资金错误,uid=%d,frozen=%d,frozen_in_db=%d' % (uid, frozen, r['frozen']))

            if frozen < r['frozen']:
                print('冻结资金小,uid=%d,frozen=%d,frozen_in_db=%d' % (uid, frozen, r['frozen']))
            
    print(total_frozen)
    
    sort_moneys=sorted(moneys, reversed_cmp)
    pprint.pprint(sort_moneys)
    
    
    with open(os.path.join(os.path.abspath(os.curdir),'结果-理财计划冻结资金-从大到小排序.txt'),'w') as f:
        content='用户id,理财计划冻结资金\n'
        for m in sort_moneys:
            c='%d,%d\n' % (m[0], m[1])
            content += c
        print()
        f.write(content)

def check2():
    with open(os.path.join(os.path.abspath(os.curdir),'a.txt'), 'r') as f:
        for line in f.readlines():
            ls=line.split(',')
            if not ls[0].startswith('2016'):
                continue
            
            uid=long(ls[0])
            frozen=long(ls[1])
            sql='select * from account.t_user_account_%02d where type=2 and uid=%d' % (uid%100, uid)
            new_cursor.execute(sql)
            results=new_cursor.fetchall()
            for r in results:
                if frozen > r['frozen']:
                    print('冻结资金错误,uid=%d,frozen=%d,frozen_in_db=%d' % (uid, frozen, r['frozen']))
    
                if frozen < r['frozen']:
                    print('冻结资金小,uid=%d,frozen=%d,frozen_in_db=%d' % (uid, frozen, r['frozen']))

    
if '__main__'==__name__:
    
    
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    check2()
    end=datetime.now()
     
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))