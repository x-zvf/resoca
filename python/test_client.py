import ResocaMessage_pb2
import struct

rsm = ResocaMessage_pb2.ResocaMessage()

import socket

hp = ("0.0.0.0", 23636)

def send(sock, msg):
    s = msg.SerializeToString()
    l = struct.pack("<h", len(s))
    sock.send(l)
    sock.send(s)

def stress():
    a = []
    rsm.messageType = rsm.PING
    while True:
        print("creating")
        while len(a) < 100:
            s = socket.socket()
            s.connect(hp)

            for i in range(10):
                send(s, rsm)
                s.recv(4)
            a.append(s)
        print("sending some more")
        for x in a:
            send(x,rsm)
            x.recv(4)
        print("deleting")
        while len(a)>0:
            x = a.pop()
            x.close()

