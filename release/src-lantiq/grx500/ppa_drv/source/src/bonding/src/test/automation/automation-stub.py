#-----------------------------------------------------------------------
# Entry file for automation's stub to be run in LAN-PC
#-----------------------------------------------------------------------
import sys
from commonStub import *
import subprocess
import getopt

from systemCheck import SystemCheck

def executeCommand(command):
    try:
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate(timeout=120)
        stdout = stdout.decode('ISO-8859-1')
        stderr = stderr.decode('ISO-8859-1').strip()
    except Exception as inst:
        #Execute if exception occurs
        print(f"Error in running command")
        return 0, "error"
    else:
        #Execute if no exception
        if "" != stderr:
            print(f"iperf command output gives error: {stderr}")
            return 0, stderr
        return 1, stdout

def bad_stub(server):
    port = 48091
    sock, msg = connectWithServer(server, port)
    print(msg)
    if -1 == sock:
        sys.exit()
    while(True):
        result, lines = recvPkt(sock)
        if result == -1:
            # Connection with relay-server broken
            try:
                close(sock)
            except:
                pass
            sock, msg = connectWithServer(server, port)
        print(lines)
        command = lines[1]
        print("command = {0}".format(command))
        if "exit" == command:
            continue
        cmdRunStatus, cmdRunResult = executeCommand(command.split(" "))
        cmdRunResult = cmdRunResult.split('\n')
        cmdRunResult = [i.strip() for i in cmdRunResult if i.strip()]
        sendPkt(sock, cmdRunResult, status=cmdRunStatus)
        print('sent')

def stub(server):

    while True:

        try:
            close(sock)
        except:
            pass

        # Check health of the remaining system
        SystemCheck.check_as_stub(server=server)

        sock, msg = connectWithServer(server, 48091)
        if -1 == sock:
            print ("Packet filters in CPE are not set for stub port {0}".format(RELAY_STUB_PORT))
            sys.exit(0)

        #--------------------------------------------------------
        # ...all is well
        # Ready to go...
        #--------------------------------------------------------
        print( "All is well" );
        disconnected = False

        while disconnected == False:

            try:
                result, lines = recvPkt(sock)
            except:
                result = "-1"

            if int(result) == -1:
                # Connection with relay-server broken
                disconnected = True
                break

            print(lines)
            command = lines[1].strip()
            print("command = {0}".format(command))
            if command == "STUB_ALREADY_CONNECTED":
                print ("Another stub already connected. Exiting...")
                sys.exit(0)
            
            if command == 'MASTER_DISCONNECTED':
                print ("Master has disconnected. Need to reconnect to relay-server")
                disconnected = True
                break

            if "exit" == command:
                print ("Ignoring 'exit' command" )
                continue

            cmdRunStatus, cmdRunResult = executeCommand(command.split(" "))
            cmdRunResult = cmdRunResult.split('\n')
            cmdRunResult = [i.strip() for i in cmdRunResult if i.strip()]
            try:
                sendPkt(sock, cmdRunResult, status=cmdRunStatus)
            except:
                # Connection with relay-server broken
                disconnected = True
            print('sent')

def showhelp(progname):
    print("Usage: {0} options".format(progname))
    print("       {0} -h|--help".format(progname))
    print(" Options:")
    print("  -r|--relay-server <IP address>  IP address of the relay-server. If not specified, {0} uses 192.168.1.1".format(progname))
    print("  -h|--help                       print this message and quit")

    
def main(argv):
    server = "192.168.1.1"
    try:
        opts, args = getopt.getopt(argv[1:],"hr:",["help", "relay-server="])
    except getopt.GetoptError:
        showhelp(argv[0])
        sys.exit(0)

    for opt, arg in opts:
        if opt == '-r':
            server = arg
        if opt == '-h':
            showhelp(argv[0])
            sys.exit(0)

    stub( server )

if __name__ == "__main__":
    main(sys.argv)
