import ResocaMessage_pb2
import struct

rsm = ResocaMessage_pb2.ResocaMessage()

import socket
sock = socket.socket()
sock.connect(("0.0.0.0", 23636))

def send(msg):
    s = msg.SerializeToString()
    l = struct.pack("<h", len(s))
    sock.send(l)
    sock.send(s)

