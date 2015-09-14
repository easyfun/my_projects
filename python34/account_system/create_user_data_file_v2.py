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
g_acc_chars = "abcdefghijklmnopqrstuvwxyz0123456789"
g_acc_chars_len = len(g_acc_chars)

g_acc_count_map = ((3,40000),(4,100000),(5,100000),
	(6,100000),(7,100000),(8,100000),(9,100000),
	(10,100000),(11,100000),(12,100000),(13,60000))
g_acc_count = 0
g_acc_bits = 3
g_acc_number = 0

#pwd有效字符
g_pwd_chars = "abcdefghijklmnopqrstuvwxyz0123456789"
g_pwd_chars_len = len(g_pwd_chars)

#phone有效字符
g_phone_chars = "0123456789"
g_phone_chars_len = len(g_phone_chars)

#email有效字符
g_email_chars = "abcdefghijklmnopqrstuvwxyz0123456789"
g_email_chars_len = len(g_email_chars)
g_email_suf = ("@qq.com", "@163.com", "@gmail.com")
g_email_suf_len = len(g_email_suf)

def Con10ToN(base, number, bits):
	result = []
	n = 0
	shan = number
	while n < bits:
		yu = shan % base
		shan //= base
		result.append(yu)
		n += 1
	result.reverse()
	return result

#生成账号
#36个有效字符
#3位 4w
#4位 10w
#5位 10w
#6位 10w
#7位 10w
#8位 10w
#9位 10w
#10位 10w
#11位 10w
#12位 10w
#13位 6w
def GetAccBits():
	global g_acc_count
	global g_acc_count_map
	global g_acc_number
	global g_acc_bits

	for acc_count in g_acc_count_map:
		print(acc_count)
		print(g_acc_bits)
		if g_acc_bits == acc_count[0]:
			if g_acc_count == acc_count[1]:
				g_acc_bits += 1
				g_acc_number = 0
				g_acc_count = 0
				return g_acc_bits
	g_acc_count += 1
	return g_acc_bits

def GenAcc():
	global g_acc_chars_len
	global g_acc_number
	
	bits = GetAccBits()
	indexs = Con10ToN(g_acc_chars_len, g_acc_number, bits)
	g_acc_number += 1
	acc = ""
	for bit in indexs:
		acc += g_acc_chars[bit]
	return acc + "y999f"

#生成密码
"""
36个有效字符
3位 4w
4位 10w
5位 10w
6位 10w
7位 10w
8位 10w
9位 10w
10位 10w
11位 10w
12位 10w
13位 6w
"""
def GenPwd(n, bits):
	indexs = Con10ToN(g_pwd_chars_len, n, bits)
	pwd = ""
	for bit in indexs:
		pwd += g_pwd_chars[bit]
	return pwd

#生成手机号
"""
10个有效字符
11位 100w
"""
def GenTelephone(n, bits):
	n += 999999999
	indexs = Con10ToN(g_phone_chars_len, n, bits)
	phone = ""
	for bit in indexs:
		phone += g_phone_chars[bit]
	return phone

#生成Email
#xxxxx, xxxxx.xxxxx, xxxxx.xxxxx.xxxxx
#@qq.com, @163.com, @gmail.com
"""
36个有效字符
3位 4w
4位 10w
5位 10w
6位 10w
7位 10w
8位 10w
9位 10w
10位 10w
11位 10w
12位 10w
13位 6w
"""
def GenEmail(n):
	email = ""
	indexs = Con10ToN(g_email_chars_len, n, 5)
	for bit in indexs:
		email += g_email_chars[bit]
	email += g_email_suf[n % g_email_suf_len]
	return email

def SaveUidData(des_file, count):
	n = 0
	uid = ""
	password = ""
	account = ""
	phone = ""
	email = ""
	with open(des_file, "w") as des_fobj:
		n = 999999999
		count += n
		while n < count:
			n += 1
			user_id = "%d"%(n)
			uid = "uid.%d"%(n)
			account = GenAcc(n, 6)
			password = GenPwd(n, 10)
			phone = GenTelephone(n, 11)
			email = GenEmail(n)
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
	save_file = os.path.join(cur_path, 'user_db')
	print(save_file)
	uid_sum = 10 #100 * 10000
#	SaveUidData(save_file, uid_sum)
	n = 0
	print(g_acc_bits)

	while n < 10:
		GetAccBits()
		acc = GenAcc()
		print(acc)