# Import socket module
from miscProc import printAndLogMsg
import socket			

# Create a socket object
"""s = socket.socket()		

# Define the port on which you want to connect
port = 48080

# connect to the server on local computer
s.connect(('192.168.1.1', port))

while True:
	str1 = input()
	if len(str1) != 0:
		s.send(str1.encode('utf-8'))
		if str1 == "exit":
			break
		buff = (s.recv(4096).decode())	
		print (buff)	
# close the connection
s.close()"""
	

def runCommandInClient(socketAddr, command, pfile):
	socketAddr.send(command.encode('utf-8'))
	if "exit" == command:
		socketAddr.close()
		return -1
	else:
		printAndLogMsg(f"{command}", pfile)
		result = socketAddr.recv(10).decode()
		length = socketAddr.recv(10).decode()
		print(f"result = {result}\nlegth = {length}")
		len = 0
		output = []
		while len < int(length):
			output.append(socketAddr.recv(1024).decode())
			print(f"line: {output[-1]}")
			while output[-1][-1] != '\n':
				print("In while")
				output[-1] += socketAddr.recv(1024).decode()
			len += 1
		return result, output

def connectWithRelayServer(RelayServerAddr, port, pfile):
	try:
		s = socket.socket()
		s.connect((RelayServerAddr, port))
	except Exception as e:
		printAndLogMsg(f"Error while connecting with client: {e}", pfile)
		return -1
	return s