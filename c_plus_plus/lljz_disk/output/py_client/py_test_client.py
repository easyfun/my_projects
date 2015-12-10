#!/usr/bin/env python

import py_client

def RunTest():
    #print(help(py_client))
    if not py_client.Init():
        print("init fail")
        return

    conn_id=py_client.Connect("tcp:127.0.0.10000",True)
    print("conn_id=%d" % conn_id)

    py_client.Stop()
    py_client.Wait()


if "__main__"==__name__:
    RunTest()