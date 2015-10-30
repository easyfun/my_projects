#!/usr/bin/env python
#-*- coding:utf-8 -*-

import functools

all_test_cases=[]

def register_test_case(msg_id):
	def decorator(func):
		@functools.wraps(func)
		def wrapper(*args,**kw):
			return func(*args,**kw)
		wrapper.__msg_id__=msg_id
		all_test_cases.append(wrapper)
		return wrapper
	return decorator
