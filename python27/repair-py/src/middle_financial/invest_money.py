#!/usr/bin/env python
#-*- coding:utf-8 -*-


import MySQLdb


new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest', charset='utf8')
new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)


def check():
    sql="select * from product.t_assets where asset_type=1 and publish_time>'2016-09-01 00:00:00'"
    new_cursor.execute(sql)
    assets=new_cursor.fetchall()
    for asset in assets:
        #print(asset)
        if asset['state']<=5:
            print('未满标,asset=%s' % str(asset))
            continue
        
        sql="select sum(amount) as total_invest_amount from product.t_investment_%02d where state in (2,4,6,7) and asset_id=%d" %(
            asset['asset_id']%100,
            asset['asset_id'])
        new_cursor.execute(sql)
        results=new_cursor.fetchall()
        for r in results:
            if r['total_invest_amount'] != asset['total_amount']:
                print('sql=%s' % sql)
                print("asset=%s" % str(asset))
                print('total_invest_amount=%d, total_amount=%d' % (r['total_invest_amount'],asset['total_amount']))
                print('----------------------\n')
            break
        

if '__main__'==__name__:
    check()