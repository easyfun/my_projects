#- coding:utf-8 -

'''
理财计划下的散标的还款表修复
repair_repayment.py
java程序repairMissionLoanRepaymets
check_repay_schedule.py
'''

import os
import demjson
import MySQLdb

loans=[]

loan_conn = MySQLdb.connect('192.168.0.220','root', '123456', 'toulf')
loan_cursor = loan_conn.cursor(MySQLdb.cursors.DictCursor)

user_conn = MySQLdb.connect('192.168.0.206', 'root', '123456', 'user')
user_cursor = user_conn.cursor(MySQLdb.cursors.DictCursor)

not_find_borrower_uid=[]

sql="select id,status,borrowamount,annualrate,add_rate,periods,periodmode,repaymode,borrower from loans where status in (6)"
#print("sql", sql)
loan_cursor.execute(sql)
loan_results=loan_cursor.fetchall()

for loan_result in loan_results:
	print('loan_result=',loan_result)
	loan_id=loan_result['id']
	amount=loan_result['borrowamount']
	annual_rate=loan_result['annualrate']
	periods=loan_result['periods']
	period_mode=loan_result['periodmode']
	repay_mode=loan_result['repaymode']
	borrower_id=loan_result['borrower']

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
	loan.append(period_mode)
	new_id=''
	loan.append(new_id)
	old_borrower_uid=borrower_id
	loan.append(old_borrower_uid)
	loan.append(new_borrower_uid)
	loan.append(repay_mode)
	print(loan)
	loans.append(loan)

print(not_find_borrower_uid)
with open(os.path.join(os.path.abspath(os.curdir),'loans.data'), 'w') as f:
	f.write('普通散标:旧标id,借款本金,年化利率,借款期数,借款类型(0月 1天),新标id,旧借款人uid,新借款人uid,还款方式\n')
	for loan in loans:
		#f.write('%s,%s,%s,%s,%s,%s,%s' % (loan[0], loan[1], loan[2], loan[3]) + '\n')
		line=''
		for r in loan:
			line+=str(r)+','
		line+='\n'
		f.write(line)
