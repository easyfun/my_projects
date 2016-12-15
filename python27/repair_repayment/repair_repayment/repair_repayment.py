#- coding:utf-8 -

'''
理财计划下的散标的还款表修复
repair_repayment.py
java程序repairMissionLoanRepaymets
check_repay_schedule.py

192.168.51.145
search search@zyxr.com

'''

import os
import demjson
import MySQLdb

loans=[]

mission_loan_fee_conn = MySQLdb.connect('192.168.0.220','root', '123456', 'toulf')
mission_loan_fee_cursor = mission_loan_fee_conn.cursor(MySQLdb.cursors.DictCursor)


asset_id_conn = MySQLdb.connect('192.168.2.80','root', '123456', 'specialDB')
asset_id_cursor = asset_id_conn.cursor(MySQLdb.cursors.DictCursor)

user_conn = MySQLdb.connect('192.168.51.145', 'search', 'search@zyxr.com', 'user')
user_cursor = user_conn.cursor(MySQLdb.cursors.DictCursor)

not_find_borrower_uid=[]

sql="select new_id, old_id, type from asset_id_map where type=2"
asset_id_cursor.execute(sql)
results=asset_id_cursor.fetchall()
for result in results:
	print('result=', result)
	'''
	new_id=result[0]
	old_id=result[1]
	asset_type=result[2]
	'''
	new_id=result['new_id']
	old_id=result['old_id']
	asset_type=result['type']

	sql="select amount,annual_rate,periods,borrower_id from mission_loans where id='%s'" % old_id
	#print("sql", sql)
	mission_loan_fee_cursor.execute(sql)
	loan_results=mission_loan_fee_cursor.fetchall()

	for loan_result in loan_results:
		print('loan_result=',loan_result)
		loan_id=old_id
		amount=loan_result['amount']
		annual_rate=loan_result['annual_rate']
		periods=loan_result['periods']
		borrower_id=loan_result['borrower_id']

		user_sql="select userid from t_userid_rel_olduid where ybuid='%s'" % borrower_id
		user_cursor.execute(user_sql)
		user_results=user_cursor.fetchall()
		new_borrower_uid=''
		for user_result in user_results:
			new_borrower_uid=user_result['userid']
		if ''==new_borrower_uid:
			not_find_borrower_uid.append(borrower_id)
			print('借款人uid不存在')
			continue

		loan=[]
		loan.append(loan_id)
		loan.append(amount)
		loan.append(annual_rate)
		loan.append(periods)
		loan.append(new_id)
		old_borrower_uid=borrower_id
		loan.append(old_borrower_uid)
		loan.append(new_borrower_uid)
		print(loan)
		loans.append(loan)

print(not_find_borrower_uid)
if len(not_find_borrower_uid) > 0:
	print('借款人错误')
	exit()
	
with open(os.path.join(os.path.abspath(os.curdir),'mission_loan.data'), 'w') as f:
	f.write('旧标id,借款本金,年化利率,借款天数,新标id,旧借款人uid,新借款人uid\n')
	for loan in loans:
		#f.write('%s,%s,%s,%s,%s,%s,%s' % (loan[0], loan[1], loan[2], loan[3]) + '\n')
		line=''
		for r in loan:
			line+=str(r)+','
		line+='\n'
		f.write(line)
