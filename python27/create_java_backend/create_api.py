#!/usr/bin/env python
#-*- coding:utf-8 -*-

import os
import jinja2
import shutil

from conf import *

def create_pom():
    # 创建pom
    os.mkdir(project_api_path)

    loader = jinja2.FileSystemLoader(searchpath = src_api_path)
    env = jinja2.Environment(loader = loader)
    template = env.get_template('pom.xml')

    xml = template.render(**template_key_words)
    path = os.path.join(project_api_path, 'pom.xml')
    with open (path, 'w') as f:
        f.write(xml)

def create_package_file(src_api_src_path, project_api_src_path):
    # print(src_api_src_path)
    # print(project_api_src_path)

    loader = jinja2.FileSystemLoader(searchpath = src_api_src_path)
    env = jinja2.Environment(loader = loader)

    files = os.listdir(src_api_src_path)
    for file in files:
        if file == '.' or file == '..':
            continue
        
        src_path = os.path.join(src_api_src_path, file)
        # print(src_path)
        if os.path.isfile(src_path):
            template = env.get_template(file)
            xml = template.render(**template_key_words)

            new_file = file
            if file in rename_file:
                new_file = rename_file[file]
            dest_path = os.path.join(project_api_src_path, new_file)
            with open(dest_path, 'w') as f:
                f.write(xml)
            continue

        if os.path.isdir(src_path):
            dest_path = os.path.join(project_api_src_path, file)
            os.mkdir(dest_path)
            create_package_file(src_path, dest_path)
            continue

def create_src():
    src_package_path = src_package_name.split('.')
    src_api_src_path = os.path.join(src_api_path, 'src', 'main', 'java', *src_package_path)
    # print(src_api_src_path)

    project_package_path = project_package_name.split('.')
    project_api_src_path = os.path.join(project_api_path, 'src', 'main', 'java', *project_package_path)
    # print(project_api_src_path)

    os.makedirs(project_api_src_path)

    # 生成java包,文件
    create_package_file(src_api_src_path, project_api_src_path)

def create_api_project():
    create_pom()

    create_src()