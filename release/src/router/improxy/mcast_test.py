#!/usr/bin/env python

import socket
import time
import sys
import optparse
import struct
import platform

version="1.0"

if platform.system() == 'Darwin':
    if not hasattr(socket, 'IP_ADD_SOURCE_MEMBERSHIP'):
        setattr(socket, 'IP_ADD_SOURCE_MEMBERSHIP', 70)

if platform.system() == 'linux':
    if not hasattr(socket, 'IP_ADD_SOURCE_MEMBERSHIP'):
        setattr(socket, 'IP_ADD_SOURCE_MEMBERSHIP', 39)

if platform.system() == 'Windows':
    if not hasattr(socket, 'IP_ADD_SOURCE_MEMBERSHIP'):
        setattr(socket, 'IP_ADD_SOURCE_MEMBERSHIP', 15)
    socket.IPPROTO_IPV6 = 41


# Parse cmdline options
parser = optparse.OptionParser(version="%prog " + version)
parser.add_option("-m", "--multicast-addr", dest="mc_addr",
                  help="Multicast addr receive or send to", metavar="MC")
parser.add_option("-p", "--multicast-port", dest="port", default= 1234,
                  help="Multicast port receive or send to", metavar="PORT")
parser.add_option("-l", "--listen-addr", dest="listen_addr", default='',
                  help="Listen on addr LA/Send from this ip", metavar="LA")
parser.add_option("-s", "--source-filter", dest="source_addr",
                  help="Only recv from SA[IPv4 Only now]", metavar="SA")
parser.add_option("-t", "--ttl", dest="ttl", default=2,
                  help="Specify Time to live/hop limit for sender", metavar="TTL")
parser.add_option("-r", "--receiver", dest="receiver", default=False, action="store_true",
                  help="Receiver or Sender, Sender by default")
options, args = parser.parse_args()
if options.mc_addr == None :
    parser.error("option -m not specified")


def recv():
## UDP Socket
    addrinfo = socket.getaddrinfo(options.mc_addr, None)[0]

    s = socket.socket(addrinfo[0], socket.SOCK_DGRAM)

    # Allow multiple copies of this program on one machine
    # (not strictly needed)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


    group_bin = socket.inet_pton(addrinfo[0], addrinfo[4][0])
    # Join group
    if addrinfo[0] == socket.AF_INET: # IPv4
        if options.listen_addr == '':
            options.listen_addr='0.0.0.0'
        if options.source_addr == None:
            imr = group_bin + socket.inet_aton(options.listen_addr)
            s.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, imr)
        else:
            if platform.system() == 'Darwin':
                imr = group_bin + socket.inet_aton(options.source_addr) + socket.inet_aton(options.listen_addr)
            else:
                imr = group_bin + socket.inet_aton(options.listen_addr) + socket.inet_aton(options.source_addr)
            s.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_SOURCE_MEMBERSHIP, imr)
    else:
        mreq = group_bin + struct.pack('@I', 0)
        s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP, mreq)

    # Bind it to the port
    s.bind(('', int(options.port)))
    # Loop, printing any data we receive
    while True:
        data, sender = s.recvfrom(1500)
        while data[-1:] == '\0': data = data[:-1] # Strip trailing \0's
        print (str(sender) + '  ' + repr(data))

def sender():

    addrinfo = socket.getaddrinfo(options.mc_addr, None)[0]

    s = socket.socket(addrinfo[0], socket.SOCK_DGRAM)
    s.bind((options.listen_addr, int(options.port)))
    # Set Time-to-live (optional)
    ttl_bin = struct.pack('@i', int(options.ttl))
    if addrinfo[0] == socket.AF_INET: # IPv4
        s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl_bin)
    else:
        s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, ttl_bin)

    while True:
        data = repr(time.time())
        s.sendto(data + '\0', (addrinfo[4][0], int(options.port)))
        print 'send %s %s' % (options.mc_addr, data)
        time.sleep(1)

if options.receiver == True:
    recv()
else:
    sender()
