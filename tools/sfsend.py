import socket

def send_data (sock : socket.socket, addr, data : str):
	ints = [int(data[i : i + 2], 16) for i in range(0, len(data), 2)] # add endian conversion if necessary
	sock.sendto(bytes(ints), addr)

def get_addr (addr, port):
	family, type, proto, canonname, sockaddr = socket.getaddrinfo(addr, port, family = socket.AF_UNSPEC)[0]
	return (family, sockaddr)

def create_socket (family):
	return socket.socket(family, socket.SOCK_DGRAM)