#!/usr/bin/env python3
#-*- coding:utf-8 -*-

import orm,asyncio
from models import User,Blog,Comment
from config import configs
import inspect

'''
def test(loop):
    yield from orm.create_pool(loop, user='root', password='tianhe', db='awesome')
    u=User(name='test3',email='1060887552@qq.com',passwd='tianhe',image='about:blank')
    yield from u.save()

loop = asyncio.get_event_loop()
loop.run_until_complete(test(loop))
loop.close()
'''

'''
class ListMetaClass(type):
    def __new__(cls, name, bases, attrs):
        attrs['add'] = lambda self, value:self.append(value)
        return type.__new__(cls,name,bases,attrs)

class ClassName(object):
     """docstring for ClassName"""
     def __init__(self, arg):
         super(ClassName, self).__init__()
         self.arg = arg
          MyList(list, metaclass=ListMetaClass):
    pass

L = MyList()
L.add(1)
L.add(2)
print(L)
'''

class PersonMetaClass(type):
    def __new__(cls,name,bases,attrs):
        print(attrs)
        return type.__new__(cls,name,bases,attrs)

class Person(object, metaclass=PersonMetaClass):
    """docstring for Person"""
    age=13
    hobby=['peace','reading']
    def __init__(self,name):
        self.name = name

    def show_name(self):
        print(self.name)

p1=Person('Y')
p2=Person('J')
print(p1.age)
print(p2.age)
p1.age=20
print(p1.age)
print(p2.age)
print(p1.name)