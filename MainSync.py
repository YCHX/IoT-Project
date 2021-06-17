# -*- coding:utf-8 -*-
import socket
import time

host = "192.168.2.7" 
port = 8080 

serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serversock.bind((host,port)) 
serversock.listen(10) 

clientsock, client_address = serversock.accept()


while True:
    fl = open('./data/lnum.txt','r')
    
    s_msg = fl.read()
    

    clientsock.sendall(s_msg) 
    rcvmsg = clientsock.recv(1024)
    
    if rcvmsg == 'end':
        break
    fs = open('./data/snum.txt','w')
    fs.write(rcvmsg)
    fs.close()

    print 'Received -> %s' % (rcvmsg)

    

    fl.close()
    
    time.sleep(1)
clientsock.close()
