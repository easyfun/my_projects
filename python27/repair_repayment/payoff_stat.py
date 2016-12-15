#- coding:utf-8 -

import MySQLdb
import os

#获取2016-11-04到2016-11-10 所有计划散标id
read_conn = MySQLdb.connect('192.168.5.151','serach', '123456', 'invest')
read_cursor = read_conn.cursor(MySQLdb.cursors.DictCursor)

m=0
n=0
with open(os.path.abspath(os.curdir)+'/test_write.txt', 'w') as f:
	sql='select * from t_investment_%d%d limit 1' % (m, n)
	read_cursor.execute(sql)
	results=read_cursor.fetchall()
	for result in results:
		f.write(str(result))
