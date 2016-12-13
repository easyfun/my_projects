#!/usr/bin/env python
# -*- coding:utf-8 -*-

import MySQLdb
from datetime import datetime
from _mysql import result

def check_account_flow():
    new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'account')
    new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)

    trans_ids=(201611070002907201,201611070002908201,201611070002856201,
               201611070002931201,201611070002857201,201611070002940201,
               201611070002861201,201611070002862201,201611070002863201,
               201611070002874201,201611070002875201,201611070002945201,
               201611070002880201,201611070002953201,201611070002881201,
               201611070003335201,201611070003336201,201611070003337201,
               201611070003595201,201611070003596201,201611070003597201,
               201611070003732201,201611070003666201,201611070003733201,
               201611070003857201,201611070003918201,201611070003919201,
               201611070003936201,201611070003937201,201611070003938201,
               201611070004326201,201611070004259201,201611070004260201,
               201611070004294201,201611070004363201,201611070004295201,
               201611070004556201,201611070004614201,201611070004615201,
               201611070004616201)
    
    flows=[]
    flow_sqls=[]
    for trans_id in trans_ids:
        for i in range(100):
            print('trans_id=%d' % trans_id)
            sql='select * from t_user_account_flow_%02d where trans_id=%d' % (i,trans_id)
            new_cursor.execute(sql)
            results=new_cursor.fetchall()
            for r in results:
                flow_sqls.append(sql)
                flows.append(r)

    
    flow_title=('flow_id','acc_id','uid','type','bus_type',
                'subbus_type','flow_type','is_show','operation','amount',
                'balance','frozen','peer_acc_id','peer_uid','trans_id',
                'loan_id','repayment_id','remark','create_index','freeze_id',
                'service_name','deposit_amt','create_time','deposit_time')
    
    for flow in flows:
        for t in flow_title:
            print '%20s,' % str(flow[t]),
        print('')
    for sql in flow_sqls:
        print(sql)


if '__main__'==__name__:
    check_account_flow()