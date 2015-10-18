#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import re, time, json, logging, hashlib, base64, asyncio
import markdown2
from coreweb import get, post
from models import User, Comment, Blog, next_id
from aiohttp import web
from apis import Page, APIError, APIValueError, APIResourceNotFoundError, APIPermissionError
from config import configs

COOKIE_NAME = 'awesession'
_COOKIE_KEY = configs.session.secret

def check_admin(request):
    if request.__user__ is None and not request.__user__.admin:
        raise APIPermissionError()


def user2cookie(user, max_age):
    '''
    Generate cookie str by user.
    '''
    #build cookie string by:id-expires-sha1
    expires = str(int(time.time() + max_age))
    s = '%s-%s-%s-%s' % (user.id, user.passwd, expires, _COOKIE_KEY)
    L = [user.id, expires, hashlib.sha1(s.encode('utf-8')).hexdigest()]
    return '-'.join(L)

def text2html(text):
    lines=map(lambda s: '<p>%s</p>' % s.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;'), filter(lambda s: s.strip() != '', text.split('\n')))
    return ''.join(lines)    

@asyncio.coroutine
def cookie2user(cookie_str):
    '''
    Parse cookie and load user if cookie is valid
    '''
    if not cookie_str:
        return None
    try:
        L=cookie_str.split('-')
        if len(L)!=3:
            return None
        uid,expires,sha1=L
        if int(expires) < time.time():
            return None
        user = yield from User.find(uid)
        if user is None:
            return None
        '''
        s='%s-%s-%s-%s' % (uid,user.passwd,expires,_COOKIE_KEY)
        if sha1!=hashlib.sha1(s.encode('utf-8').hexdigest()):
            logging.info('Invalid sha1')
            return None
        '''
        user.passwd = '******'
        return user
    except Exception as e:
        logging.exception(e)
        return None

def get_page_index(page_str):
    p=1
    try:
        p=int(page_str)
    except ValueError as e:
        pass
    if p<1:
        p=1
    return p

'''
@get('/')
def index_test(request):
    users = yield from User.findAll()
    return {
        '__template__': 'test.html',
        'users': users
    }
'''

'''
@get('/')
def index(request):
    summary = 'Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.'
    blogs = [
        Blog(id='1', name='Test Blog', summary=summary, created_at=time.time()-120),
        Blog(id='2', name='Somehing New', summary=summary, created_at=time.time()-3600),
        Blog(id='3', name='Learn Swift', summary=summary, created_at=time.time()-7200)
    ]
    return {
        '__template__': 'blogs.html',
        'blogs': blogs
    }
'''

'''
1. get home page
'''
@get('/')
def index(request):
    if request.__user__ is None:
        return {
            '__template__': 'index.html'
        }
    #return web.HTTPFound('/api/blogs')
    '''
    page_index=get_page_index(1)
    num=yield from Blog.findNumber('count(id)')
    p=Page(num,page_index)
    if num==0:
        return dict(page=p,blogs=())
    blogs=yield from Blog.findAll(orderBy='created_at desc', limit=(p.offset,p.limit))
    return dict(page=p,blogs=blogs)
    '''
    #return api_blogs(page='1')
    return manage_blogs(request,page='1')
    '''
    return {
        '__template__':'manage_blogs.html',
        'page_index':get_page_index('1')
        '__user__':request.__user__
    }
    '''
    #return 'redirect:/manage/blogs'

'''
1.register new user
'''
@get('/register')
def register():
    return {
        '__template__': 'register.html'
    }

_RE_EMAIL = re.compile(r'^[a-z0-9\.\-\_]+\@[a-z0-9\-\_]+(\.[a-z0-9\-\_]+){1,4}$')
_RE_SHA1 = re.compile(r'^[0-9a-f]{40}$')
@post('/api/users')
def api_register_user(*, email, name, passwd):
    if not name or not name.strip():
        raise APIValueError('name')
    if not email or not _RE_EMAIL.match(email):
        raise APIValueError('email')
    if not passwd or not _RE_SHA1.match(passwd):
        raise APIValueError('passwd')
    users = yield from User.findAll('email=?', [email])
    if len(users) > 0:
        raise APIError('register:failed', 'email', 'Email is already in use.')
    uid = next_id()
    sha1_passwd = '%s:%s' % (uid, passwd)
    user = User(id=uid,
                name=name.strip(),
                email=email,
                passwd=hashlib.sha1(sha1_passwd.encode('utf-8')).hexdigest(),
                image='http://www.gravatar.com/avatar/%s?d=mm&s=120' % hashlib.md5(email.encode('utf-8')).hexdigest()
                )
    yield from user.save()
    #make session cookie
    r = web.Response()
    r.set_cookie(COOKIE_NAME, user2cookie(user, 86400), max_age=86400, httponly=True)
    user.passwd = '******'
    r.content_type = 'application/json'
    r.body = json.dumps(user, ensure_ascii=False).encode('utf-8')
    return r

'''
login
'''
@get('/signin')
def signin():
    return {
        '__template__': 'signin.html'
    }

@post('/api/authenticate')
def authenticate(*, email, passwd):
    if not email:
        raise APIValueError('email', 'Invalid email')
    if not passwd:
        raise APIValueError('passwd', 'Invalid passwd')
    users = yield from User.findAll('email=?', [email])
    if (len(users) == 0):
        raise APIValueError('email', 'Email not exist.')
    user = users[0]
    #check passwd
    sha1 = hashlib.sha1()
    sha1.update(user.id.encode('utf-8'))
    sha1.update(b':')
    sha1.update(passwd.encode('utf-8'))
    if user.passwd != sha1.hexdigest():
        raise APIValueError('passwd', 'Invalid passwd')
    #authenticate ok, set cookie
    r = web.Response()
    r.set_cookie(COOKIE_NAME, user2cookie(user, 86400), httponly=True)
    user.passwd = '******'
    r.content_type = 'application/json'
    r.body = json.dumps(user, ensure_ascii=False).encode('utf-8')
    return r

'''
logout
'''
@get('/signout')
def signout(request):
    referer=request.headers.get('Referer')
    r=web.HTTPFound(referer or '/')
    r.set_cookie(COOKIE_NAME,'-deleted-',max_age=0,httponly=True)
    logging.info('user signed out')
    return r

@get('/manage/users')
def manage_users(*,page='1'):
    return {
        '__template__':'manage_users.html',
        'page_index':get_page_index(page)
    }

@get('/api/users')
def api_get_users(*,page='1'):
    page_index=get_page_index(page)
    num=yield from User.findNumber('count(id)')
    p=Page(num,page_index)
    if num==0:
        return dict(page=p,users=())
    users = yield from User.findAll(orderBy='created_at desc',limit=(p.offset,p.limit))
    for u in users:
        u.passwd = '******'
    return dict(users=users)

'''
1.blogs home page
html
'''
@get('/manage/blogs')
def manage_blogs(request,*, page='1'):
    return {
        '__template__':'manage_blogs.html',
        'page_index':get_page_index(page),
        '__user__':request.__user__
    }

'''
2.get blogs list
json
'''
@get('/api/blogs')
def api_blogs(*, page='1'):
    page_index=get_page_index(page)
    num=yield from Blog.findNumber('count(id)')
    p=Page(num,page_index)
    if num==0:
        return dict(page=p,blogs=())
    blogs=yield from Blog.findAll(orderBy='created_at desc', limit=(p.offset,p.limit))
    return dict(page=p,blogs=blogs)

'''
3.get blog identified by id
json
'''
@get('/api/blogs/{id}')
def api_get_blog(*, id):
    blog = yield from Blog.find(id)
    return blog

'''
4.get blog identified by id for reading
html
'''
@get('/blog/{id}')
def get_blog(request,*,id):
    blog=yield from Blog.find(id)
    comments=yield from Comment.findAll('blog_id=?',[id],orderBy='created_at desc')
    for c in comments:
        c.html_content=text2html(c.content)
    blog.html_content=markdown2.markdown(blog.content)
    return {
        '__template__':'blog.html',
        'blog':blog,
        'comments':comments,
        '__user__':request.__user__
    }

'''
1.return create blog page
html
'''
@get('/manage/blogs/create')
def manage_create_blog():
    print('manage_create_blog call in')
    return {
        '__template__': 'manage_blog_edit.html',
        'id': '',
        'action': '/api/blogs'
    }
    #post('/api/blogs')

'''
2.create a new blog to database
'''
@post('/api/blogs')
def api_create_blog(request, *, name, summary, content):
    check_admin(request)
    if not name or not name.strip():
        raise APIValueError('name', 'name cannot be empty')
    if not summary or not summary.strip():
        raise APIValueError('summary', 'summary cannot be empty')
    if not content or not content.strip():
        raise APIValueError('content', 'content cannot be empty')
    blog = Blog(user_id=request.__user__.id,
                user_name=request.__user__.name,
                user_image=request.__user__.image,
                name=name.strip(),
                summary=summary.strip(),
                content=content.strip())
    yield from blog.save()
    return blog

'''
1.edit blog identified by id
html
'''
@get('/manage/blogs/edit')
def manage_edit_blog(*,id):
    return {
        '__template__':'manage_blog_edit.html',
        'id':id,
        'action':'/api/blogs/%s' % id
    }

'''
2.update blog identified by id
'''
@post('/api/blogs/{id}')
def api_update_blog(id,request,*,name,summary,content):
    check_admin(request)
    blog=yield from Blog.find(id)
    if not name or not name.strip():
        raise APIValueError('name','name cannot be empty')
    if not summary or not summary.strip():
        raise APIValueError('summary','summary cannot be empty')
    if not content or not content.strip():
        raise APIValueError('content','content cannot be empty')
    blog.name=name.strip()
    blog.summary=summary.strip()
    blog.content=content.strip()
    yield from blog.update()
    return blog

'''
3.delete blog identified by id
'''
@post('/api/blogs/{id}/delete')
def api_delete_blog(request,*,id):
    check_admin(request)
    blog=yield from Blog.find(id)
    yield from blog.remove()
    return dict(id=id)
