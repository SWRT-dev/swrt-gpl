import time
from telnetlib import Telnet
from miscProc import *

def executeCommandviaTelnet(tn, cmd, timeout, endPattern, readSwitch, pfile = ""):

    if 2 == readSwitch:
        #pfile.write(f"{getTimestamp()} Checking that the DUT is logging in as root or not\n")
        ret = telnetLoginToDUT(tn, pfile)
        if -1 == ret:
            printAndLogMsg("DUT login as a root user failed, existing the current configuration", pfile)
            #ser.close()
            return -2
        #pfile.write(f"{getTimestamp()} DUT is logged in as root\n")
        #printAndLogMsg(f"Executing command==> {cmd}", pfile)

    tn.write(cmd.encode('ascii'))
    # Begin:
    # Replaced:
    # output = tn.read_until(endPattern.encode('ascii'), timeout = timeout)
    # decodeOutput = output.decode('ascii')
    # With:
    starttime = int(round(time.time(),0))
    timeelapsed = 0
    decodeOutput = ''
    while timeelapsed < timeout:
        output = tn.read_until(endPattern.encode('ascii'), timeout = 1)
        output = output.decode('ascii')
        decodeOutput += output
        timeelapsed = int(round(time.time(),0)) - starttime 
        print( 'CPE[{0} out of {1}s elapsed]: {2}'.format(timeelapsed, timeout, output.strip()) )
        if endPattern in decodeOutput:
            break
    # End

    retlist = []
    if endPattern in decodeOutput:
        retlist.append(1)
    else:
        retlist.append(-1)
    if 1 == readSwitch:
        retlist.append(decodeOutput)
    elif 2 == readSwitch:
        retlist.append(decodeOutput.split('\n'))
    return retlist

def loginAsRoot(tn):
    cmd = 'sudo /bin/ash --login\n'
    executeCommandviaTelnet(tn, cmd, 3, 'Password', 1) # provide username
    cmd = 'admin\n'
    executeCommandviaTelnet(tn, cmd, 3, r'root@ugwcpe:/#', 1) # provide password
    cmd = '\n'
    output = executeCommandviaTelnet(tn, cmd, 3, r'root@ugwcpe:/#', 1)
    if r'root@ugwcpe:/#' in output[1]:
        return 1
    else:
        return -1

def loginAsAdmin(tn):
    cmd = 'admin\n'
    executeCommandviaTelnet(tn, cmd, 1, 'Password', 1) #provide username
    executeCommandviaTelnet(tn, cmd, 1, r'admin@ugwcpe:~$', 1) #provide password
    cmd = '\n'
    output = executeCommandviaTelnet(tn, cmd, 1, r'admin@ugwcpe:~$', 1)
    if r'admin@ugwcpe:~$' in output[1]:
        return 1
    else:
        return -1

def telnetLoginToDUT(tn, pfile):
    cmd = '\n'
    output = executeCommandviaTelnet(tn, cmd, 2, 'ugwcpe.intel.com login', 1)
    output[1] = output[1].strip()
    mOutput = output[1].replace('\n', f'\n{getTimestamp()}')
    pfile.write(f"{getTimestamp()} {mOutput}\n")
    loginSuccessfull = False
    if r'ugwcpe.intel.com login' in output[1]:
        res = loginAsAdmin(tn)
        if 1 == res:
            pfile.write(f"{getTimestamp()} Successfully login as user:admin\n")
        elif -1 == res:
            pfile.write(f"{getTimestamp()} user:admin login unsuccefull\n")

        res = loginAsRoot(tn)
        if 1 == res:
            pfile.write(f"{getTimestamp()} Successfully login as user:root\n")
            loginSuccessfull = True
        elif -1 == res:
            pfile.write(f"{getTimestamp()} user:root login unsuccefull\n")
            
    elif r'admin@ugwcpe:~$' in output[1]:
        pfile.write(f"{getTimestamp()} Already logged in as user:admin\n")
        res = loginAsRoot(tn)
        if 1 == res:
            pfile.write(f"{getTimestamp()} Successfully login as user:root\n")
            loginSuccessfull = True
        elif -1 == res:
            pfile.write(f"{getTimestamp()} user:root login unsuccefull\n")

    elif r'root@ugwcpe:/#' in output[1]:
        pfile.write(f"{getTimestamp()} Already logged in as user:root\n")
        loginSuccessfull = True
    else:
        pfile.write(f"{getTimestamp()} Unknown Promt: {output}\n")
    if False == loginSuccessfull:
        return -1
    else:
        return 1

def ConnectWithDUT(pfile):
    hostIP = '192.168.1.1'
    try:
        tn = Telnet(hostIP)
    except ConnectionRefusedError as err:
        printAndLogMsg(f"Could not connect with DUT with IP {hostIP}: {err}", pfile)
        return -1
    except Exception as err:
        printAndLogMsg(f"Could not connect with DUT with IP {hostIP}: {err}", pfile)
        return -1
    printAndLogMsg(f"Successfully open DUT with hostIP: {hostIP} via telnet", pfile)
    return tn
