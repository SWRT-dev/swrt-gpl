import re
import threading
#from runCmdUsingParamiko import runCommandInRemoteHost
from miscProc import *
from commonStub import *
from telnetCli import telnetLoginToDUT, ConnectWithDUT, executeCommandviaTelnet


"""
Arguments:
    output:     client/server output according to TCP/UDP protocol
    switch:     1 for finding average Kbyte transfered
                2 for finding average bandwidth (Kbits/sec)
    pfile:      file pointer to write logs
    protocol:   protocol used in iperf (TCP/UDP)

Return:
    -1 if switch is other than 1 or 2 or some other error occured
     Average of Kbyte transfered or bandwidth according to switch value 1 or 2

Purpose:
    Find average Kbyte transfered or bandwidth from the iperf output provided.
"""
def findAverage(output, switch, pfile, protocol):
    regexp = r'\[\s*\d{1,3}\s*\][^(sec)]*sec\s*(\d*\.?\d*)\s*([KMGkmg])?Bytes\s*(\d*\.?\d*)\s*([KMGkmg])?bits/sec'
    sum = 0 
    for line in output:
        res = re.match(regexp, line)
        if not res:
            printAndLogMsg(f"Could not find number to calculate average for {line}", pfile)
            return -1
        if 1 == switch:
            data = res.group(1)
            dataUnitGroup = 2
        elif 2 == switch:
            data = res.group(3)
            dataUnitGroup = 4
        #Covert into Byte
        if 'k' == res.group(dataUnitGroup) or 'K' == res.group(dataUnitGroup):
            power = pow(10,3)
        elif 'm' == res.group(dataUnitGroup) or 'M' == res.group(dataUnitGroup):
            power = pow(10,6)
        elif 'g' == res.group(dataUnitGroup) or 'G' == res.group(dataUnitGroup):
            power = pow(10,9)
        elif None == res.group(dataUnitGroup):
            power = 1
        else:
            printAndLogMsg(f"Unknown data unit group to calculate average for {line}", pfile)
            return -1
        sum += float(data)*power
    avg = round(sum / (len(output)*pow(10,6)), 4)
    if 1 == switch:
        retStr = f"{avg}"
    elif 2 == switch:
        retStr = f"{avg}"
    return retStr

"""
    Arguments:
        output: Complete output of iperf command in list data structure
        args:   list of arguments used to run iperf using python subprocess
        pfile:  file pointer to write logs

    Return:
        If the protocol used is TCP then client output is returned and for UDP protocol server output is returned
    
    Purpose:
        Collecting client/server ouput according to TCP/UDP protocol
"""
def collectIperfOutput(output, protocol, iperfVersion, pfile):
    
    if protocol == 'UDP':
        #Collecting Server output
        serverOutputStarted = 0
        testStarted = 0
        reomveHeader = 0
        serverOutput = []
        serverOutputEnds = 0
        outputRegexp = r"\[.*\][^(sec)]*sec"
        for line in output:
            line = line.replace('\n', '')
            if serverOutputEnds == 1:
                break
            if testStarted == 1:
                if reomveHeader == 0:
                    reomveHeader = 1
                    continue
                else:
                    if re.match(outputRegexp, line):
                        serverOutput.append(line)
                        #print(f"Collect --> {line}")
                    else:
                        serverOutputEnds = 1
            if r'Server output' in line:
                serverOutputStarted = 1
            if serverOutputStarted == 1:
                if r'Starting Test' in line:
                    testStarted = 1
        #pfile.write(f"{getTimestamp()} Output for further processing:\n")
        output = serverOutput

    elif protocol == 'TCP':
        #Collecting Client Output
        removeHeader = 0 
        clientOutputEnds = 0
        headerRegexp = r"\[.*]\s*Interval"
        outputRegexp = r"\[.*\][^(sec)]*sec"
        clientOutput = []
        for line in output:
            line = line.replace('\n', '')
            if clientOutputEnds == 1:
                break
            if 0 == removeHeader:
                if re.search(headerRegexp, line):
                    removeHeader = 1
            else:
                if re.search(outputRegexp, line):
                    clientOutput.append(line)
                else:
                    clientOutputEnds = 1
        output = clientOutput
        
        #In iperf2 the last output is the summary of total transmission time, so it has to be discarted
        #In iperf3 no such thing is given
        if '2' == iperfVersion:
            output.pop()

    return output

