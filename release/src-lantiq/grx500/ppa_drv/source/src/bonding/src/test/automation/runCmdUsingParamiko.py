import paramiko
import paramiko.ssh_exception as sshExecp
from miscProc import *

"""
    Arguments:
        hostIP  : IP of the host pc where the command need to run
        username: username of the host pc to log in
        password: password of the host pc to log in
        command : comamnd need to run
        pfile   : file pointer to the log file
    
    Returns:
        -1 if error occur while establishing the connection or running the command.
        output of the command which is a list. Each line is a element in list
"""

def runCommandInRemoteHost(hostIP, username, password, command, pfile):
    #username = "qateam"
    #password = "qateam"
    port = 22
    exitProgram = False
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    #ssh.connect(hostIP, port, username, password)

    try:
        ssh.connect(hostIP, port, username, password, timeout = 5)
    except sshExecp.BadHostKeyException as e:
        printAndLogMsg(f"The server host key is not verified while opening a connection to host", pfile)
        return -1
    except sshExecp.AuthenticationException as e:
        printAndLogMsg(f"Authentication fails to the host while opening a connection to host", pfile)
        return -1
    except sshExecp.SSHException as e:
        printAndLogMsg(f"Error connecting or establishing an SSH session while opening a connection to host", pfile)
        return -1
    except sshExecp.socket.error as e:
        printAndLogMsg(f"a socket error occurred while opening a connection to host", pfile)
        return -1
    printAndLogMsg(f"Successfully open ssh connection with {hostIP}", pfile)
    printAndLogMsg(f"Command to be run --> {command}", pfile)
    stdin, stdout, stderr = ssh.exec_command(command)
    stdoutlines = stdout.readlines()
    stderrorlines = stderr.readlines()
    ssh.close()
    if len(stderrorlines) > 0:
        for line in stderrorlines:
            if 'warning' not in line.lower():
                exitProgram = True
            printAndLogMsg(f"{line}", pfile)
        if True == exitProgram:
            return -1
    return stdoutlines