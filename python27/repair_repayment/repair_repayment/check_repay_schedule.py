#-*- coding:utf-8 -*-

'''
理财计划下的散标的还款表修复
'''

import os
import demjson
import MySQLdb

loans=[]

error_results=[]
loan_ids=[]
repay_ids=[]
fix_repayment_sqls=[]	#sql脚本
dispatch_money_list=[]	#还款表派钱

readonly_conn = MySQLdb.connect('192.168.0.220','root', '123456', 'toulf')
readonly_cursor = readonly_conn.cursor(MySQLdb.cursors.DictCursor)

asset_id_conn = MySQLdb.connect('192.168.2.80','root', '123456', 'specialDB')
asset_id_cursor = asset_id_conn.cursor(MySQLdb.cursors.DictCursor)

with open(os.path.join(os.path.abspath(os.curdir),'mission_repay_schedule.data'), 'r') as f:
	n=0
	for line in f.readlines():
		n+=1
		# if n>2:
		# 	break
		line=line.strip()
		#print(line)
		repayments=line.split(',')
		loan_id=repayments[0]
		period_no=int(repayments[1])
		capital=int(repayments[2])
		interest=int(repayments[3])
		new_asset_id=repayments[4]
		old_borrower_uid=repayments[5]
		new_borrower_uid=repayments[6]

		#标id，还款状态，还款期号，预期本金，预期利息，标标题，还款id，还款方式
		sql="select r.loan_id,r.status,r.period,r.still_principal,r.still_interest,l.title,r.id,r.repay_mode from mission_repayment r,mission_loans l where r.loan_id='%s' and r.period='%s' and l.id='%s'" % (loan_id, period_no, loan_id)
		readonly_cursor.execute(sql)
		results=readonly_cursor.fetchall()
		#print(results)
		for result in results:
			#print(result)
			loan_id=result['loan_id']
			status=result['status']
			period=result['period']
			still_principal=result['still_principal']
			still_interest=result['still_interest']
			repay_id=result['id']
			repay_mode=result['repay_mode']

			if 3==status:
				print('已还款')
				continue

			if period_no != period:
				print('还款期号错误')
				#exit()
				continue

			if 100*still_principal != capital:
				print('本金错误，严重错误')
				exit()

				'''
				error_results.append(result)
				if loan_id not in loan_ids:
					loan_ids.append(loan_id)
				if repay_id not in repay_ids:
					repay_ids.append(repay_id)
				'''

			old_still_interest=int(100*still_interest)
			diff_interest=interest - old_still_interest

			if diff_interest == 1:
				print('利息错误')
				#exit()
				print('still_interest=', still_interest, 'interest=', interest, 'diff=', interest - 100*still_interest)

				'''
				new_result=list(result)
				#新利息
				new_result.append(interest)
				#差额
				new_result.append(str(interest - 100*still_interest))
				'''

				result['new_interest']=interest
				result['diff_interest']=diff_interest
				error_results.append(result)

				if loan_id not in loan_ids:
					loan_ids.append(loan_id)

				if repay_id not in repay_ids:
					repay_ids.append(repay_id)

				fix_repayment_sql='mysql product -e "update product.t_repayments set expect_interest=expect_interest+1 where asset_id=%s and borrower_uid=%s and phase=%d and state=0;"' %(new_asset_id, new_borrower_uid, period)
				fix_repayment_sqls.append(fix_repayment_sql)
				#分表借款人uid后两位
				fix_repayment_sql='mysql product -e "update product.t_repayment_%02d set expect_interest=expect_interest+1 where asset_id=%s and borrower_uid=%s and phase=%d and state=0;"' %(long(new_borrower_uid)%100, new_asset_id, new_borrower_uid, period)
				fix_repayment_sqls.append(fix_repayment_sql)

				dispatch_money=[]
				dispatch_money.append(new_asset_id)
				dispatch_money.append(new_borrower_uid)
				dispatch_money.append(repay_id)
				dispatch_money.append(period)
				dispatch_money.append(diff_interest)
				dispatch_money_list.append(dispatch_money)


			if diff_interest > 1 or diff_interest < 0:
				print('利息严重错误,diff_interest=',diff_interest)
				print('interest=',interest,'old_still_interest=',old_still_interest)
				exit()


with open(os.path.join(os.path.abspath(os.curdir),'diff_repayment.data'), 'w') as dr:
	dr.write('旧标id,借款本金,年化利率,借款天数,新标id,旧借款人uid,新借款人uid,新利息,差额\n')
	for result in error_results:
		line=str(result)
		'''
		for r in result:
			line+=str(r)+','
		'''
		line+='\n'
		dr.write(line)

with open(os.path.join(os.path.abspath(os.curdir),'diff_repay_id.data'), 'w') as dri:
	for repay_id in repay_ids:
		line = '%s\n' % repay_id
		dri.write(line)

with open(os.path.join(os.path.abspath(os.curdir),'fix_financial_asset_repayment.sql'), 'w') as ffar:
	for fix_repayment_sql in fix_repayment_sqls:
		line = '%s\n' % fix_repayment_sql
		ffar.write(line)


with open(os.path.join(os.path.abspath(os.curdir),'dispatch_money.data'), 'w') as dm:
	dm.write('标id,借款人uid,还款id,还款期号,派送差额\n')
	for dispatch_money in dispatch_money_list:
		line=''
		for r in dispatch_money:
			line+=str(r)+','
		line+='\n'

		dm.write(line)