"""
    Arguments: 
            args:               a list of parameters to run iperf from python subprocess
            startingServerPort: Starting port number to connect with iperf server
            noOfServerPort:     No of consequtive port to try with
            pfile:              file pointer to write logs
    Return:
            -1 if there is an error
            output of the iperf command

    description:
            This proc runs iperf using the given parameters using python subprocess
"""
def collectStat(ppfile, bondingStat):
    pfile = open('bonding.log', 'w')
    cmd = "cat /sys/kernel/debug/ima_bond/stats \n"
    telnetCli = ConnectWithDUT(pfile)
    telnetLoginToDUT(telnetCli, pfile)
    #printAndLogMsg(cmd, pfile)
    endPattern = r'root@ugwcpe:/#'
    timeout = 1
    telnetCli.write(cmd.encode('ascii'))
    output = telnetCli.read_until(endPattern.encode('ascii'), timeout = timeout)
    decodeOutput = output.decode('ascii')
    print(decodeOutput)
    writeInLog(decodeOutput.split('\n'), ppfile)
    for line in decodeOutput.split('\n'):
        bondingStat.append(line.strip())
    telnetCli.close()
    pfile.close()

def run_iperf(args, startingServerPort, noOfServerPort, serverAddr, pfile):
    args.append('-p')
    iperfExecution = False
    iperfCount = 0
    while False == iperfExecution and iperfCount < 25:
        incr = 0
        while incr <= int(noOfServerPort) and False == iperfExecution and iperfCount < 25:
            portAddr = int(startingServerPort) + incr
            args.append(f'{str(portAddr)}')
            iperfArgs = " "
            iperfArgs = iperfArgs.join(args)
            #printAndLogMsg(iperfArgs, pfile)
            bondingStat1 = []
            bondingStat2 = []
            highestFillLevel = []
            timer1 = threading.Timer(11.0, collectStat, args=[pfile, bondingStat1])
            timer2 = threading.Timer(31.0, collectStat, args=[pfile, bondingStat2])
            #timer3 = threading.Timer(38.0, prepareFillLevel, args=[bondingStat1, bondingStat2, highestFillLevel])
            timer1.start()
            timer2.start()
            #timer3.start()
            printAndLogMsg(iperfArgs, pfile)
            res, output = executeCommandInClient(serverAddr, iperfArgs)
            writeInLog(output, pfile)
            for line in output:
                print(line)
            if '0' != res and 'terminated' not in output[-1] and len(output) > 4 and "iperf done" in output[-1].lower():
                iperfOutput = output
                iperfExecution = True
                return 1, [iperfOutput, bondingStat1, bondingStat2]
                #break
            else:
                #printAndLogMsg(f"iperf failed: {output}", pfile)
                try:
                    #timer3.cancel()
                    timer2.cancel()
                    timer1.cancel()
                except Exception as e:
                    printAndLogMsg(f"Error while stopping timer: {e}", pfile)
            if 'the server is busy' not in output[-1]:
                iperfCount +=1
                printAndLogMsg(f"Number of iperf retries = {iperfCount} out of 25", pfile)        
            incr += 1
            args.pop()
            
    return -1, "iperf aborted after 25 retries"
    #return iperfOutput, bondingStat1, bondingStat2
    

def findLossDatagram(iperfOutput):
    count = 0
    gatherLossInfo = False
    reg = f".*ms\s*(\d+)/(\d+).*"
    lossInfo = []
    for line in iperfOutput:
        if True == gatherLossInfo:
            ma = re.match(reg, line)
            lossInfo.append(ma.group(1))
            lossInfo.append(ma.group(2))
            gatherLossInfo = False
        if "Lost/Total Datagrams" in line:
            count += 1
            if 2 != count:
                gatherLossInfo = True
    retString = f"{lossInfo[2]}/{lossInfo[3]}"
    if lossInfo[0] != lossInfo[2]:
        retString += '*'
    return retString
            
