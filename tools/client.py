import sys
import socket
import struct

def get_addr (addr, port):
	family, type, proto, canonname, sockaddr = socket.getaddrinfo(addr, port, family = socket.AF_UNSPEC)[0]
	return (family, sockaddr)

def c_str (val):
	"""
	Bytes containing null-terminated string => string
	"""
	return val[0:val.index(0)].decode()

class flowly_struct:
	@classmethod
	def size (cls):
		return struct.calcsize(cls.fmt)

class flowly_header (flowly_struct):
	fmt = "!lll"
	def __init__ (self, data):
		self.count = data[0]
		self.time = data[1]
		self.nanotime = data[2]

class flowly_stat (flowly_struct):
	fmt = "!c63s64sQ"
	def __init__(self, data):
		self.direction = data[0].decode()
		self.network = c_str(data[1])
		self.name = c_str(data[2])
		self.value = data[3]

if (len(sys.argv) < 3):
	print("usage: client.py [address] [port]")
	sys.exit(1)

family, addr = get_addr(sys.argv[1], sys.argv[2])

with socket.socket(family, socket.SOCK_DGRAM) as sock:
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.bind(addr)
	
	while True:
		data, remote_addr = sock.recvfrom(65536)
		header = flowly_header(struct.unpack(flowly_header.fmt, data[:flowly_header.size()]))
		items = map(flowly_stat, struct.iter_unpack(flowly_stat.fmt, data[flowly_header.size() : flowly_header.size() + header.count * flowly_stat.size()]))
		
		print("Received {0.count} items (time {0.time}):".format(header))
		
		for item in items:
			print("{0.direction} {0.network}\n\t {0.name} = {0.value}".format(item))
		
		print()
