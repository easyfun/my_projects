#!/usr/bin/env python
#-*- coding:utf-8 -*-

import os
import sys
import time
import json
import logging
import py_auto_test
import test_cases

APP_ROOT_DIR=os.path.abspath('../')
APP_LOG_FILE_NAME=os.path.join(os.path.abspath(APP_ROOT_DIR),'log','auto_test_report_%s.%s' % (time.strftime("%Y-%m-%d-%H-%M-%S", time.localtime()),'log'))
print('APP_LOG_FILE_NAME=',APP_LOG_FILE_NAME)

logging.basicConfig(level=logging.DEBUG,
                format='%(asctime)s|%(levelname)s|%(filename)s[line:%(lineno)d]|%(message)s',
                datefmt='%a, %d %b %Y %H:%M:%S',
                filename=APP_LOG_FILE_NAME,
                filemode='w')
console = logging.StreamHandler()
console.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s|%(levelname)s|%(filename)s[line:%(lineno)d]|%(message)s')
console.setFormatter(formatter)
logging.getLogger('').addHandler(console)


def conn_handle(service_name,timeout=30):
    return py_auto_test.DSBGetConn(service_name,timeout)

def send_handle(conn_id,req_packet,timeout=30):
    return py_auto_test.DSBSendReq(conn_id,req_packet,timeout)


def run_test():
	#print(help(py_auto_test))
	result=py_auto_test.DSBInit(os.path.join(APP_ROOT_DIR,'bin'),
		os.path.join(APP_ROOT_DIR,'log'),
		os.path.join(APP_ROOT_DIR,'config'),
		os.path.join(APP_ROOT_DIR,'bundles'),
		os.path.join(APP_ROOT_DIR,'template'))
	
	if not result:
		print('failed to start esb_middle service')
		exit(1)
	time.sleep(1)

	#run test cases
	print('ready to run test cases')
	print('len(all_test_cases)=',len(test_cases.all_test_cases))
	for test_case in test_cases.all_test_cases:
		print('msg_id=', test_case.__msg_id__, 'func_name=', test_case.__name__)
		test_case(conn_handle,py_auto_test.DSBDisConn,send_handle)

	print('auto test finish')
	print('ready to exit')
	time.sleep(1)
	py_auto_test.DSBStop()

if '__main__'==__name__:
	if len(test_cases.all_test_cases)>0:
		run_test()
