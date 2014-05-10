flowly
======

An sFlow data analyzer focused on realtime processing

Configuration syntax
======
The configuration file is divided into three sections - variables, networks and clients.
It is possible to change the current section by typing the new section's name in square brackets on a single line.
The default section is "variables".

Variables
------
Variables modify flowly's default behavior. The syntax is "variable=value" with any number of spaces/tabs around the "=" character.

port - sets the port to listen for incoming sflow data
send_interval - the interval to send output data to clients (in milliseconds)

Networks
------
Networks are used to determine from which network did the received sample origin and where did it go.
The syntax is "address mask network_name"

Clients
------
A list of clients to which flowly sends data.

Example configuration file
------
	# Everything that follows a hash sign is a comment
	port = 6343 # The default sFlow port
	send_interval = 2000 # 2 seconds
	
	[networks]
	127.0.0.0	0.0.0.0		Network_1
	128.0.0.0	0.0.0.0		Network_2
	64.0.0.0/8			Network_1 # CIDR-style masks are supported too
	
	[clients]
	127.0.0.1	6666	raw
	#::1		6666	raw