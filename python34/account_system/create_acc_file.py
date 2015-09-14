#!/usr/bin/env python3
# -*- coding:utf-8 -*-

#author:trxh
#qq:1060887552
#github:https://github.com/tianruoxuanhe
#license:LGPL

#import data into redis command: cat file|redis-cli –pipe
#redis protocol file: 
#		*<args><cr><lf>
#		$<len><cr><lf>
#		<arg0><cr><lf>
#		<arg1><cr><lf>
#		...
#example:
#		<argN><cr><lf>
#		*3<cr><lf>
#		$3<cr><lf>
#		SET<cr><lf>
#		$3<cr><lf>
#		key<cr><lf>
#		$5<cr><lf>
#		value<cr><lf>

#Generate 100*10000 records file losses a few of hours more than four hours.

import sys
import os
from os import path
import random

#acc有效字符
g_acc_chars = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789"
g_acc_chars_len = len(g_acc_chars)-1
g_acc_list = []

#5~15英文字母，数字
def GenAccount():
	length = random.randint(5,20)
	n = 0
	account = ""
	while 1:
		n = 0
		account = ""
		while n < length:
			n += 1
			i = random.randint(0,g_acc_chars_len)
			account += g_acc_chars[i]
		if not account in g_acc_list:
			break
	#print(account)
	g_acc_list.append(account)		
	return account

def SaveUidData(des_file, count):
	n = 0
	uid = ""
	account = ""
	with open(des_file, "w") as des_fobj:
		while n < count:
			n += 1
			GenAccount()
		
		content = ""
		n = 0
		for account in g_acc_list:
			content += account
			content += "\r\n"
			n += 1
			if 10 == n:
				des_fobj.write(content)
				content = ""
				n = 0
		if n > 0 and n != 10:
			des_fobj.write(content)

def Con10To62(number, bits):
	result = []
	n = 0
	shan = number
	while n < bits:
		yu = shan %62
		shan //= 62
		result.append(yu)
		n += 1
	result.reverse()
	return result

def GenAcc(n, bits):
	indexs = Con10To62(n, bits)
	acc = ""
	for bit in indexs:
		acc += g_acc_chars[bit]
	return acc + "y999f"

def SaveAccDB(des_file, count):
	n = 0
	acc = ""
	while n < count:
		acc = GenAcc(n, 6)
#		print(acc)
		n += 1
	print(n)

if __name__ == "__main__":
	cur_path = os.path.abspath('.')
	save_file = os.path.join(cur_path, 'acc_db')
	print(save_file)
	uid_sum = 100 * 10000
#	SaveUidData(save_file, uid_sum)
#	print(len(g_acc_list))
	SaveAccDB(save_file, uid_sum)