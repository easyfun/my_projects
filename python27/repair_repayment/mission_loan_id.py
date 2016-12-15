#- coding:utf-8 -
import MySQLdb
import os

#理财计划，理财计划散标id
'''
create table asset_id_map (
	new_id		bigint not null comment '新ID',
	old_id		varchar(64) not null default '' comment '旧id',
	old_title		varchar(200) not null default '' comment '旧标题',
	type	tinyint not null default 0 comment '标类型:0-未知，1-理财计划，2-理财计划散标',
	primary key (new_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
'''
def do_asset_id():
	readonly_conn = MySQLdb.connect('192.168.50.180','search', 'search@zyxr.com', 'toulf')
	readonly_cursor = readonly_conn.cursor()

	write_conn=MySQLdb.connect('192.168.0.204','root', '123456', 'toulf')
	write_cursor=write_conn.cursor()

	with open(os.path.abspath(os.curdir)+'/mapAssetId-2016-11-04-03-18-59.txt', 'r') as f:
		n=0
		for line in f.readlines():
			n+=1
			# if n>2:
			# 	break
			line=line.strip()
			print(line)
			ids=line.split(':')
			if len(ids[0])==22:
				read_sql='select id,title from mission_loans where id=%s' % ids[0]
				print(read_sql)
				readonly_cursor.execute(read_sql)
				results=readonly_cursor.fetchall()
				print(results)
				for result in results:
					id=result[0]
					title=result[1]
					print(id,title,ids[1])

					write_sql="insert into asset_id_map(new_id,old_id,old_title,type) values (%s,%s,'%s',%d)" %(ids[1], id, title, 2)
					print('write_sql:', write_sql)
					try:
						write_cursor.execute(write_sql)
						#write_conn.commit()
					except Exception as e:
						write_conn.rollback()
						print('保存数据错误')
						exit()

			
			if len(ids[0])==20:
				read_sql='select id,title from missions where id=%s' % ids[0]
				print(read_sql)
				readonly_cursor.execute(read_sql)
				results=readonly_cursor.fetchall()
				print(results)
				for result in results:
					id=result[0]
					title=result[1]
					print(id,title,ids[1])

					write_sql="insert into asset_id_map(new_id,old_id,old_title,type) values (%s,%s,'%s',%d)" %(ids[1], id, title, 1)
					print('write_sql:', write_sql)
					try:
						write_cursor.execute(write_sql)
						#write_conn.commit()
					except Exception as e:
						write_conn.rollback()
						print('保存数据错误')
						exit()

		write_conn.commit()
		print('n=',n)
#do_asset_id()

def check_asset_id():
	'''
	readonly_conn = MySQLdb.connect('192.168.50.180','search', 'search@zyxr.com', 'toulf')
	readonly_cursor = readonly_conn.cursor()
	'''

	read_conn=MySQLdb.connect('192.168.2.80','root', '123456', 'specialDB')
	read_cursor=read_conn.cursor()

	with open(os.path.abspath(os.curdir)+'/mapAssetId-2016-11-04-03-18-59.txt', 'r') as f:
		n=0
		for line in f.readlines():
			n+=1
			# if n>2:
			# 	break
			line=line.strip()
			print(line)
			ids=line.split(':')

			sql='select new_id,old_id from asset_id_map where old_id=%s' % ids[0]
			read_cursor.execute(sql)
			results=read_cursor.fetchall()
			for result in results:
				new_id=str(result[0])
				old_id=result[1]
				if new_id!=ids[1] or old_id!=ids[0]:
					print('数据错误')
					print('results', results)
					print('ids',line)
					exit()
check_asset_id()

#理财计划投资记录id
'''
create table invest_id_map (
	new_investment_id		bigint not null comment '新投资记录ID',
	old_investor_uid	varchar(40) NOT NULL comment '旧投资人uid',
	old_investment_id		bigint not null comment '旧投资记录ID',
	old_loan_id	varchar(64) not null default '' comment '旧标id',
	old_loan_title		varchar(64) not null default '' comment '旧标title',
	old_borrower_uid	varchar(40) NOT NULL comment '旧借款人uid',
	old_mission_id	varchar(64) not null default '' comment '旧标id',
	old_mission_title	varchar(64) not null default '' comment '旧标id',
	type		tinyint not null default 0 comment '标类型:0-未知，1-投理财计划，2-投理财计划散标',
	primary key (new_investment_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
'''

