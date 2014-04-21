import socket
import random
import captures

def send_data (sock : socket.socket, addr, data : str):
	ints = [int(data[i : i + 2], 16) for i in range(0, len(data), 2)] # add endian conversion if necessary
	sock.sendto(bytes(ints), addr)

def get_addr (addr, port):
	family, type, proto, canonname, sockaddr = socket.getaddrinfo(addr, port, family = socket.AF_UNSPEC)[0]
	return (family, sockaddr)

def create_socket (family):
	return socket.socket(family, socket.SOCK_DGRAM)

def send_single (addr, port, data):
	"""
	Send a single packet
	
	Args:
		addr: a string representation of destination address
		port: destination port
		data: data to be sent
	"""
	family, sockaddr = get_addr(addr, port)
	with create_socket(family) as sock:
		send_data(sock, sockaddr, data)

def send_list (addr, port, data):
	"""
	Send every item of an iterable as a packet
	
	Args:
		addr: a string representation of destination address
		port: destination port
		data: list of items to be sent
	"""
	family, sockaddr = get_addr(addr, port)
	with create_socket(family) as sock:
		for item in data:
			send_data(sock, sockaddr, item)

def berserk (addr, port):
	"""
	Gets medieval on a collector :)
	
	Args:
		addr: a string representation of destination address
		port: destination port
	"""
	def gen ():
		for i in range(10000):
			yield captures.data[random.randint(0, len(captures.data) - 1)]
	
	send_list(addr, port, gen())
