#!/usr/bin/env python
#-*- coding:utf-8 -*-

import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email.mime.image import MIMEImage  

from email import encoders
from email.header import Header

def sendmail():
    # 第三方 SMTP 服务  
    # mail_host='smtp.qq.com'  #设置服务器
    # mail_user='1060887552@qq.com'    #用户名
    # mail_pass='xordcfxcmmbfbfic'   #口令,QQ邮箱是输入授权码，在qq邮箱设置 里用验证过的手机发送短信获得，不含空格
    # sender = '1060887552@qq.com'
    # receivers = ['1143030113@qq.com']  # 接收邮件，可设置为你的QQ邮箱或者其他邮箱

    mail_host='smtp.163.com'
    mail_user='sunny9easyfun@163.com'
    mail_pass=''
    sender = 'sunny9easyfun@163.com'
    receivers = ['sunny9easyfun@163.com']  # 接收邮件，可设置为你的QQ邮箱或者其他邮箱
    
    message = MIMEText('1060887552', 'plain', 'utf-8')
    message['From'] = Header('sunny9easyfun@163.com', 'utf-8')
    message['To'] =  Header('sunny9easyfun@163.com', 'utf-8')
    message['Subject'] = Header('software development', 'utf-8')
    # message['Title'] = Header('数据错误', 'utf-8')

    try:
        smtpObj = smtplib.SMTP_SSL(mail_host, 465)
        # smtpObj.set_debuglevel(1)
        smtpObj.login(mail_user,mail_pass)
        smtpObj.sendmail(sender,receivers,message.as_string())
        smtpObj.quit()
        print('send email ok')
    except Exception as e:
        print('send email error')
        print(str(e))

#发附件
def sendmail_multi_part():
    # 第三方 SMTP 服务  
    # mail_host='smtp.qq.com'  #设置服务器
    # mail_user='1060887552@qq.com'    #用户名
    # mail_pass='xordcfxcmmbfbfic'   #口令,QQ邮箱是输入授权码，在qq邮箱设置 里用验证过的手机发送短信获得，不含空格
    # sender = '1060887552@qq.com'
    # receivers = ['1143030113@qq.com']  # 接收邮件，可设置为你的QQ邮箱或者其他邮箱

    mail_host='smtp.163.com'
    mail_user='sunny9easyfun@163.com'
    mail_pass=''
    sender = 'sunny9easyfun@163.com'
    receivers = ['sunny9easyfun@163.com']  # 接收邮件，可设置为你的QQ邮箱或者其他邮箱

    message = MIMEMultipart()
    message['From'] = Header('sunny9easyfun@163.com', 'utf-8')
    message['To'] =  Header('sunny9easyfun@163.com', 'utf-8')
    message['Subject'] = Header('TeamViewer', 'utf-8')

    #邮件正文是MIMEText
    message.attach(MIMEText('Send TeamViewer infomation', 'html', 'utf-8'))

    with open('hwnd.jpg', 'rb') as f:
        # 设置附件的MIME和文件名，这里是png类型:
        mime = MIMEBase('image', 'jpg', filename='hwnd.jpg')
        # 加上必要的头信息:
        mime.add_header('Content-Disposition', 'attachment', filename='hwnd.jpg')
        mime.add_header('Content-ID', '<0>')
        mime.add_header('X-Attachment-Id', '0')
        # 把附件的内容读进来:
        mime.set_payload(f.read())
        # 用Base64编码:
        encoders.encode_base64(mime)
        # 添加到MIMEMultipar)t:
        message.attach(mime)

        try:
            smtpObj = smtplib.SMTP_SSL(mail_host, 465)
            # smtpObj.set_debuglevel(1)
            smtpObj.login(mail_user,mail_pass)
            smtpObj.sendmail(sender,receivers,message.as_string())
            smtpObj.quit()
            print('send email ok')
        except Exception as e:
            print('send email error')
            print(str(e))

# 附件嵌入正文
def sendmail_multi_part_2():
    # 第三方 SMTP 服务  
    # mail_host='smtp.qq.com'  #设置服务器
    # mail_user='1060887552@qq.com'    #用户名
    # mail_pass='xordcfxcmmbfbfic'   #口令,QQ邮箱是输入授权码，在qq邮箱设置 里用验证过的手机发送短信获得，不含空格
    # sender = '1060887552@qq.com'
    # receivers = ['1143030113@qq.com']  # 接收邮件，可设置为你的QQ邮箱或者其他邮箱

    mail_host='smtp.163.com'
    mail_user='sunny9easyfun@163.com'
    mail_pass=''
    sender = 'sunny9easyfun@163.com'
    receivers = ['sunny9easyfun@163.com']  # 接收邮件，可设置为你的QQ邮箱或者其他邮箱

    message = MIMEMultipart('related')
    message['From'] = Header('sunny9easyfun@163.com', 'utf-8')
    message['To'] =  Header('sunny9easyfun@163.com', 'utf-8')
    message['Subject'] = Header('TeamViewer', 'utf-8')

    #邮件正文是MIMEText
    # message.attach(MIMEText('Send TeamViewer infomation', 'plain', 'utf-8'))
    message.attach(MIMEText('<html><body><h1>Send TeamViewer infomation</h1>' + '<p><img src="cid:0"></p>' + '</body></html>', 'html', 'utf-8'))

    with open('hwnd.jpg', 'rb') as f:
        # 设置附件的MIME和文件名，这里是png类型:
        mime = MIMEBase('image', 'jpg', filename='hwnd.jpg')
        # 加上必要的头信息:
        mime.add_header('Content-Disposition', 'attachment', filename='hwnd.jpg')
        mime.add_header('Content-ID', '<0>')
        mime.add_header('X-Attachment-Id', '0')
        # 把附件的内容读进来:
        mime.set_payload(f.read())
        # 用Base64编码:
        encoders.encode_base64(mime)
        # 添加到MIMEMultipar)t:
        message.attach(mime)

        try:
            smtpObj = smtplib.SMTP_SSL(mail_host, 465)
            # smtpObj.set_debuglevel(1)
            smtpObj.login(mail_user,mail_pass)
            smtpObj.sendmail(sender,receivers,message.as_string())
            smtpObj.quit()
            print('send email ok')
        except Exception as e:
            print('send email error')
            print(str(e))


if '__main__' == __name__:
    sendmail_multi_part_2()