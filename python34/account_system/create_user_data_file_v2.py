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

#Generate 100*10000 records file losses a few of minus.

import sys
import os
from os import path
import random

#acc有效字符
g_acc_chars = "abcdefghijklmnopqrstuvwxyz0123456789"
g_acc_chars_len = len(g_acc_chars)

#pwd有效字符
g_pwd_chars = "abcdefghijklmnopqrstuvwxyz0123456789"
g_pwd_chars_len = len(g_pwd_chars)

#phone有效字符
g_phone_chars = "0123456789"
g_phone_chars_len = len(g_phone_chars)

#email有效字符
g_email_chars = "zyxwvutsrqponmlkjihgfedcba"
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
g_acc_count_map = ((3,40000),(4,100000),(5,100000),
	(6,100000),(7,100000),(8,100000),(9,100000),
	(10,100000),(11,100000),(12,100000),(13,60000))
g_acc_count = 0
g_acc_bits = 3
g_acc_number = 0

def GetAccBits():
	global g_acc_count
	global g_acc_count_map
	global g_acc_number
	global g_acc_bits

	for acc_count in g_acc_count_map:
		if g_acc_bits == acc_count[0]:
			if g_acc_count == acc_count[1]:
				g_acc_bits += 1
				g_acc_number = 0
				g_acc_count = 1
				return g_acc_bits
	g_acc_count += 1
	return g_acc_bits

def GenAcc():
	global g_acc_chars_len
	global g_acc_number
	
	bits = GetAccBits()
	#print("%d   %d"%(bits, g_acc_number))
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
g_pwd_count_map = ((3,40000),(4,100000),(5,100000),
	(6,100000),(7,100000),(8,100000),(9,100000),
	(10,100000),(11,100000),(12,100000),(13,60000))
g_pwd_count = 0
g_pwd_bits = 3
g_pwd_number = 0

def GetPwdBits():
	global g_pwd_count
	global g_pwd_count_map
	global g_pwd_number
	global g_pwd_bits

	for pwd_count in g_pwd_count_map:
		if g_pwd_bits == pwd_count[0]:
			if g_pwd_count == pwd_count[1]/1000:
				g_pwd_bits += 1
				g_pwd_number = 0
				g_pwd_count = 1
				return g_pwd_bits
	g_pwd_count += 1
	return g_pwd_bits

def GenPwd():
	global g_pwd_chars_len
	global g_pwd_number
	
	bits = GetPwdBits()
	#print("%d   %d"%(bits, g_pwd_number))
	indexs = Con10ToN(g_pwd_chars_len, g_pwd_number, bits)
	g_pwd_number += 1
	pwd = ""
	for bit in indexs:
		pwd += g_pwd_chars[bit]
	return pwd


#生成手机号
"""
10个有效字符
11位 100w
"""
g_tele_phone_num = 19890813000
g_tele_phone_bits = 11
def GenTelephone():
	global g_tele_phone_bits
	global g_tele_phone_num
	global g_tele_phone_bits

	indexs = Con10ToN(g_phone_chars_len, g_tele_phone_num, g_tele_phone_bits)
	g_tele_phone_num += 1
	phone = ""
	for bit in indexs:
		phone += g_phone_chars[bit]
	return phone

#生成Email
#xxxxx, xxxxx.xxxxx, xxxxx.xxxxx.xxxxx
#@qq.com, @163.com, @gmail.com
"""
26个有效字符
4位 10w
5位 10w
6位 10w
7位 10w
8位 10w
9位 10w
10位 10w
11位 10w
12位 10w
13位 10w
"""
g_email_count_map = ((4,100000),(5,100000),
	(6,100000),(7,100000),(8,100000),(9,100000),
	(10,100000),(11,100000),(12,100000),(13,100000))
g_email_count = 0
g_email_bits = 4
g_email_number = 0

def GetEmailBits():
	global g_email_count
	global g_email_count_map
	global g_email_number
	global g_email_bits

	for email_count in g_email_count_map:
		if g_email_bits == email_count[0]:
			if g_email_count == email_count[1]/10000:
				g_email_bits += 1
				g_email_number = 0
				g_email_count = 1
				return g_email_bits
	g_email_count += 1
	return g_email_bits

def GenEmail():
	global g_email_chars_len
	global g_email_number
	
	bits = GetEmailBits()
	#print("%d   %d"%(bits, g_email_number))
	indexs = Con10ToN(g_email_chars_len, g_email_number, bits)
	g_email_number += 1
	email = ""
	for bit in indexs:
		email += g_email_chars[bit]
	#email += g_email_suf[n % g_email_suf_len]
	email += "@trxh.com"
	return email

"""
def GenEmail(n):
	email = ""
	indexs = Con10ToN(g_email_chars_len, n, 5)
	for bit in indexs:
		email += g_email_chars[bit]
	email += g_email_suf[n % g_email_suf_len]
	return email
"""

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
			account = GenAcc()
			password = GenPwd()
			phone = GenTelephone()
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
	save_file = os.path.join(cur_path, 'user_db')
	print(save_file)
	uid_sum = 100 * 10000
	SaveUidData(save_file, uid_sum)

"""
	n = 0
	while n < 100:
		n += 1
		email = GenEmail()
		print(email)
"""		