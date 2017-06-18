#!/usr/bin/env python
# -*- coding:utf-8 -*-

#安装Pillow
#安装pyWin32
#安装wmi

import wmi
import win32gui
import win32process
import win32con
from PIL import ImageGrab
import time

import smtplib  
from email.mime.text import MIMEText

TEAM_VIEWER = 'TeamViewer.exe'
TEAM_VIEWER_WINDOW_TEXT = 'TeamViewer'

def get_hwnds_for_pid (pid):
    def callback (hwnd, hwnds):
        if win32gui.IsWindowVisible(hwnd) and win32gui.IsWindowEnabled(hwnd):
            _, found_pid = win32process.GetWindowThreadProcessId(hwnd)
            if found_pid == pid:
                hwnds.append(hwnd)
            return True

    hwnds = []
    win32gui.EnumWindows(callback, hwnds)
    return hwnds

def main():
    w=wmi.WMI()
    for p in w.Win32_Process(name=TEAM_VIEWER):
        print("%5s   %s" % (p.ProcessId, p.Name))
        hwnds = get_hwnds_for_pid(p.ProcessId)
        for hwnd in hwnds:
            print(hwnd, '=>', win32gui.GetWindowText(hwnd))
            win32gui.ShowWindow(hwnd, win32con.SW_NORMAL)
            #win32gui.SetForegroundWindow(hwnd)
            time.sleep(3)
            pic = ImageGrab.grab()
            pic.save("hwnd.jpg")



#print(__name__)
if '__main__' == __name__:
    main()