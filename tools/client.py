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
	fmt = "!lllll"
	def __init__ (self, data):
		self.version = data[0]
		self.time = data[1]
		self.nanotime = data[2]
		self.network_count = data[3]
		self.stat_count = data[4]

class flowly_stat_header (flowly_struct):
	fmt = "!64s"
	def __init__ (self, data):
		self.name = data[0]

class flowly_network_header (flowly_struct):
	fmt = "!64s"
	def __init (self, data):
		self.name = data[0]

class flowly_item (flowly_struct):
	fmt = "!QQ"
	def __init__(self, data):
		self.value_in = data[0]
		self.value_out = data[1]

if (len(sys.argv) < 3):
	print("usage: client.py [address] [port]")
	sys.exit(1)

family, addr = get_addr(sys.argv[1], sys.argv[2])

with socket.socket(family, socket.SOCK_DGRAM) as sock:
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.bind(addr)
	
	while True:
		data, remote_addr = sock.recvfrom(65536)
		end = flowly_header.size()
		header = flowly_header(struct.unpack(flowly_header.fmt, data[:end]))
		
		if (header.version != 0): # TODO upgrade to 1
			print("Unsupported flowly protocol version")
			sys.exit(1)
		
		stats = map(flowly_stat_header, struct.iter_unpack(flowly_stat_header.fmt, data[end : end + header.stat_count * flowly_stat_header.size()]))
		end += header.stat_count * flowly_stat_header.size()
		
		def items ():
			for i in range(header.network_count):
				yield i
		
		print("Received {0.network_count} items (time {0.time}):".format(header))
		
		for item in items:
			print("{0.direction} {0.network}\n\t {0.name} = {0.value}".format(item))
		
		print()
