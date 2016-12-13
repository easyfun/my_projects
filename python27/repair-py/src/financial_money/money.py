#!/usr/bin/env python
# -*- coding:utf-8 -*-


'''
非理财计划冻结资金总额
'''

import financial_investor
import MySQLdb
from datetime import datetime, date
import os

time_format='%y-%m-%d-%H-%M-%S'
new_conn = MySQLdb.connect('192.168.51.145','search', 'search@zyxr.com', 'invest')
new_cursor = new_conn.cursor(MySQLdb.cursors.DictCursor)
asset_pools={}
unknown_statuses=[]
to_now_not_create_fuyou_uids=[]
to_17_not_create_fuyou_uids=[]

not_sure_account_flows=0

'''
{
    uid:{
        'not_financial_frozen':0,
        'amount':[],
        'frozen':0,
        'not_sure':[]
    }
}
'''
uid_moneys={}

#uid列表
def get_uids():
    #uids=(201609010004136096L,)
    #uids=(201609010016303048L,)
    #uids=(201609010056726085L,)
    #uids=(201609010014688011L,)
    uids=financial_investor.uids
    return uids



#创建富友账号起点
def get_create_index(uid):
    sql="select create_index from account.t_user_account_flow_%02d where create_time<='2016-11-17 22:00:00' and type=2 and operation=7 and remark='create account' and uid=%d;" %(
        uid%100,
        uid)
    new_cursor.execute(sql)
    results=new_cursor.fetchall()
    
    to_17_index=-1
    for result in results:
        to_17_index = result['create_index']
        break
    
    sql="select create_index from account.t_user_account_flow_%02d where type=2 and operation=7 and remark='create account' and uid=%d;" %(
        uid%100,
        uid)
    new_cursor.execute(sql)
    results=new_cursor.fetchall()
    
    to_now_index=-1
    for result in results:
        to_now_index = result['create_index']
        break
    
    
    return (to_17_index,to_now_index)

#根据标id获得标类型
#0,"无"|1, "散标池"|2, "理财计划池"|3,"散标池和理财计划尺"|4, "理财计划置顶资产池"
#1,定期理财散标|2,理财计划散标|4,理财计划
def get_asset_pool(asset_id):
    asset_type=-1
    asset_pool=-1
    
    if asset_id>0 and asset_id in asset_pools:
        asset_pool=asset_pools[asset_id]

        if -1==asset_pool:
            print('标不存在,asset_id=%d' % asset_id)
            exit()
        return asset_pool
    
    sql='select asset_type,asset_pool from product.t_assets where asset_id=%d' % (
        asset_id,)
    new_cursor.execute(sql)
    results=new_cursor.fetchall()
    
    for result in results:
        asset_type=result['asset_type']
        asset_pool=result['asset_pool']
        break
    if -1==asset_pool or -1==asset_type:
        #print('标不存在2,asset_id=%d' % asset_id)
        #exit()
        return get_debt_pool(asset_id)
    
    if 1==asset_type:
        asset_pool=4
    else:
        if 2==asset_pool or 4==asset_pool:
            asset_pool=2
        else:
            asset_pool=1
    asset_pools[asset_id]=asset_pool
    
#     if asset_id==20161107000028396L:
#         print('asset_pool', asset_pool)
    return asset_pool

#根据转让标id获得标类型
def get_debt_pool(debt_id):
    asset_type=-1
    asset_pool=-1
    if debt_id>0 and debt_id in asset_pools:
        asset_pool=asset_pools[debt_id]

        if -1==asset_pool:
            print('标不存在,debt_id=%d' % debt_id)
            exit()
        return asset_pool
    
    sql='select asset_type,asset_pool from product.t_debts where debt_id=%d' % (
        debt_id,)
    new_cursor.execute(sql)
    results=new_cursor.fetchall()
    
    for result in results:
        asset_type=result['asset_type']
        asset_pool=result['asset_pool']
        break
    if -1==asset_pool or -1==asset_type:
        print('标不存在2,debt_id=%d' % debt_id)
        exit()

    if 1==asset_type:
        asset_pool=4
    else:
        if 2==asset_pool or 4==asset_pool:
            asset_pool=2
        else:
            asset_pool=1

    asset_pools[debt_id]=asset_pool
