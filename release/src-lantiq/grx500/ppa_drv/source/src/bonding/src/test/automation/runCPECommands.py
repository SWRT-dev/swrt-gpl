import re
from miscProc import *
from telnetCli import *
import time

def fetchActualTXDelay(lines): 

    analyse_str = ''.join([i.strip() for i in lines])
    l0 = -1
    l1 = -1
    reg0 = ".*transmission delay: \[0:(\d+), 1:(\d+)\].*"
    ma0 = re.match(reg0, analyse_str)
 
    if ma0:
        l0 = int(ma0.group(1))
        l1 = int(ma0.group(2))
    if l0!=-1 and l1!=-1:
        return l0,l1
    elif l0==-1 and l1!=-1:
        return None,l1
    elif l0!=-1 and l1==-1:
        return l0,None
    elif l0 ==-1 and l1 ==-1:
        return None,None          

def fetchActualDataRate(lines):

    analyse_str = ''.join([i.strip() for i in lines])
    l0 = -1
    l1 = -1
    reg0 = ".*nDevice=0.*ActualDataRate=(\d+).*nDevice=1"
    reg1 = ".*nDevice=1.*ActualDataRate=(\d+)"
    ma0 = re.match(reg0, analyse_str)
    ma1 = re.match(reg1, analyse_str)
    try:
        # Convert from bps to kbps
        l0 = round(int(ma0.group(1))/1000)
    finally:
        try:
            # Convert from bps to kbps
            l1 = round(int(ma1.group(1))/1000)
        finally:
            if l0!=-1 and l1!=-1:
                return l0,l1
            elif l0==-1 and l1!=-1:
                return None,l1
            elif l0!=-1 and l1==-1:
                return l0,None
            elif l0 ==-1 and l1 ==-1:
                return None,None       

def findPirValue(data):
    reg = r"pir\s*=\s*(\d+)"
    pirValue = -1
    for line in data:
        if "pir" in line:
            match = re.search(reg, line)
            pirValue = match.group(1)
            break
    return pirValue

"""
    tn is the telnet object
    Return:
        Returns a list of two elements:
        First element: -1 if root login to DUT fails
                       -2 when reopening a telnet connection fails
                        1 when setting line rate command os successfull
        Second element:
                        telnet object

"""
def setLineRates(cmdlinerates, tn, pfile):

    #command = f'/opt/intel/usr/sbin/setlinerate.sh 0 {mlineRateLine0}\n'

    for lineidx, cmdlinerate in enumerate(cmdlinerates):

        if cmdlinerate == None:
            continue;

        cmd = '/opt/intel/usr/sbin/setlinerate.sh {0} {1}\n'.format(lineidx, cmdlinerate)

        setLinerateTryCount = 1
        setLinerateStatus = False
        while setLinerateTryCount < 4 and False == setLinerateStatus:
            printAndLogMsg(f"Executing command {cmd.strip()}", pfile)
            output = executeCommandviaTelnet(tn, cmd, 300, r'root@ugwcpe:/#', 2, pfile)
            if -2 == output:
                return [-1, tn]
            writeInLog(output[1], pfile)
            if -1 == output[0]:
                setLinerateTryCount += 1
                printAndLogMsg(f"Timeout occurs when running command {cmd}", pfile)
                printAndLogMsg(f"Closing the telnet connection to stop the command and open again", pfile)
                tn.close()
                tn = ConnectWithDUT(pfile) #telnet
                if -1 == tn:
                    printAndLogMsg("Error in reopening the telnet connection, exsiting the current configuration", pfile)
                    return [-2, tn]
                
                ret = telnetLoginToDUT(tn, pfile)
                if -1 == ret:
                    printAndLogMsg("DUT login as a root user failed, exiting the current configuration", pfile)
                    #tn.close()
                    return [-1, tn]
                else:
                    print(f"Login to DUT as a root user")
            else:
                setLinerateStatus = True

        if False == setLinerateStatus:
            printAndLogMsg(f"Command ==> {cmd} could not run successfully, exiting the current configuration", pfile)
            return [-1, tn]
    
    return [1, tn]

