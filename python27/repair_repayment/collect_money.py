#- coding:utf-8 -
import os
import demjson
import MySQLdb
import os

#transId,uid,borrower,platform_uid,amount,diff_amount,trusteeBusType,subType

amounts={}
transids=[]

print('hello')
with open(os.path.abspath(os.curdir)+'/exit_money.log', 'r') as f:
	n=0
	for line in f.readlines():
		n+=1
		print(n)
		if n>4:
			pass
		#print(line)

		pos=line.find('805 - investInfos=')
		#print(pos)
		if pos>0:
			investInfos=line[pos:]
			#print(investInfos)
			pos=investInfos.find('=[')
			invest=investInfos[pos+2:-2]
			#print(invest)
			d_invest=eval(invest)
			#print(d_invest)
			#print(d_invest['transId'])
			record={'transId':d_invest['transId'],
			'uid':d_invest['uid'],
			'borrower':d_invest['borrower'],
			'amount':d_invest['amount'],
			'diff_amount':0
			}
			#print(record)
			if (record['transId'] in transids):
				print(record['transId'])
				print('transId duplicate')
				print('transId=',transids)
				exit()

			transids.append(record['transId'])
			amounts[record['transId']]=record
			#print(amounts)
			continue


		pos=line.find('806 - feeInfos=')
		#print(pos)
		if pos>0:
			feeInfos=line[pos:]
			#print(feeInfos)
			pos=feeInfos.find('=')
			fee=feeInfos[pos+1:-1]
			#print(fee)
			fees=demjson.decode(fee)
			#print(fees)
			for fee in fees:
				#if 20161109000460960L==fee['transid']:
				#	print(fee)

				#print('fee=',fee)
				if 'PLAN_EXIT_FEE' == fee['desc']:
					continue

				if 'PLAN_EXIT_DEDUCT_INTEREST' == fee['trusteeBusType']:
					record=amounts[fee['transid']]

					record['trusteeBusType']=fee['trusteeBusType']
					record['subType']=fee['subType']
					record['platform_uid']=fee['toUid']
					record['diff_amount']=-1*fee['amount']
	

				
				if  'PLAN_EXIT_ADD_INTEREST' == fee['trusteeBusType']:
					record=amounts[fee['transid']]
					record['trusteeBusType']=fee['trusteeBusType']
					record['subType']=fee['subType']
					record['platform_uid']=fee['formUid']
					record['diff_amount']=fee['amount']
			continue
				
		#print(record)
#print('amounts=',amounts)
with open(os.path.abspath(os.curdir)+'/stas.log', 'w') as w:
	for k in amounts:
		print(k,amounts[k])
		#transId,uid,borrower,platform_uid,amount,diff_amount,trusteeBusType,subType
		amount=amounts[k]

		if 0==amount['diff_amount']:
			line = "%d, %d, %d, %-10d, %-10d\n" %(
				amount['transId'],
				amount['uid'],
				amount['borrower'],
				amount['amount'],
				amount['diff_amount'])
		else:		
			line = "%d, %d, %d, %d, %-10d, %-10d, %-26s, %d\n" %(
				amount['transId'],
				amount['uid'],
				amount['borrower'],
				amount['platform_uid'],
				amount['amount'],
				amount['diff_amount'],
				amount['trusteeBusType'],
				amount['subType'])
		print(line)
		w.write(line)

		#w.write(str(amounts))


	