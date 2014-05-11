#!/usr/bin/env ruby

require 'socket'

u = UDPSocket.new
u.bind(ARGV.shift, ARGV.shift)

loop do
	data, peer = u.recvfrom(2**16)
	version,time,nanotime,network_count,stat_count = data.unpack('l>l>l>l>l>')
	data=data[20..-1]

	fmt=stat_count.times.map{'Z64'}.join
	stats = data.unpack(fmt)
	data = data[64*stat_count..-1]

	network_count.times do
		r = data.unpack('Z64l>l>l>l>q>q>q>q>')

		table,time_in,nanotime_in,time_out,nanotime_out,packets_in,packets_out,bytes_in,bytes_out = r
		puts table

		puts "\tpacket_count: #{packets_in} in, #{packets_out} out"
		puts "\tbyte_count: #{bytes_in} in, #{bytes_out} out"
		data = data[112..-1]
	end
end