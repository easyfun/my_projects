# /usr/bin/env python
# -*- coding:utf-8 -*-

import os

skip='remotes/origin/HEAD'
remotes='remotes/origin/'

cmd = 'git branch -a'
result = os.popen(cmd)
result = result.read()
result = result.split('\n')

with open('pull.bat', 'w') as f:
	for r in result:
		r=r.strip()
		if r.find('remotes/origin/HEAD') >= 0:
			continue

		if r.find(remotes) <= -1:
			continue

		branch = r[len(remotes):]
		print(branch)

		cmd = 'git checkout %s && git pull\n' % branch
		f.write(cmd)
