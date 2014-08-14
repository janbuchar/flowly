flowly
======

An sFlow data analyzer focused on realtime processing

Description
------
Flowly acts as an sFlow collector (it accepts sFlow data on a configured port). 
When a packet containing flow samples arrives, flowly parses out the destination 
and source IP addresses, finds from/to which network does it go and stores data 
about it.

Collected data is periodically aggregated and sent to a list of clients via UDP 
(see Output format). Currently this data contains the packet and byte count, 
along with the time interval this traffic has been observed in, for each network 
and direction (in/out).

Configuration syntax
======
The configuration file is divided into three sections - variables, networks and 
clients. It is possible to change the current section by typing the new 
section's name in square brackets on a single line. The default section is 
"variables". The file is read line by line and every line consists of several 
tokens separated by any number of spaces/tabs.

It is possilbe to force flowly to reload the configuration by sending it a
SIGHUP.

Variables
------
Variables modify flowly's default behavior. The syntax is "variable=value" with 
any number of spaces/tabs around the "=" character.

- port - sets the port to listen for incoming sflow data
- send_interval - the interval to send output data to clients (in milliseconds)

Networks
------
Networks are used to determine from which network did the received sample origin 
and where did it go.
The syntax is "address mask network_name"

Clients
------
A list of clients to which flowly sends data. The syntax is "address 
destination_port format".
Only the "raw" format option is supported so far.

Example configuration file
------
	# Everything that follows a hash sign is a comment
	port = 6343 # The default sFlow port
	send_interval = 2000 # 2 seconds
	
	[networks]
	127.0.0.0/0		Network_1
	128.0.0.0/16		Network_2
	64.0.0.0/8		Network_1 # Only CIDR-style masks are supported
	
	[clients]
	127.0.0.1	6666	raw
	#::1		6666	raw

Output format
======
The following is a description of flowly's output format (version 2).

Header
------
Basic information about the output.
- Flowly protocol version (4B)
- Time (8B) - the time this packet was sent (the first 4B is the seconds part, 
	the second 4B represents nanoseconds)
- Network count (4B) - the number of networks about which flowly records data
- Stat count (4B) - the number of statistic information items flowly records 
	about every network

The header is followed by "Stat count" statistics headers

Statistics header
------
- Name (64B) - the name of the statistics item (0-terminated string)

The statistics headers are followed by "Network count" network information items

Network header
------
- Name (64B) - the name of the network (0-terminated string)
- "In" time interval (8B) - the time in which the data (going into the network) 
	was gathered
- "Out" time interval (8B) - the time in which the data (going out of the 
	network) was gathered

The network header is followed by "Stat count" statistics items

Statistics item
------
- "In" value (8B) - the value of the statistics item (for data going into the 
	network)
- "Out" value (8B) - the value of the statistics item (for data going out of the 
	network)

Implementation details
======
Data storing
------
Incoming data is stored in a circular buffer, which drops information about 
traffic from long ago. There is a buffer for each network and direction, so that 
networks with a lot of traffic don't make us drop information about other 
networks.

This implementation also allows fast copying (needed for independent outputting),
insertion and traversal, because a circular buffer can be stored in a statically
allocated array.

Network matching
------
In the current implementation, the matching algorithm is rather primitive - a list of 
(network address, mask) pairs is traversed, and the network of the first matching
pair is considered the result.

Output
------
Sending output is managed by a separate thread. This thread locks the statistic data,
copies and unlocks it, and then proceeds to send it to clients. After the data is sent,
the output thread goes to sleep for a time specified by the "send_interval" config 
variable.
