#!/usr/bin/env python
#-*- coding:utf-8 -*-


def number_list():
    n=0
    
    c=0
    while True:
        print('before yield')
        c += 1
        yield n
        print('after yield')
        n += 1
        
        if c>2:
            break
    

def test_generator():
    print(number_list)
    n=number_list()
    print(n)

    c=0
    while c<6:
        c += 1
        print(next(n))
    return

    i=0    
    for a in n:
        i+=1
        print(a)
        if i>2:
            break


def list_cal():
    l=(x*x for x in range(10))
    print(l)



class Fibs(object):
    def __init__(self):
        self.a=0
        self.b=1
        self.n=0
        
    def next(self):
        self.a,self.b=self.b,self.a+self.b
        self.n += 1
        if self.n > 10:
            raise StopIteration
        return self.a
    
    def __iter__(self):
        return self
    
    
class Test(object):
    a=1
    
    def __init__(self):
        self.c=3
        
        
class Test2(Test,str):
    pass

if '__main__'==__name__:
    
    '''
    t1=Test()
    t2=Test()
    print(t1.a)
    t1.a=3
    print(t1.a)
    print(t2.a)
    
    
    t1.c=5
    print(t2.c)
    '''
    
    t1=Test()
    t1.b=5
    t1.a=10
#     print(t1.__dict__)
#     print(Test.__dict__)
    
    t2=Test()
#    print(t2.a)
    print(t2.__class__)
    print(Test.__class__)
    print()
    
#     print(Test.__bases__)
#     print(Test2.__bases__)
#     print(type(t2))