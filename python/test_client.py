import ResocaMessage_pb2
import struct
import time

rsm = ResocaMessage_pb2.ResocaMessage()

import socket

hp = ("0.0.0.0", 23636)

sock = socket.socket()
sock.connect(hp)

class ResocaClient():
    def __init__(self, hp):
        self.sock = socket.socket()
        self.sock.connect(hp)
        self.pending = dict()
        self.nr = 0
        self.sessionPrefix = 0

    def send(self, msg):
        s = msg.SerializeToString()
        l = struct.pack("<h", len(s))
        self.sock.send(l)
        self.sock.send(s)

    def sendWS(self, msg):
        rid = self.sessionPrefix + ( self.nr & 0xFFFF)
        self.nr += 1
        self.pending[rid] = time.monotonic()
        msg.request.requestID = rid
        self.send(msg)

    def readRSM(self):
        lr = self.sock.recv(2)
        l = struct.unpack("<h", lr)[0]
        b = self.sock.recv(l)
        rsm = ResocaMessage_pb2.ResocaMessage().FromString(b)
        self.pending.pop(rsm.response.responseID, None)
        print("RECV: ", str(rsm))
        return rsm

    def getInfo(self):
        infoMsg = ResocaMessage_pb2.ResocaMessage()
        infoMsg.isResponse = False
        infoMsg.request.requestType = infoMsg.request.INFO
        self.send(infoMsg)
        resMsg = self.readRSM()
        self.interface = resMsg.response.resocaInfo.interfaces
        self.sversion = resMsg.response.resocaInfo.version
        self.sessionPrefix = resMsg.response.resocaInfo.sessionPrefix

    def sendPing(self):
        msg = ResocaMessage_pb2.ResocaMessage()
        msg.isResponse = False
        msg.request.requestType = msg.request.PING
        self.sendWS(msg)