def do_invest_id():
	readonly_conn = MySQLdb.connect('192.168.50.180','search', 'search@zyxr.com', 'toulf')
	readonly_cursor = readonly_conn.cursor()

	write_conn=MySQLdb.connect('192.168.0.204','root', '123456', 'toulf')
	write_cursor=write_conn.cursor()

	with open(os.path.abspath(os.curdir)+'/mapInvestId-2016-11-04-03-18-59.txt', 'r') as f:
		n=0
		test_a=0
		test_b=0
		for line in f.readlines():
			n+=1
			# if n>2:
			# 	break
			line=line.strip()
			#print(line)
			ids=line.split(':')
			#print(ids[0],ids[1])
			old_id=ids[0]
			new_id=ids[1].replace('"', '')
			#print(new_id, type(new_id))
			'''
			if new_id[0]=='3':
				print('小于') 
			if new_id[0]=='4':
				print('大于')
			continue
			'''

			if new_id[0]=='3':
				read_sql='select investor_id,loan_id,loan_title,borrower_id,mission_id,mission_title from mission_invest where id=%s' % old_id
				#print(read_sql)
				readonly_cursor.execute(read_sql)
				results=readonly_cursor.fetchall()
				#print(results)
				for result in results:
					test_a+=1
					#if test_a>2:
					#	break
					new_investment_id=new_id
					old_investor_uid=result[0]
					old_investment_id=old_id
					old_loan_id=result[1]
					old_loan_title=result[2]
					old_borrower_uid=result[3]
					old_mission_id=result[4]
					old_mission_title=result[5]
					invest_type=2
					write_sql="insert into invest_id_map(new_investment_id,old_investor_uid,old_investment_id,old_loan_id,old_loan_title,old_borrower_uid,old_mission_id,old_mission_title,type) values ('%s','%s','%s','%s','%s','%s','%s','%s',%d)" %(new_investment_id,old_investor_uid,old_investment_id,old_loan_id,old_loan_title,old_borrower_uid,old_mission_id,old_mission_title,invest_type)
					#print('write_sql:', write_sql)
					try:
						write_cursor.execute(write_sql)
						#write_conn.commit()
					except Exception as e:
						write_conn.rollback()
						print('保存数据错误')
						print(write_sql)
						exit()
					#write_conn.commit()

			
			if new_id[0]=='4':
				read_sql='select uid,mission_id,mission_title from mission_joins where id=%s' % old_id
				#print(read_sql)
				readonly_cursor.execute(read_sql)
				results=readonly_cursor.fetchall()
				print('results=', results, len(results))
				for result in results:
					test_b+=1
					#if test_b>2:
					#	break

					new_investment_id=new_id
					old_investor_uid=result[0]
					old_investment_id=old_id
					old_loan_id=''
					old_loan_title=''
					old_borrower_uid=''
					old_mission_id=result[1]
					old_mission_title=result[2]
					invest_type=1
					write_sql="insert into invest_id_map(new_investment_id,old_investor_uid,old_investment_id,old_loan_id,old_loan_title,old_borrower_uid,old_mission_id,old_mission_title,type) values ('%s','%s','%s','%s','%s','%s','%s','%s',%d)" %(new_investment_id,old_investor_uid,old_investment_id,old_loan_id,old_loan_title,old_borrower_uid,old_mission_id,old_mission_title,invest_type)
					print('write_sql:', write_sql)
					try:
						write_cursor.execute(write_sql)
						#write_conn.commit()
					except Exception as e:
						write_conn.rollback()
						print('保存数据错误2')
						print('write_sql:', write_sql)
						exit()
					#write_conn.commit()

		write_conn.commit()
		print('n=',n)
		print('test_a=', test_a)
		print('test_b=', test_b)

#do_invest_id()


def check_invest_id():
	'''
	readonly_conn = MySQLdb.connect('192.168.50.180','search', 'search@zyxr.com', 'toulf')
	readonly_cursor = readonly_conn.cursor()
	'''

	read_conn=MySQLdb.connect('192.168.2.80','root', '123456', 'specialDB')
	read_cursor=read_conn.cursor()

	with open(os.path.abspath(os.curdir)+'/mapInvestId-2016-11-04-03-18-59.txt', 'r') as f:
		n=0
		test_a=0
		test_b=0
		for line in f.readlines():
			n+=1
			print(n)
			# if n>2:
			# 	break
			line=line.strip()
			#print(line)
			ids=line.split(':')
			#print(ids[0],ids[1])
			old_id=ids[0]
			new_id=ids[1].replace('"', '')

			sql='select new_investment_id,old_investment_id from invest_id_map where old_investment_id=%s' % old_id
			read_cursor.execute(sql)
			results=read_cursor.fetchall()
			for result in results:
				new_investment_id=str(result[0])
				old_investment_id=str(result[1])
				if new_id!=new_investment_id or old_id!=old_investment_id:
					print('数据错误')
					print('results', results)
					print('ids',line)
					exit()

		print('n=',n)

check_invest_id()