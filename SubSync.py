# -*- coding:utf-8 -*-
import socket
import time

host = "192.168.2.20" 
port = 8080 

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect((host, port)) 



while True:
    fl = open('./data/lnum.txt','r')
    
    rcvmsg = client.recv(1024)
    
    if rcvmsg == 'end':
        break
    
    fs = open('./data/snum.txt','w')
    fs.write(rcvmsg)
    fs.close()

    print 'Sync -> %s' % (rcvmsg)

    s_msg = fl.read()
    client.send(s_msg) 
    fl.close()
    
    time.sleep(1)

client.close()