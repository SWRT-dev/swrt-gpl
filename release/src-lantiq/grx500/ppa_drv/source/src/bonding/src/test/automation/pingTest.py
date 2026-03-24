import re
from miscProc import *
from runCmdUsingParamiko import runCommandInRemoteHost

"""
    Arguments:
        pingOutput:      Output of the ping command
        noOfICMPPackets: Number of ICMP packets asked to send
        regexr:          regular expression to validate successful ICMP reply
        pfile:           file pointer to the log file
    
    Return:
        return average round trip time

    Purpose:
        Print total number of icmp packets send and total number of icmp reply and find the average round trip time

"""
def fetchImpPingDetails(pingOutput, noOfICMPPackets, regexr, pfile):
    regStart = f"PING|Pinging .* bytes of data."
    fetchedOutput = []
    regEnd = '^\s*$'
    outputStart = 0
    ouputEnd = 0
    #for line in pingOutput.split('\n'):
    for line in pingOutput:
        line = line.replace('\n', '')
        if 1 == ouputEnd:
            break
        if 0 == outputStart:
            if re.match(regStart, line):
                outputStart = 1
        else:
            if re.match(regEnd, line):
                ouputEnd = 1
            else:
                fetchedOutput.append(line)
    totalRTT = 0
    successfullICMPReply = 0

    #pfile.write(f"{getTimestamp()} Output Colleceted for processing\n")
    #for line in fetchedOutput:
    #    pfile.write(f"{getTimestamp()} {line.strip()}\n")
    #print(fetchedOutput)
    for line in fetchedOutput:
        result = re.match(regexr, line)
        if result:
            totalRTT += float(result.group(2))
            successfullICMPReply += 1

    #printAndLogMsg(f"Total No of ICMP request send = {noOfICMPPackets}", pfile)
    #printAndLogMsg(f"Total No of ICMP reply lost = {int(noOfICMPPackets) - successfullICMPReply}", pfile)
    successRatio = int(successfullICMPReply) / int(noOfICMPPackets)
    if successRatio < 0.60:
        print("Ping failed")
        return successRatio, -1
    else:
        #printAndLogMsg(f"Success percentage of ping: {successRatio * 100}%", pfile)
        avgRTT = totalRTT / successfullICMPReply
        return successRatio, round(avgRTT,3)

"""
    Arguments:
        args:           a list of parameters to run ping from python subprocess
        serverAddress:  address where to ping
        pfile:          file pointer to the log file
    
    Return:
        -1 if the ping command can not be run successfully
        output of the ping command
    
    Purpose:
        run ping command useing python subprocess
"""

def runPing(args, serverAddress, hostIP, username, password, pfile):
    command_join= ' '
    command = command_join.join(args)
    output = runCommandInRemoteHost(hostIP, username, password, command, pfile)
    if -1 == output:
        printAndLogMsg(f"Ping to {serverAddress} from {hostIP} is not successfull existing the program", pfile)
    return output