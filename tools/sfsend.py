import socket

def send_data (dst : socket.socket, data : str):
	ints = [int(data[i : 8], 16) for i in range(0, len(data), 8)] # add endian conversion if necessary
	dst.sendto(bytes([n for n in n_list for n_list in ints]), len(data) / 2)

def create_socket (addr, port):
	for family, type, proto, canonname, sockaddr in socket.getaddrinfo(addr, port, family = socket.AF_UNSPEC):
		s = socket.socket(family, type = socket.SOCK_DGRAM)
		try:
			s.bind(sockaddr)
			return s
		except Exception:
			s.close()
	return None

