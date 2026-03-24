import sys
import socket
import select
from miscProc import *
import time as timeS
""" 
    Argument: 
            port: port no of DUT where to connect
    Return:
            returns two arguments
                first: -1 if could not connect with server stub
                        s, socket object when server connection is successful
                second:
                        message
    Purpose: connect with relay server with given port

"""
def connectWithServer(ipaddr, port):
    try:
        s = socket.socket()
        s.connect((ipaddr, port))
    except Exception as e:
        msg = f"Could not connect with server: {e}"
        return -1, msg
    msg = f"Connected with server successfully"
    return s, msg

#--------------------------------------
# sendPkt(): Constructs a complete packet by glueing the lines into one packet,
# Prepend two header lines to this packet and send it through the tcp connection: conn
#
# Returns True on success,
# False otherwise
# Raises Exception if connection closes
#--------------------------------------
def sendPkt(conn, lines, status=1):

	tmplist = lines[:]
	linecount = len(tmplist)
	tmplist.insert( 0, "{0}".format(status) );
	tmplist.insert( 1, "{0}".format(linecount) )
	packet = "\r\n".join(tmplist) + "\n"
	# print( "packet sent = {0}".format(packet))
	conn.sendall( packet.encode('utf-8'))	
	return True

#--------------------------------------
# recvPkt(): To receive a complete packet from tcp connection: conn
# Fills up the output variable: response with the packet: as a list of lines
#
# Returns:
# True on receiving a complete packet
# False on timeout
# Raises Exception if connection closes
#--------------------------------------
def read_line(conn):

    #------------------------------------------
    # This function reads the next line from tcp connection
    #------------------------------------------

    line = ""
    while True:
        char = conn.recv(1).decode("utf-8")
        if char == '\n':
            break;
        if len(char) > 0:
            line += char
        else:
            raise socket.timeout("connection is closed")

    return line[:] # Return a copy of the line

def recvPkt(conn, timeout=-1):

	try:
		if timeout < 0:
			read_ready, _, _ = select.select( [conn], [], [] )
		else:
			read_ready, _, _ = select.select( [conn], [], [], timeout )
	except: 
		print( 'recvPkt: select failed' )
		sys.exit(-1)
    
	if len(read_ready) == 0:
		# Timeout has occurred
		print( 'recvPkt: timeout' )
		return -1, ["TIMEOUT"]

	result = read_line(conn).strip()

	#print( "RCV-PKT: Line 1: {0}".format(result) )

	response = []
	line = read_line(conn).strip()
	linecount = int(line)
	response.append(line)

	#print( "RCV-PKT: Line 2: {0}".format(line) )

	for i in range(linecount):
		line = read_line(conn)
		response.append(line)
		#print( "RCV-PKT: Line {0}: {1}".format(i+3, line) )

	return result, response


def executeCommandInClient(sock, command):
    res = sendPkt(sock, [command], 1)
    #print( "Sent command to stub: {0}".format(command))
    if "exit" == command:
        sock.close()
        return 1, 1
    if res is False:
        return -1, "Failed to send command"
    #print( "Waiting for response from stub: {0}".format(command))
    res, output = recvPkt(sock)
    return res, output

