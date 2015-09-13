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

#email有效字符
g_email_chars = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789"
g_email_chars_len = len(g_email_chars)-1
g_email_suf = ("@qq.com", "@163.com", "@gmail.com")
g_email_suf_len = len(g_email_suf)-1
g_email_list = []

#xxxxx, xxxxx.xxxxx, xxxxx.xxxxx.xxxxx
#@qq.com, @163.com, @gmail.com
def GenEmail():
	seg = 0	
	length = 0
	email = ""
	m = 0
	n = 0
	while 1:
		email = ""
		seg = random.randint(1, 3)
		m = 0
		while m < seg:
			m += 1 
			length = random.randint(3,5)
			n = 0
			while n < length:
				n += 1
				i = random.randint(0,g_email_chars_len)
				email += g_email_chars[i]
			if m != seg:
				email += "."	
		
		i = random.randint(0, g_email_suf_len)
		email += g_email_suf[i]
		if not email in g_email_list:
			break
	#print(email)
	g_email_list.append(email)
	return email		

def SaveUidData(des_file, count):
	n = 0
	email = ""
	with open(des_file, "w") as des_fobj:
		while n < count:
			n += 1
			GenEmail()
		
		content = ""
		n = 0
		for email in g_email_list:
			content += email
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
	save_file = os.path.join(cur_path, 'email_db')
	print(save_file)
	uid_sum = 100 * 10000
	SaveUidData(save_file, uid_sum)
	print(len(g_email_list))
