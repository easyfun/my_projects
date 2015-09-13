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
			user_id = "%d"%(n)
			uid = "uid.%d"%(n)
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
	SaveUidData(save_file, uid_sum)
