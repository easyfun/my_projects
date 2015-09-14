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

#phone有效字符
g_phone_chars = "0123456789"
g_phone_chars_len = len(g_phone_chars)-1
g_phone_list = []

#11位纯数字串
def GenPhone():
	length = 11
	n = 0
	phone =""
	while 1:
		n = 0
		phone = ""
		while n < length:
			n += 1
			i = random.randint(0, g_phone_chars_len)
			phone += g_phone_chars[i]

		if not phone in g_phone_list:
			break
	#print(phone)
	g_phone_list.append(phone)
	return phone

def SaveUidData(des_file, count):
	n = 0
	phone = ""
	with open(des_file, "w") as des_fobj:
		while n < count:
			n += 1
			GenPhone()
		
		content = ""
		n = 0
		for phone in g_phone_list:
			content += phone
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
	save_file = os.path.join(cur_path, 'phone_db')
	print(save_file)
	uid_sum = 100 * 10000
	SaveUidData(save_file, uid_sum)
	print(len(g_phone_list))
