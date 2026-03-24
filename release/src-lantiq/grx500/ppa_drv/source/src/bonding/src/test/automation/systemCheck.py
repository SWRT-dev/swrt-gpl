import sys

#import fileinput
#import os
#import subprocess
#import getopt

import select
import socket

from commonStub import *

class SystemCheck:

    RELAY_MASTER_PORT = 48090
    RELAY_STUB_PORT = 48091

    context = { }

    #--------------------------------------
    # To check for reachability of CPE
    #
    # Returns True,
    # Otherwise returns False
    #--------------------------------------
    def check_reachability(server):
        
        # TODO
        return True

    #--------------------------------------
    # To check for arrival of heartbeats from relay-server
    #
    # Returns True,
    # Otherwise returns False
    #--------------------------------------
    def check_relay_heartbeats(server):

        print('checking for heartbeat')
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind(('', SystemCheck.RELAY_MASTER_PORT))

        timeout = 5
        read_ready, _, err_ready = select.select( [sock], [], [], timeout )
        if len(read_ready) == 0 or len(err_ready) != 0:
            print( 'No heartbeat' )
            return False;

        data, addr = sock.recvfrom(16)
        print("received heartbeat from: {0}, port: {1}. Expected from: {2}".format(addr[0], addr[1], server))
        sock.close()

        if addr[0] != server:
            return False

        return True

    #--------------------------------------
    # To tcp connect/disconnect as master, or as stub
    #
    # Returns True,
    # Otherwise returns False
    #--------------------------------------
    def connect_as_master():
        sock, msg = connectWithServer(SystemCheck.context['server'],
                                      SystemCheck.RELAY_MASTER_PORT)
        if sock == -1:
            print( "Failed to connect to {0} as master on port {1}".format(
                SystemCheck.context['server'],
                SystemCheck.RELAY_MASTER_PORT))
            return False
            
        SystemCheck.context['socket'] = sock

        print( "Connected as master to {0} at port number {1}".format(SystemCheck.context['server'],
                            SystemCheck.RELAY_MASTER_PORT))
        return True

    def disconnect_as_master():
        try:
            SystemCheck.context['socket'].close()
        except Exception as e:
            print( 'Ignorning the exception raised while closing socket: {0}'.format(e) )
        del SystemCheck.context['socket']

    def connect_as_stub():
        sock, msg = connectWithServer(SystemCheck.context['server'],
                                      SystemCheck.RELAY_STUB_PORT)
        if sock == -1:
            print( "Failed to connect to {0} as stub on port {1}".format(
                SystemCheck.context['server'],
                SystemCheck.RELAY_STUB_PORT))
            return False

        SystemCheck.context['socket'] = sock
            
        print( "Connected as stub to {0} at port number {1}".format(SystemCheck.context['server'],
                            SystemCheck.RELAY_STUB_PORT))
        return True

    def disconnect_as_stub():
        SystemCheck.disconnect_as_master()

    def common_check(server):

        # Check for reachability of CPE IP address
        if SystemCheck.check_reachability(server) != True:
            print( "IP address {0} not reachable. Probably needs to be powered on".format(server) )
            sys.exit(0)

        # Check for heartbeats from relay-server
        if SystemCheck.check_relay_heartbeats(server) != True:
            print( "automation_relayd not running in {0}. Needs to be started before running automation. Exiting...".format(server) )
            sys.exit(0)

    def check_as_master(server="192.168.1.1"):

        SystemCheck.context['server'] = server

        SystemCheck.common_check(server)

        # Check for packet-filter configuration in CPE for master
        if SystemCheck.connect_as_master() is True:
            pass
        else:
            print( "Packet filters in CPE not set for automation-TCP-Port: {0}".format(SystemCheck.RELAY_MASTER_PORT))
            print( "==========================================================" )
            print( "Protocol: TCP    Destination Address:192.168.1.1    Destination Netmask: 255.255.255.255" )
            print( "                 Destination Port:   {0}   Version: IPv4    Action: Accept".format(
                            SystemCheck.RELAY_MASTER_PORT))
            print( "==========================================================" )
            sys.exit(0)

        # Check duplication of master
        result, resp = recvPkt(SystemCheck.context['socket'], timeout=2)
        if int(result) >= 0 and resp[1].strip() == "MASTER_ALREADY_CONNECTED":
            print ("Another master is connected. Exiting...")
            sys.exit(0)

        # Check availability of stub
        sendPkt(SystemCheck.context['socket'], ["Hello stub"])
        result, resp = recvPkt(SystemCheck.context['socket'], timeout=2)
        if int(result) != 0:
            print( "Response timeout. Improper stub currently running. Proper stub needs to be run before running automation" )
            ok = False
        elif int(resp[0]) > 0 and resp[1].strip() == "STUB_NOT_CONNECTED":
            print( "Stub currently not running. It needs to be started before running automation" )
            ok = False
        else:
            ok = True

        if ok == False:
            sys.exit(0)

        SystemCheck.disconnect_as_master()

    def check_as_stub(server="192.168.1.1"):

        SystemCheck.context['server'] = server

        SystemCheck.common_check(server)

        # Check for packet-filter configuration in CPE for stub
        if SystemCheck.connect_as_stub() is True:
            pass
        else:
            print( "Packet filters in CPE not set for automation-stub-TCP-Port: {0}".format(SystemCheck.RELAY_STUB_PORT))
            print( "==========================================================" )
            print( "Protocol: TCP    Destination Address:192.168.1.1    Destination Netmask: 255.255.255.255" )
            print( "                 Destination Port:   {0}   Version: IPv4    Action: Accept".format(
                            SystemCheck.RELAY_STUB_PORT))
            print( "==========================================================" )
            sys.exit(0)

        # Check duplication of stub
        result, resp = recvPkt(SystemCheck.context['socket'], timeout=2)
        if int(result) >= 0 and resp[1].strip() == "STUB_ALREADY_CONNECTED":
            print ("Another stub is connected. Exiting...")
            sys.exit(0)

        SystemCheck.disconnect_as_stub()