def fetchSWShaperRate(data, pfile):
    #reg = r"swq:\s*\[1\s*:\*(\d+)\s*,\s*2\s*:\s*(\d+)\s*,\s*3\s*:\s*(\d+)\s+\]"
    reg = r"swq:\s*\[1\s*:\s*(\d+)\s*,\s*2\s*:\s*(\d+)\s*,\s*3\s*:\s*(\d+)\s*\]"
    swq = []
    for line in data:
        if 'swq' in line:
            match = re.search(reg, line)
            try:
                swq.append(match[1])
                swq.append(match[2])
                swq.append(match[3])
            except Exception as e:
                printAndLogMsg(f"Error while retirving software shaper rate {e}", pfile)
                return -1
    return swq

def isInterfaceUp(interfaceName, tn, pfile):
    command = f"ifconfig {interfaceName}\n"
    output = executeCommandviaTelnet(tn, command, 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        tn.close()
        return -1
    writeInLog(output[1], pfile)
    for line in output[1]:
        print(line)
        if 'RUNNING' in line:
            if 'UP' in line:
                return True
            else:
                return False
    return False

def setPirValue(tn, wanInterfaceName, pfile):
    ret = isInterfaceUp('nas0', tn, pfile)
    if -1 == ret:
        printAndLogMsg("Pir value can not be set", pfile)
        return -1
    if False == ret:
        command = f"ifconfig nas0 up\n"
        output = executeCommandviaTelnet(tn, command, 3, r'root@ugwcpe:/#', 2, pfile)
        if -2 == output:
            return -1
        writeInLog(output[1], pfile)
    
    wanInfUp = False
    tryCount = 1
    while tryCount < 4 and False == wanInfUp:
        printAndLogMsg(f"Waiting for 60sec to get interface {wanInterfaceName} up", pfile)
        time.sleep(60)
        res = isInterfaceUp(wanInterfaceName, tn, pfile)
        if True == res:
            wanInfUp = True
        tryCount += 1
    
    if 4 == tryCount:
        printAndLogMsg(f"Interface {wanInterfaceName} is down", pfile)
        return -1
    
    """command = f"ping 8.8.8.8 -c 4\n"
    output = executeCommandviaTelnet(tn, command, 60, r'root@ugwcpe:/#', 2)
    writeInLog(output[1], pfile)"""

    command = f"qcfg --port -M shaper -i {wanInterfaceName} -s cb --pir 400\n"
    output = executeCommandviaTelnet(tn, command, 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        return -1
    writeInLog(output[1], pfile)

    command = f"qcfg --port -M shaper -i {wanInterfaceName} -s cb --pir 4000\n"
    output = executeCommandviaTelnet(tn, command, 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        return -1
    writeInLog(output[1], pfile)

    return 1

def fetchWakeupData(data):
    for line in data:
        if "Fill level after wakeup" in line:
            requiredData = line.strip()
            break
    reg = f".*TXIN-0:(.*),\s*TXIN-1:(.*),\s*Data SwQ.*"
    ma = re.match(reg, requiredData)
    print(f"group1 = {ma.group(1)}")
    print(f"group2 = {ma.group(2)}")
    TXIN0 = ma.group(1)
    TXIN1 = ma.group(2)
    dictTXIN0 = {}
    dictTXIN1 = {}
    #reg = ".*},\s*(.*):.*,\s*(\d+)},\s*(.*):.*,\s*(\d+)},\s*(.*):.*,\s*(\d+)},\s*(.*):.*,\s*(\d+)}"
    reg = ".*},\s*(.*):.*,\s*(\d+)},\s*(.*):.*,\s*(\d+)},\s*(.*):.*,\s*(\d+)},\s*(.*):.*,\s*(\d+)},\s*(.*):.*,\s*(\d+)},\s*(.*):.*,\s*(\d+)}"
    keyValue0 = re.match(reg, TXIN0)
    keyValue1 = re.match(reg, TXIN1)
    #\s*(.*):.*,\s*(\d+)}
    dictTXIN0[keyValue0.group(1)] = keyValue0.group(2)
    dictTXIN0[keyValue0.group(3)] = keyValue0.group(4)
    dictTXIN0[keyValue0.group(5)] = keyValue0.group(6)
    dictTXIN0[keyValue0.group(7)] = keyValue0.group(8)
    dictTXIN0[keyValue0.group(9)] = keyValue0.group(10)
    dictTXIN0[keyValue0.group(11)] = keyValue0.group(12)

    dictTXIN1[keyValue1.group(1)] = keyValue1.group(2)
    dictTXIN1[keyValue1.group(3)] = keyValue1.group(4)
    dictTXIN1[keyValue1.group(5)] = keyValue1.group(6)
    dictTXIN1[keyValue1.group(7)] = keyValue1.group(8)
    dictTXIN1[keyValue1.group(9)] = keyValue1.group(10)
    dictTXIN1[keyValue1.group(11)] = keyValue1.group(12)
    
    return dictTXIN0, dictTXIN1


def comaprefetchWakeupData(data1, data2):
    keys = data1.keys()
    rkeys = reversed(keys)
    for key in rkeys:
        if data1[key] != data2[key]:
            return key
    return 0

def IsSetlineratePresent(ser, pfile):
    command = "setlinerate.sh\n"
    output = executeCommandviaTelnet(ser, command, 2, 'root@ugwcpe:/#', 2, pfile)
    cmdOutput = output[1]
    notFound = False
    for line in cmdOutput:
        if "not found" in line.strip().lower():
            notFound = True
            msg = "setlinerate.sh is not present"
            return -1, msg
    if False == notFound:
        msg = "setlinerate is present"
        return 1, msg

def copyFileToDUT(filename, ser, pfile):
    tftpIp = input("Please set tftpd process and after that enter the ip of tftp server address: ")
    reg = "(\d{1,3}\.){3}\d{1,3}$"
    if not re.match(reg, tftpIp):
        msg = "ip address is incorrect"
        return -2, msg
    command = f"tftp -gr {filename} {tftpIp}\n"
    output = executeCommandviaTelnet(ser, command, 20, 'root@ugwcpe:/#', 2, pfile)
    cmdOutput = output[1]
    for line in cmdOutput:
        print(line)
        if "timeout" in line or "unreachable" in line.lower():
            msg = "Could not connect with tftp server"
            return -1, msg
    
        if "not found" in line.lower():
            msg = "File setlinerate.sh is not found in the server"
            return -1, msg
    msg = f"File {filename} copied"
    return 1, msg

def copySetlinerate(ser, pfile):
    res, msg = copyFileToDUT("setlinerate.sh", ser, pfile)
    print(f"{msg}")
    if 1 != res:
        return res, msg
    #msg = "setlinerate.sh is successfully copied to the CPE"
    command = "chmod +x setlinerate.sh\n"
    executeCommandviaTelnet(ser, command, 2, 'root@ugwcpe:/#', 1)
    command = "mv setlinerate.sh /opt/intel/usr/sbin/ \n"
    executeCommandviaTelnet(ser, command, 2, 'root@ugwcpe:/#', 1)
    res, msg = IsSetlineratePresent(ser, pfile)
    if -1 == res:
        msg = "setlinerate.sh cannot be copied for unspecified reason"
        return res, msg
    return 1, msg

def fetchSleepTime(data):
    retDict = {}
    for line in data:
        if "Sleep time" in line:
            #print(line)
            reg = "Sleep\stime\(ms\)=>\s*(.*):(\d+),\s*(.*):(\d+),\s*(.*):(\d+),\s*(.*):(\d+),\s*(.*):(\d+),\s*(.*):(\d+),\s*(.*):(\d+),\s*(.*):(\d+),\s*(.*):(\d+),\s*(.*):(\d+)"
            ma = re.search(reg, line)
            retDict[ma.group(1)] = int(ma.group(2))
            retDict[ma.group(3)] = int(ma.group(4))
            retDict[ma.group(5)] = int(ma.group(6))
            retDict[ma.group(7)] = int(ma.group(8))
            retDict[ma.group(9)] = int(ma.group(10))
            retDict[ma.group(11)] = int(ma.group(12))
            retDict[ma.group(13)] = int(ma.group(14))
            retDict[ma.group(15)] = int(ma.group(16))
            retDict[ma.group(17)] = int(ma.group(18))
            retDict[ma.group(19)] = int(ma.group(20))
            return retDict


def sleepTimeComparison(data1, data2):
    dict1 = fetchSleepTime(data1)
    dict2 = fetchSleepTime(data2)
    retDict = {}
    for key in dict1:
        retDict[key] = dict2[key] - dict1[key]
    return retDict