#     if debt_id==20161107000028396L:
#         print('asset_pool', asset_pool)
    return asset_pool

#检查账号流水
def check_account_flow(uid, create_index):
    uid_moneys[uid]={
        'uid':uid,
        'not_financial_frozen':0,
        'init_frozen':0,
        'final_frozen_17':0,
        'financial_frozen':0,
        'frozen':0,
        'amounts':[],
        'not_sures':[]
    }
    uid_money=uid_moneys[uid]
    
    sql="select f.*,a.frozen as last_frozen from account.t_user_account_flow_%02d f,account.t_user_account_%02d a where  a.type=2 and f.create_time<='2016-11-17 22:15:00' and f.type=2 and f.create_index>=%d and f.uid=%d and a.uid=%d order by f.create_index asc;" % (
        uid % 100,
        uid % 100,
        create_index,
        uid,
        uid)
    new_cursor.execute(sql)
    account_flows=new_cursor.fetchall()
    
    print('account_flows records:', len(account_flows))
    if len(account_flows) > 0:
        uid_money['final_frozen_17']=account_flows[len(account_flows)-1]['frozen']
        uid_money['frozen']=account_flows[len(account_flows)-1]['last_frozen']

    for account_flow in account_flows:
        #print(account_flow)
        
        bus_type=account_flow['bus_type']
        operation=account_flow['operation']
        loan_id=account_flow['loan_id']
        amount=account_flow['amount']
        last_frozen=account_flow['last_frozen']
        uid_money['frozen']=last_frozen


        #operation,bus_type组合
        #(0, [5])
        #(1, [6])
        #(2, [0, 9, 15, 16, 23, 26, 27, 28, 29, 45, 47, 50, 51, 52, 55])
        #(3, [0, 9, 15, 16, 23, 26, 27, 28, 29, 45, 47, 50, 51, 52, 55])
        #(4, [1, 3, 10, 12, 31])
        #(5, [1, 3, 9, 13, 16, 18, 22, 45, 47, 50, 52, 53])
        #(6, [])
        #(7, [7, 45])
        #(8, [3, 9, 10, 16, 18, 19, 23, 30, 46])
        #(9, [3, 9, 10, 16, 18, 19, 23, 30, 46])
        

        #充值，不改变冻结金额
        #O_DEPOSIT(0)
        if 0==operation:
            #(0, [5])
            continue    
        
        #提现
        #O_WITHDRAW(1)
        elif 1==operation:
            #提现操作成功，有账号冻结金额未减少201609010206239196
            #需要修复
            #(1, [6])

            #B_WITHDRAW(6, "提现")
            uid_money['not_financial_frozen'] -= amount
            uid_money['amounts'].append(-1*amount)
            
            #记录trans_id,uid人工确认
            not_sure={}
            not_sure['flow_id']=account_flow['flow_id']
            not_sure['trans_id']=account_flow['trans_id']
            not_sure['uid']=account_flow['uid']
            not_sure['desc']='提现'
            uid_money['not_sures'].append(not_sure)
            continue


        #转入，不改变冻结金额
        #O_TRANSFER_IN(2)
        elif 2==operation:
            #(2, [0, 9, 15, 16, 23, 26, 27, 28, 29, 45, 47, 50, 51, 52, 55])
            continue
        
        #转出，不改变冻结金额
        #O_TRANSFER_OUT(3)
        elif 3==operation:
            #(3, [0, 9, 15, 16, 23, 26, 27, 28, 29, 45, 47, 50, 51, 52, 55])
            continue
        
        #冻结
        #O_FREEZE(4)
        elif 4==operation:
            #(4, [1, 3, 10, 12, 31])
            #B_INVESTMENT(1, "投资")
            if 1==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资冻结'
                    uid_money['not_sures'].append(not_sure)
                    continue
                
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue
                
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            #B_REPAYMENT(3, "借款还款"),借款人还款,冻结本金|利息|费用
            elif 3==bus_type:
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            #平台垫付，垫付方出资金
            #B_COMPENSATORY(10, "垫付"),垫付方代偿还款,冻结本金|利息|费用
            elif 10==bus_type:
                print('状态错误，垫付，冻结方错误')
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            #B_FREEZE(12, "冻结"),转账冻结[与理财计划不相关]
            elif 12==bus_type:
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            #WITHDRAW_FREEZE(31, "提现冻结")
            elif 31==bus_type:
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            else:
                unknown_status='未知状态,operation=%d,bus_type=%d' % (operation, bus_type)
                unknown_statuses.append(unknown_status)
                print(unknown_status)
                
        #解冻
        #O_UNFREEZE(5)
        elif 5==operation:
            #(5, [1, 3, 9, 13, 16, 18, 22, 45, 47, 50, 52, 53])
            
            #B_INVESTMENT(1, "投资")
            if 1==bus_type:
                if 0==loan_id:
                    #理财计划散标投资解冻
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资解冻'
                    uid_money['not_sures'].append(not_sure)
                    continue

                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue

                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                
            #B_REPAYMENT(3, "借款还款"),投资人回款解冻,本金|利息|扣除费用
            elif 3==bus_type:
                #定期理财散标回款，扣除费用
                #可能部分理财计划散标回款解冻，归投资人所有
                '''
                #uid=201609010275093096,定期理财散标没有填loan_id
                '''
                if 0==loan_id:
                    #理财计划散标投资解冻
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资人回款解冻,本金|利息|扣除费用'
                    uid_money['not_sures'].append(not_sure)
                    continue

                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue

                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
            
            #B_COMMON(9, "付款"),服务费保证金其它费,借款人向平台交费
            elif 9==bus_type:
                #借款人，解冻服务费，随后转到平台|垫付方
                '''
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='借款人还款向平台缴费'
                    uid_money['not_sures'].append(not_sure)
                    continue
                '''
                print('告警，投资人不应该是借款方,uid=%d' % uid)
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                
            #B_UNFREEZE(13, "解冻"),[人工解冻]
            elif 13==bus_type:
                '''
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='解冻'
                    uid_money['not_sures'].append(not_sure)
                    continue


                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue
                '''

                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
            
            #B_FREEZE_TO_FREEZE(16, "冻结到冻结"),到中间户
            elif 16==bus_type:
                not_sure={}
                not_sure['flow_id']=account_flow['flow_id']
                not_sure['trans_id']=account_flow['trans_id']
                not_sure['uid']=account_flow['uid']
                not_sure['desc']='冻结到冻结'
                uid_money['not_sures'].append(not_sure)
                continue

                '''
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='冻结到冻结'
                    uid_money['not_sures'].append(not_sure)
                    continue

                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue

                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                '''
            
            #LOAN(18, "借款放款"),平台放款给借款人，扣除费用
            elif 18==bus_type:
                print('告警,即是投资人，又是还款人')
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                
            #CREDIT_ASSIGNMENT(22, "债权转让")
            elif 22==bus_type:
                #理财计划相关，归用户所有
                continue
            
            #YB_TRANSFER(45, "易宝转账")
            elif 45==bus_type:
                #记录trans_id,uid，人工确认
                not_sure={}
                not_sure['flow_id']=account_flow['flow_id']
                not_sure['trans_id']=account_flow['trans_id']
                not_sure['uid']=account_flow['uid']
                not_sure['desc']='易宝转账'
                uid_money['not_sures'].append(not_sure)
                continue
            
            #INTEREST_DEDUCT(47, "投资管理费"),回款投资人向平台缴费
            elif 47==bus_type:
                if 0==loan_id:
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='回款投资人向平台缴费'
                    uid_money['not_sures'].append(not_sure)
                    continue
                
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue
                
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
        
            #PLAN_EXIT(50, "理财计划退出")
            elif 50==bus_type:
                #理财计划相关，归用户所有
                continue

            #PLAN_EXIT_DEDUCT_INTEREST(52, "理财计划退出扣息")
            elif 52==bus_type:
                #理财计划相关，归用户所有
                continue
                            
            #REFUND(53, "回款"),投资人回款，本金|利息|已扣除投资费用
            elif 53==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资人回款'
                    uid_money['not_sures'].append(not_sure)
                    continue
                
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue
                
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)

            else:
                unknown_status='未知状态,operation=%d,bus_type=%d' % (operation, bus_type)
                unknown_statuses.append(unknown_status)
                print(unknown_status)
            
        #O_UNFREEZE_TRANSFER(6)
        elif 6==operation:
            #(6, [])
            unknown_status='未知状态,operation=%d,bus_type=%d' % (operation, bus_type)
            unknown_statuses.append(unknown_status)
            print(unknown_status)

            continue
        
        #创建富友账号
        #O_CREATE(7)
        elif 7==operation:
            #(7, [7, 45])
            
            #B_CREATE(7, "CREATE")
            if 7==bus_type:
                #print('create account continue', account_flow)
                uid_money['init_frozen']=account_flow['frozen']
                uid_money['not_financial_frozen'] += amount
                continue
            
            #YB_TRANSFER(45, "易宝转账"),开户之后有余额转账
            elif 45==bus_type:
                uid_money['init_frozen']=account_flow['frozen']
                uid_money['not_financial_frozen'] += amount
                continue
                
            else:
                unknown_status='未知状态,operation=%d,bus_type=%d' % (operation, bus_type)
                unknown_statuses.append(unknown_status)
                print(unknown_status)
                
        #O_FREEZE_TO_FREEZE_OUT(8)
        elif 8==operation:
            #(8, [3, 9, 10, 16, 18, 19, 23, 30, 46])
            
            #B_REPAYMENT(3, "借款还款"),借款人还本金，转出
            if 3==bus_type:
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                
            #B_COMMON(9, "付款"),服务费保证金其它费,#放款给借款人，借款人缴费
            elif 9==bus_type:
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
            
            #B_COMPENSATORY(10, "垫付"),平台垫付本金,[对投资人-不应该出现的情况]
            elif 10==bus_type:
                print('告警，投资人不应该是垫付方,uid=%d' % uid)
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                
            #B_FREEZE_TO_FREEZE(16, "冻结到冻结")
            elif 16==bus_type:
                #理财计划资金转到中间户
                continue
                
            #LOAN(18, "借款放款"),投资人放款给借款人
            elif 18==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资人放款给借款人'
                    uid_money['not_sures'].append(not_sure)
                    continue
                    
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                
            #REPAYMENT_INTEREST(19, "还款利息"),借款人还利息,[投资，借款，过滤?]
            elif 19==bus_type:
                '''
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    uid_money['not_sures'].append(not_sure)
                    continue
                '''    
                #定期理财散标
                '''
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标，跳过')
                    continue
                '''
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
            
            #LOAN_FEE(23, "放款费用"),放款给借款人，借款人向平台缴费
            #[note]201609010018684019,扣费多,出现负数
            elif 23==bus_type:
