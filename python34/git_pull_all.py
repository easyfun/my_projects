# /usr/bin/env python
# -*- coding:utf-8 -*-

import os

branches = ["develop",
"changeMailValible",
"develop",
"feature/#IK7SH/app.api",
"feature/assetStatistics",
"feature/cleanRepeatAddress",
"feature/customCenterAPI",
"feature/incremental.push",
"feature/mine",
"feature/recordMemberAndAsset",
"feature/smsAndIdentity",
"feature/sms_access",
"feature/split.cache.websocket",
"features/usdt",
"issue/air.drop.tool",
"master",
"trader-bot",
"v2"]


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



# with open('pull_all.bat', 'w') as f:
# 	for branch in branches:
# 		# print(branch)
# 		# cmd = 'git checkout %s' % branch
# 		# result = os.popen(cmd)
# 		# print(result.read())

# 		# cmd = 'git pull'
# 		# result = os.popen(cmd)
# 		# print(result.read())

# 		cmd = 'git checkout %s && git pull\n' % branch
# 		# f.write(cmd)
# 		# cmd = 'git pull\n'
# 		f.write(cmd)





