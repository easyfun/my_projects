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

g_uid_list = []

def SaveUidData(des_file, count):
	n = 0
	with open(des_file, "w") as des_fobj:
		n = 999999999
		count += n
		while n < count:
			uid = "%d"%(n)
			g_uid_list.append(uid)
			n += 1

		content = ""
		n = 0
		for uid in g_uid_list:
			content += uid
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
	save_file = os.path.join(cur_path, 'uid_db')
	print(save_file)
	uid_sum = 100 * 10000
	SaveUidData(save_file, uid_sum)
	print(len(g_uid_list))