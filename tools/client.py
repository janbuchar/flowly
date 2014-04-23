import sys
import socket
import struct

def get_addr (addr, port):
	family, type, proto, canonname, sockaddr = socket.getaddrinfo(addr, port, family = socket.AF_UNSPEC)[0]
	return (family, sockaddr)

class flowly_header:
	fmt = "!Qll"
	def __init__ (self, data):
		self.count = data[0]

class flowly_stat:
	fmt = "!c63s64sQ"
	def __init__(self, data):
		self.direction = data[0]
		self.network = data[1]
		self.name = data[2]
		self.value = data[3]

addr, family = get_addr("localhost", sys.argv[1])
sock = socket.socket(family)
sock.bind(addr)

while True:
	data, remote_addr = sock.recvfrom(65536)
	header = flowly_header(struct.unpack(flowly_header.fmt, data))
	items = map(flowly_stat, struct.iter_unpack(flowly_stat.fmt, data[struct.calcsize(flowly_header.fmt):]))
	
	for item in items:
		print("{0.network}".format(item))
