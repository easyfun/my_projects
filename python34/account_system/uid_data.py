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

#pwd有效字符
g_pwd_chars = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789"
g_pwd_chars_len = len(g_pwd_chars)-1
g_pwd_list = []

#phone有效字符
g_phone_chars = "0123456789"
g_phone_chars_len = len(g_phone_chars)-1
g_phone_list = []

#email有效字符
g_email_chars = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789"
g_email_chars_len = len(g_email_chars)-1
g_email_suf = ("@qq.com", "@163.com", "@gmail.com")
g_email_suf_len = len(g_email_suf)-1
g_email_list = []

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
	uid = ""
	account = ""
	phone = ""
	email = ""
	password = ""
	with open(des_file, "w") as des_fobj:
		n = 8888888
		count += n
		while n < count:
			user_id = "%d"%(n)
			uid = "%s.%d"%("uid",n)
			n += 1
			account = GenAccount()
			password = GenPassword()
			phone = GenPhone()
			email = GenEmail()
			user_info = "*10\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n"%(
				len("hmset"),"hmset",len(uid),uid,
				len("acc"),"acc",len(account),account,
				len("pwd"),"pwd",len(password),password,
				len("phone"),"phone",len(phone),phone,
				len("email"),"email",len(email),email)
			#print(user_info)
			des_fobj.write(user_info)
			acc_uid = "*4\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n"%(
				len("hmset"),"hmset",
				len("acc.uid"),"acc.uid",
				len(account),account,
				len(user_id),user_id)
			des_fobj.write(acc_uid)
			phone_uid = "*4\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n"%(
				len("hmset"),"hmset",
				len("phone.uid"),"phone.uid",
				len(phone),phone,
				len(user_id),user_id)
			des_fobj.write(phone_uid)
			email_uid = "*4\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n"%(
				len("hmset"),"hmset",
				len("email.uid"),"email.uid",
				len(email),email,
				len(user_id),user_id)
			des_fobj.write(email_uid)
#hmset key field1 value1 field2 value2
#hmset uid.id acc acc_v phone pwd pwd_v phone_v email email_v
#hmset acc.uid acc_v uid_v
#hmset phone.uid phone_v uid_v
#hmset email.uid email_v uid_v

if __name__ == "__main__":
	cur_path = os.path.abspath('.')
	save_file = os.path.join(cur_path, 'uid_db')
	print(save_file)
	uid_sum = 10 * 10000
	SaveUidData(save_file, uid_sum)
	print(len(g_acc_list))
	print(len(g_phone_list))
	print(len(g_email_list))
	print(len(g_pwd_list))