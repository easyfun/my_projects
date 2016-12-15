#!/bin/bash
#################################################################
# 库列表：
# loan: 	核心业务库,负责标的表数据
# invest：	投资库，负责投资相关表数据
# repay: 	还款库，负责还款相关表数据
#################################################################

password="search@zyxr.com"
m=0
n=0
l=0
mysql_exec="mysql -usearch -p$password"
#mysql_exec="mysql "

db=product
for i in {0..99}
do	
	m=$(($i/10))
	n=$(($i%10))
	#标的流水表
	$mysql_exec $db -e "select * outfile './xls03.xls' from t_investment_${m}${n}"
end