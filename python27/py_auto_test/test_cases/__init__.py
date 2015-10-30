import os
import sys
from utility_manager import all_test_cases

cur_dir=os.path.dirname(__file__)
for x in os.listdir(cur_dir):
	cur_file=os.path.join(cur_dir,x)
	if not os.path.isfile(cur_file):
		continue

	if not x.endswith('Req.py'):
		continue

	if not x.startswith('Msg_'):
		continue

	n = x.rfind('.')
	if n == (-1):
        	__import__(x, globals(), locals())
	else:
        	__import__(x[:n], globals(), locals())