#                 print('不可能存在,trans_id=%d,uid=%d' % (account_flow['trans_id'], uid))
#                 exit()
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                #记录trans_id,uid，人工确认
                not_sure={}
                not_sure['flow_id']=account_flow['flow_id']
                not_sure['trans_id']=account_flow['trans_id']
                not_sure['uid']=account_flow['uid']
                uid_money['not_sures'].append(not_sure)
                continue
                
                
            #COMPENSATORY_INTEREST(30, "代偿利息"),垫付方代偿利息,[对投资人-不应该出现的情况]
            elif 30==bus_type:
                print('告警，投资人不应该是垫付方,uid=%d' % uid)
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)

            #COMPENSATORY_FEE(46, "逾期费率"),借款人支付逾期费用
            elif 46==bus_type:
                print('告警，老系统才有,trans_id=%d,uid=%d' % (account_flow['trans_id'], uid))
                uid_money['not_financial_frozen'] -= amount
                uid_money['amounts'].append(-1*amount)
                
            else:
                unknown_status='未知状态,operation=%d,bus_type=%d' % (operation, bus_type)
                unknown_statuses.append(unknown_status)
                print(unknown_status)

        #O_FREEZE_TO_FREEZE_IN
        elif 9==operation:
            #(9, [3, 9, 10, 16, 18, 19, 23, 30, 46])

            #B_REPAYMENT(3, "借款还款"),投资人回款本金
            if 3==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资人回款本金'
                    uid_money['not_sures'].append(not_sure)
                    continue
                    
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue

                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            #B_COMMON(9, "付款"),服务费保证金其它费,平台|垫付方,放款给借款人，借款人向平台，垫付方缴费
            elif 9==bus_type:
                print('告警,平台账号,trans_id=%d,uid=%d' % (account_flow['trans_id'], uid))
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
            
            #B_COMPENSATORY(10, "垫付"),投资人收到垫付方垫付的本金
            elif 10==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资人收到垫付方垫付的本金'
                    uid_money['not_sures'].append(not_sure)
                    continue
                    
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue

                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            #B_FREEZE_TO_FREEZE(16, "冻结到冻结"),[todo:不确定]
            elif 16==bus_type:
                #理财计划资金转到中间户
                print('告警，不确定的状态, trans_id=%d, uid=%d' % (account_flow['trans_id'], uid))
                continue
                
            #LOAN(18, "借款放款"),借款人收到投资人放款
            #理财计划退出债转走的这个，所有要过滤
            elif 18==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='借款人收到投资人放款'
                    uid_money['not_sures'].append(not_sure)
                    continue
                    
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            #REPAYMENT_INTEREST(19, "还款利息"),投资人收到回款利息
            elif 19==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资人收到回款利息'
                    uid_money['not_sures'].append(not_sure)
                    continue
                    
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标，跳过, loan_id=%d' % loan_id)
                    continue
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
            
            #LOAN_FEE(23, "放款费用"),平台|垫付方，收取费用,[投资人-不应出现]
            elif 23==bus_type:
                print('告警，投资人不应该是平台|垫付方,uid=%d' % uid)
                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            #COMPENSATORY_INTEREST(30, "代偿利息"),投资人收到代偿利息
            elif 30==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资人收到代偿利息'
                    uid_money['not_sures'].append(not_sure)
                    continue
                    
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue

                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)

            #COMPENSATORY_FEE(46, "逾期费率"),平台|垫付方接受借款人缴纳的逾期费用
            elif 46==bus_type:
                if 0==loan_id:
                    #记录trans_id,uid，人工确认
                    not_sure={}
                    not_sure['flow_id']=account_flow['flow_id']
                    not_sure['trans_id']=account_flow['trans_id']
                    not_sure['uid']=account_flow['uid']
                    not_sure['desc']='投资人收到预期费用'
                    uid_money['not_sures'].append(not_sure)
                    continue
                    
                #定期理财散标
                asset_pool=get_asset_pool(loan_id)
                if 2==asset_pool or 4==asset_pool:
                    print('理财计划散标,跳过,loan_id=%d' % loan_id)
                    continue

                uid_money['not_financial_frozen'] += amount
                uid_money['amounts'].append(amount)
                
            else:
                unknown_status='未知状态,operation=%d,bus_type=%d' % (operation, bus_type)
                unknown_statuses.append(unknown_status)
                print(unknown_status)

    uid_money['financial_frozen']=uid_money['final_frozen_17'] - uid_money['not_financial_frozen']
