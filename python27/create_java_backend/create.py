#!/usr/bin/env python
#-*- coding:utf-8 -*-

import sys
import os
import jinja2
import shutil

from conf import *
from create_api import *
from create_impl import *

def create_root_pom():
    loader = jinja2.FileSystemLoader(searchpath = src_path)
    env = jinja2.Environment(loader = loader)
    template = env.get_template('pom.xml')

    xml = template.render(**template_key_words)
    path = os.path.join(project_path, 'pom.xml')
    with open (path, 'w') as f:
        f.write(xml)

    
if __name__ == "__main__":
    print(sys.getdefaultencoding())
    reload(sys)
    sys.setdefaultencoding('utf-8')
    print(sys.getdefaultencoding())

    if os.path.lexists(project_path):
        v = input('Will delete path = %s, input 1 to sure: ' % project_path)
        if v == 1:
            shutil.rmtree(project_path)
        else:
            exit()
    os.mkdir(project_path)

    create_root_pom()
    create_api_project()
    create_impl_project()


