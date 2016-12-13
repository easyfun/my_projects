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

def with_draw():
    operations=(0,1,2,3,4,5,6,7,8,9)
    #operations=(9,)
    bus_types={}
    for operation in operations:
        bus_types[operation]=[]
        for i in range(100):
            sql="select bus_type from account.t_user_account_flow_%02d where operation=%d and date(create_time)>'2016-11-03' group by bus_type;" %(
                i,
                operation)
            
            new_cursor.execute(sql)
            results=new_cursor.fetchall()
            for result in results:
                if 46==result['bus_type']:
                    #print(sql)
                    pass
                if not result['bus_type'] in bus_types[operation]:
                    bus_types[operation].append(result['bus_type'])
                    
    for k,v in bus_types.iteritems():
        print(k,sorted(v))
        
        

if '__main__'==__name__:
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    with_draw()
    end=datetime.now()
    
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))