#
def check_money():
    check_uids=get_uids()
    for uid in check_uids:
        to_17_create_index, to_now_create_index=get_create_index(uid)
        print('to_17_create_index=', to_17_create_index)
        print('to_now_create_index=', to_now_create_index)
        
        if -1==to_now_create_index:
            to_now_not_create_fuyou_uids.append(uid)
            
        if -1==to_17_create_index:
#             print('获取create_index错误, uid=%d' % uid)
#             print('未开户, uid=%d' % uid)
            to_17_not_create_fuyou_uids.append(uid)
            continue
        check_account_flow(uid, to_17_create_index)
        
    print('\n--------------------------------------------------------------------------------')
    print('调试信息，uid_moneys=')
#    print(str(uid_moneys))
    for k,v in uid_moneys.iteritems():
#         total_amount=0
#         for amount in  uid_moneys[k]['amounts']:
#             total_amount+=amount
#         print(total_amount)
        print(k,v)
    print('--------------------------------------------------------------------------------')
    print('\n')
    
    financial_money_content=''
    if to_now_not_create_fuyou_uids:
        content='到现在没有开通富友户的uid列表\n'
        financial_money_content += content
        for not_fuyou_uid in to_now_not_create_fuyou_uids:
            content='%d\n' % not_fuyou_uid
            financial_money_content += content
        financial_money_content += '\n\n------------------------------------------------------\n\n'
            
    
    if to_17_not_create_fuyou_uids:
        content='2016-11-4到2016-11-17 20:00:00未开通富友户的uid列表\n'
        financial_money_content += content
        for not_fuyou_uid in to_17_not_create_fuyou_uids:
            content='%d\n' % not_fuyou_uid
            financial_money_content += content
        financial_money_content += '\n\n------------------------------------------------------\n\n'
    
    for k,v in uid_moneys.iteritems():
        content='账号id,理财计划冻结金额,17日冻结总金额,非理财计划冻结金额,初始冻结金额,最新冻结金额,最新冻结金额 - 理财计划冻结金额\n'
        financial_money_content += content
        
        content='%d,%d,%d,%d,%d,%d,%d\n' %(v['uid'],
                                     v['financial_frozen'],
                                     v['final_frozen_17'],
                                     v['not_financial_frozen'],
                                     v['init_frozen'],
                                     v['frozen'],
                                     v['frozen']-v['financial_frozen'])
        financial_money_content += content

        if v['not_sures']:
            content="\n不确定的账号流水\n"
            financial_money_content += content

            content='        flow_id,trans_id,desc\n'
            financial_money_content += content

            for not_sure in v['not_sures']:
                content='        %d,%d,%s\n' %(not_sure['flow_id'], not_sure['trans_id'], not_sure['desc'])
                financial_money_content += content
        
        financial_money_content += '\n\n------------------------------------------------------\n\n'
        
    with open(os.path.join(os.path.abspath(os.curdir),'frozen_money.dat'), 'w') as f:
        f.write(financial_money_content)
    
    print('\n--------------------------------------------------------------------------------')
    print('未知operation,bus_type组合:')
    print(unknown_statuses)
    print('--------------------------------------------------------------------------------')
    print('\n')
    
#     test_chinese='测试中文'
#     print(test_chinese)

if '__main__'==__name__:
    start = datetime.now()
    print('start_time='+start.strftime(time_format))
    check_money()
    end=datetime.now()
    
    print('start_time='+start.strftime(time_format))
    print('end_time='+end.strftime(time_format))
    print('used_time='+str(end-start))