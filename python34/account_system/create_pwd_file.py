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

#pwd有效字符
g_pwd_chars = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789"
g_pwd_chars_len = len(g_pwd_chars)-1
g_pwd_list = []

#5~15英文字母，数字
def GenPassword():
	length = random.randint(5,20)
	n = 0
	password = ""
	while 1:
		n = 0
		password = ""
		while n < length:
			n += 1
			i = random.randint(0,g_pwd_chars_len)
			password += g_pwd_chars[i]
		if not password in g_pwd_list:
			break
	#print(password)
	g_pwd_list.append(password)		
	return password

def SaveUidData(des_file, count):
	pwd = ""
	n = 0
	with open(des_file, "w") as des_fobj:
		n = 999999999
		count += n
		while n < count:
			GenPassword()
			n += 1

		content = ""
		n = 0
		for pwd in g_pwd_list:
			content += pwd
			content += "\r\n"
			n += 1
			if 10 == n:
				des_fobj.write(content)
				content = ""
				n = 0
		if n > 0 and n != 10:
			des_fobj.write(content)

if __name__ == "__main__":
	cur_path = os.path.abspath('.')
	save_file = os.path.join(cur_path, 'pwd_db')
	print(save_file)
	uid_sum = 100 * 10000
	SaveUidData(save_file, uid_sum)
	print(len(g_pwd_list))