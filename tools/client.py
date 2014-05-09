#!/usr/bin/env python3

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
		self.name = c_str(data[0])

class flowly_network_header (flowly_struct):
	fmt = "!64sllll"
	def __init__ (self, data):
		self.name = c_str(data[0])
		self.time_in = data[1]
		self.nanotime_in = data[2]
		self.time_out = data[3]
		self.nanotime_out = data[4]

class flowly_item (flowly_struct):
	fmt = "!QQ"
	def __init__(self, data):
		self.value_in = data[0]
		self.value_out = data[1]

if __name__ == "__main__":
	if (len(sys.argv) < 3):
		print("usage: client.py [address] [port]")
		sys.exit(1)
	
	if (sys.version_info.major < 3 or (sys.version_info.major == 3 and sys.version_info.minor < 4)):
		print("unsupported Python version")
		sys.exit(1)
	
	family, addr = get_addr(sys.argv[1], sys.argv[2])
	
	with socket.socket(family, socket.SOCK_DGRAM) as sock:
		sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		sock.bind(addr)
		
		while True:
			data, remote_addr = sock.recvfrom(65536)
			end = flowly_header.size()
			header = flowly_header(struct.unpack(flowly_header.fmt, data[:end]))
			
			if (header.version != 2):
				print("Unsupported flowly protocol version")
				sys.exit(1)
			
			stats = list(map(flowly_stat_header, struct.iter_unpack(flowly_stat_header.fmt, data[end : end + header.stat_count * flowly_stat_header.size()])))
			
			end += header.stat_count * flowly_stat_header.size()
			
			print("Received data from {0.network_count} networks (time {0.time}):".format(header))
			
			for i in range(header.network_count):
				network = flowly_network_header(struct.unpack(flowly_network_header.fmt, data[end : end + flowly_network_header.size()]))
				end += flowly_network_header.size()
				items = map(flowly_item, struct.iter_unpack(flowly_item.fmt, data[end : end + header.stat_count * flowly_item.size()]))
				end += header.stat_count * flowly_item.size()
				
				print("{0.name}".format(network))
				for stat, item in zip(stats, items):
					in_per_sec = (item.value_in // (network.time_in + network.nanotime_in / 1000000))
					out_per_sec = (item.value_out // (network.time_out + network.nanotime_out / 1000000))
					print("\t{0.name}: {1.value_in} in ({2}/s), {1.value_out} out ({3}/s)".format(stat, item, in_per_sec, out_per_sec))
			
			print()
