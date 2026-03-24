#-----------------------------------------------------------------------
# Entry file for automation to be run in Master-PC
#-----------------------------------------------------------------------
from datetime import date, time
import time as timeS
import os
import sys
import re
import math
import threading
import traceback
from iperfCli import *
from readCsv import *
from telnetCli import telnetLoginToDUT, ConnectWithDUT, executeCommandviaTelnet
from serialPro import openSerialPort, serialLoginToDUT
from pingTest import *
from miscProc import *
from runCPECommands import *
from commonStub import *
import threading
import getopt

from systemCheck import SystemCheck

exit_event = threading.Event()
def lineRatesComparable(actuallinerate, desiredlinerate, tolerance ):
    if actuallinerate is None:
        return False
    elif abs(int(actuallinerate) - int(desiredlinerate)) > tolerance:
        return False
    return True

def consoleLogDump(ser, pfile):
    
    print("Start dumping data into console log")
    
    while not exit_event.is_set():
        line = ser.readline()
        line = line.decode('ascii')
        pfile.write(f"{getTimestamp()} {line.strip()}\n")
        pfile.flush()
        
    
    print("Stop dumping data into console log")
    pfile.close()


def prepareFillLevel(data1, data2):
    retdata = []
    firstTXIN0, firstTXIN1 = fetchWakeupData(data1)
    secondTXIN0, secondTXIN1 = fetchWakeupData(data2)
    diff0 = comaprefetchWakeupData(firstTXIN0, secondTXIN0)
    diff1 = comaprefetchWakeupData(firstTXIN1, secondTXIN1)
    retdata.append(diff0)
    retdata.append(diff1)
    return retdata

def runFullTest(inputArgs, iperfVersion, protocol, wanInterfaceName, iperfServerAddr, USRateChartList, lineratetolerance, setlineratedelta, serverSock, platformType, keys, pfile):
    
    #Configuration file parameters
    testName = inputArgs['filename']
    intervalInSecs = r'1'
    lineRateLine0 = int(inputArgs["lineRateLine0"])
    lineRateLine1 = int(inputArgs["lineRateLine1"])
    TXINBufferSizeLine0 = inputArgs["TXINBufferSizeLine0"]
    TXINBufferSizeLine1 = inputArgs["TXINBufferSizeLine1"]
    TxDelay_0 = inputArgs["TxDelay_0"]
    TxDelay_1 = inputArgs["TxDelay_1"]
    Rate_Ratio_0 = inputArgs["Rate_Ratio_0"]
    Rate_Ratio_1 = inputArgs["Rate_Ratio_1"]
    SW_Shaper_Per = inputArgs["SW_Shaper_%"] 
    pingCount = "6"
    pingBufLen = "56"
    pingEndpoint = inputArgs["pingEndpoint"]
    transmitSecs = "60"
    bufLen = "1400"
    bandwidth = inputArgs["bandwidth"]
    startingServerPort = inputArgs["startingServerPort"]
    noOfServerPort = inputArgs["noOfServerPort"]
    iperfPath = inputArgs["iperfPath"].strip()


    #Setting up data structure to collect data to be printed in excel
    retDataDict = {}
    for key in keys:
        if key == "Expected_Throughput":
            retDataDict[key] = float(bandwidth)*0.85/1000
        elif key == 'DSLAM':
            retDataDict[key] = inputArgs['filename']
        else:
            retDataDict[key] = "n/a"

    #Login to DUT via serial port
    #ser = openSerialPort(serial_port, baudRate, pfile) #serial
    ser = ConnectWithDUT(pfile) #telnet
    if -1 == ser:
        printAndLogMsg(f"Error in opening the telnet connection with DUT, exsiting the current configuration", pfile)
        return -1, retDataDict
    
    ret = telnetLoginToDUT(ser, pfile)
    if -1 == ret:
        printAndLogMsg("DUT login as a root user failed, existing the current configuration", pfile)
        ser.close()
        return -1, retDataDict
    else:
        print(f"Login to DUT as a root user")

    #=================================================== Setting line rates ====================================================#

    setlineratesuccess = False
    for tryCount in range(3):

        # Get line rates from CPE
        output = executeCommandviaTelnet(ser, 'dsl_cpe_pipe g997csg -1 0 0\n', 5, r'root@ugwcpe:/#', 2, pfile)
        if -2 == output:
            printAndLogMsg(f"Failed to run dsl g997csg -1 0 0 commsnd in CPE", pfile)
            ser.close()
            return -1, retDataDict

        #print('QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ:{0} => [{1}, {2}]:'.format(output[1], actuallinerate0, actuallinerate1))
        writeInLog(output[1], pfile)
        actuallinerate0, actuallinerate1 = fetchActualDataRate(output[1])
        actuallinerates = [ actuallinerate0, actuallinerate1 ]

        # Acceptable ?
        setlinerates = [False, False]
        if lineRatesComparable(actuallinerates[0], lineRateLine0, lineratetolerance) is False:
            setlinerates[0] = True
        if lineRatesComparable(actuallinerates[1], lineRateLine1, lineratetolerance) is False:
            setlinerates[1] = True
        if not setlinerates[0] and not setlinerates[1]:
            setlineratesuccess = True
            break;

        desiredlinerates = [lineRateLine0, lineRateLine1]
        for lineidx in range(2):

            if setlinerates[lineidx]:

                print( "Need to configure line rate of line-{0} since its actual rate ({1} kbps) is not close enough to the desired: {2} kbps".format( lineidx, actuallinerates[lineidx], desiredlinerates[lineidx] ) )

                # Try getting the parameter to be specified to setlinerate.sh by consulting US rate chart
                # If not found, set it to desired-value + setlineratedelta
                setlinerates[lineidx] = desiredlinerates[lineidx] + setlineratedelta
                try:

                    USindex = USRateChartList.index(str(desiredlinerates[lineidx]))

                    if USindex < len(USRateChartList)-1:
                        setlinerates[lineidx] = USRateChartList[USindex+1]
                    else:
                        printAndLogMsg( "Desired line rate of line-{0} = {1}kbps is the last element in US line rate chart. Going to setlinerate.sh with {2} instead".format(lineidx, desiredlinerates[lineidx], setlinerates[lineidx]), pfile)

                except ValueError as e:
                    printAndLogMsg( "Desired line rate of line-{0} = {1} kbps is not present in US line rate chart. Going to setlinerate.sh with {2} instead".format(lineidx, desiredlinerates[lineidx], setlinerates[lineidx]), pfile)
            else:
                setlinerates[lineidx] = None

        # Set line rates
        retList = setLineRates(setlinerates, ser, pfile)
        ser = retList[1]

    # End retry loop
                
    if setlineratesuccess is not True:
        retDataDict['Actual_Throughput'] = "line rates could not be set"
        return -1, retDataDict

    print( "Desired line rates ({2}, {3}) kbps acceptable against the actual run-rates ({0}, {1}) kbps required for test {4}. Continuing to next step...".format( actuallinerates[0], actuallinerates[1], lineRateLine0, lineRateLine1, testName))

    #============================================ End Up Setting line rate ==============================================#
    
    #======================================= Start finding and setting pir value ============================================#
    cmd = "cat /sys/kernel/debug/tmu/queue1\n"
    output = executeCommandviaTelnet(ser, cmd, 7, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        ser.close()
        return -1, retDataDict
    writeInLog(output[1], pfile)
    pirValue = findPirValue(output[1])
    if -1 == pirValue:
        printAndLogMsg("pir value count not be found, please enable QoS first", pfile)
        return -1, retDataDict
    print( "Pir value found to be {0}".format(pirValue))
    pirTryCount = 1
    while float(pirValue) < 500000 and pirTryCount <=3:
        printAndLogMsg( "Trying to set pir value to desired 500000, {0} out of 3 times".format(pirTryCount), pfile)
        ret = setPirValue(ser, wanInterfaceName, pfile)
        if -1 == ret:
            return -1, retDataDict
        output = executeCommandviaTelnet(ser, cmd, 7, r'root@ugwcpe:/#', 2, pfile)
        if -2 == output:
            ser.close()
            return -1, retDataDict
        writeInLog(output[1], pfile)
        pirValue = findPirValue(output[1])
        if -1 == pirValue:
            printAndLogMsg("pir value count not be found", pfile)
        printAndLogMsg(f"Pir value found = {pirValue}", pfile)
        pirTryCount += 1

    print( "Pir value is found to be at desired 500000. Continuing to next step" )
    #======================================================= End finding and setting pir value ============================================#


    #=========================================Fetch bonding driver TxDelay per line ==============================================#
    command5 = f"cat /sys/kernel/debug/ima_bond/tx\n"
    output = executeCommandviaTelnet(ser, "cat /sys/kernel/debug/ima_bond/tx\n", 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        ser.close()
        return -1, retDataDict
    writeInLog(output[1], pfile)
    actualTxDelayLine0, actualTxDelayLine1 = fetchActualTXDelay(output[1])
    if float(TxDelay_0) < 0:
        TxDelay_0 = float(actualTxDelayLine0) / 1000
    if float(TxDelay_1) < 0:
        TxDelay_1 = float(actualTxDelayLine1) / 1000

    print( "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ: ({0}, {1})".format(TxDelay_0, TxDelay_1 ))
    
    #=========================================Start Configuring bonding driver and fill level==============================================#
    
    delay_line0 = float(TxDelay_0)
    delay_line1 = float(TxDelay_1)
    TxDelayLine0 = math.floor(delay_line0*1000)
    TxDelayLine1 = math.floor(delay_line1*1000)

    intRateRatio0 = int(Rate_Ratio_0) * 1000
    intRateRatio1 = int(Rate_Ratio_1) * 1000

    fillLevelValues = [4, 8, 12, 4, 8, 12]
    shapperOffset = math.floor(float(lineRateLine0) * delay_line0 / (53*8))
    fillLevelValues[0:3] = [x+shapperOffset for x in fillLevelValues[0:3]]
    shapperOffset = math.floor(float(lineRateLine1) * delay_line1 / (53*8))
    fillLevelValues[3:6] = [x+shapperOffset for x in fillLevelValues[3:6]]
    

    command4 = f"echo 2 {TXINBufferSizeLine0} {TXINBufferSizeLine1} {TxDelayLine0} {TxDelayLine1} {intRateRatio0} {intRateRatio1} {SW_Shaper_Per} 16 > /sys/kernel/debug/ima_bond/tx\n"
    #Before running Every command checking that DUT is logged in as root or not
    output = executeCommandviaTelnet(ser, command4, 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        ser.close()
        return -1, retDataDict
    writeInLog(output[1], pfile)

    command5 = f"cat /sys/kernel/debug/ima_bond/tx\n"
    output = executeCommandviaTelnet(ser, command5, 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        ser.close()
        return -1, retDataDict
    writeInLog(output[1], pfile)
    swq = fetchSWShaperRate(output[1], pfile)
    if -1 == swq:
        return -1, retDataDict
    elif len(swq) != 3:
        printAndLogMsg("Software shaper rate is not fetched successfully" , pfile)
        return -1, retDataDict
    fillLevelValues.extend(swq)

    command6 = f"echo fill-level {fillLevelValues[0]} {fillLevelValues[1]} {fillLevelValues[2]} {fillLevelValues[3]} {fillLevelValues[4]} {fillLevelValues[5]} {fillLevelValues[6]} {fillLevelValues[7]} {fillLevelValues[8]} > /sys/kernel/debug/ima_bond/tx\n"
    output = executeCommandviaTelnet(ser, command6, 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        ser.close()
        return -1, retDataDict
    writeInLog(output[1], pfile)
    
    command7= f"cat /sys/kernel/debug/ima_bond/tx\n"
    output = executeCommandviaTelnet(ser, command7, 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        ser.close()
        return -1, retDataDict
    writeInLog(output[1], pfile)
    #=========================================End up configuring bonding driver and fill level==============================================#

    #Close serial line
    #ser.close()
    # Run ping test after running command in DUT
    print( "Testing ping now" )
    if 'linux' == platformType:
        pingArgs = ['ping', pingEndpoint, '-c', pingCount, '-s', pingBufLen]
        regexrSuccessfulPingOutput = f'\d+ bytes from .*: icmp_seq=\d+ ttl=\d+ time(=|>|<)(\d+\.?\d*) ms'
    elif 'windows' == platformType:
        pingArgs = ['ping', pingEndpoint, '-n', pingCount, '-l', pingBufLen]
        regexrSuccessfulPingOutput = f'Reply from .*: bytes=\d+ .*time(=|>|<)(\d+\.?\d*)ms TTL=\d+'
    else:
        pfile.write(f"{getTimestamp()} Exisitng the current configuration\n")
        printAndLogMsg(f"OS Type is not supported for ping: {platformType}", pfile)
    pingCmd = " "
    pingCmd = pingCmd.join(pingArgs)
    
    pingTryCount = 1
    pingSuccess = False
    while pingTryCount < 4 and False == pingSuccess:
        printAndLogMsg(f"Trying ping {pingTryCount} time out of 3", pfile)
        #Run ping
        printAndLogMsg(pingCmd, pfile)
        res, output = executeCommandInClient(serverSock, pingCmd)
        #if -1 == pingOutput:
        #if '-2' == res
        if '0' == res:
            printAndLogMsg(f"Ping Failed: {output}", pfile)
        else:
            pingOutput = output
            #pingOutput = output.split('\n')
            writeInLog(pingOutput, pfile)
            for line in pingOutput:
                print(line)

            successRatio, avgRTT = fetchImpPingDetails(pingOutput, pingCount, regexrSuccessfulPingOutput, pfile)
            if -1 == avgRTT:
                printAndLogMsg(f"Ping Failed, destination is not reachable, success ratio = {successRatio}", pfile)
                printAndLogMsg(f"Wait 60 seconds before try to ping again", pfile)
                timeS.sleep(60)
                #retDataDict["pingLoss%"] = (1-successRatio)*100
            else:
                #retDataDict["pingRTT"] = avgRTT
                #retDataDict["pingLoss%"] = (1-successRatio)*100
                printAndLogMsg(f"Ping Successful average RTT = {avgRTT}", pfile)
                pingSuccess = True
        pingTryCount += 1

    # return if all the three times ping is unsuccessful
    if False == pingSuccess:
        retDataDict["Expected_Throughput"] = "ping-failed"
        retDataDict["Loss"] = "ping-failed"
        return -1, retDataDict

    print( "Successfully pinged" )

    #Run iperf
    print(f"\nTrying iperf with server addres {iperfServerAddr}")
    pfile.write(f"{getTimestamp()} Trying iperf with server addres {iperfServerAddr}\n")
    mbandwidth = f"{round(float(bandwidth)*0.9)}k"
    if protocol.upper() == 'UDP':
        args = [iperfPath,'-c', iperfServerAddr, '-u', '-b', mbandwidth, '-l', bufLen, '-i', intervalInSecs, '-t', transmitSecs, '--get-server-output']
    elif protocol.upper() == 'TCP':
        args = [iperfPath,'-c', iperfServerAddr, '-b', mbandwidth, '-l', bufLen, '-i', intervalInSecs, '-t', transmitSecs, '--get-server-output']
    else:
        pfile.write(f"{getTimestamp()} Wrong Protocol Type\n")
        pfile.write(f"{getTimestamp()} Exiting the program\n")
        return -1, retDataDict
    if '2' == version:
        args.pop()

    iperfExecutionStatus, iperfOutputList = run_iperf(args, startingServerPort, noOfServerPort, serverSock, pfile)
    if 1 == iperfExecutionStatus:
        iperfOutput = iperfOutputList[0]
        bondingStat1 = iperfOutputList[1]
        bondingStat2 = iperfOutputList[2]
        highestFillLevel = prepareFillLevel(bondingStat1, bondingStat2)

        #writeInLog(iperfOutput, pfile)
        miperfOutput = collectIperfOutput(iperfOutput, protocol, iperfVersion, pfile)
        miperfOutput.pop(0)

        ret = findAverage(miperfOutput, 2, pfile, protocol)
        if -1 == ret:
            printAndLogMsg("Error in finding average", pfile)
            pfile.write(f"{getTimestamp()} Exsiting the program\n")
            return -1, retDataDict
        else:
            printAndLogMsg(f"Average bandwidth = {ret}", pfile)
        retDataDict["Actual_Throughput"] = ret
        retDataDict["Loss"] = findLossDatagram(iperfOutput)
        retDataDict["Highest_Fill_level_0"] = highestFillLevel[0]
        retDataDict["Highest_Fill_level_1"] = highestFillLevel[1]
    else:
        printAndLogMsg("iperf execution is not successful",pfile)
        retDataDict["Actual_Throughput"] = iperfOutputList
        retDataDict["Loss"] = iperfOutputList
        retDataDict["Highest_Fill_level_0"] = iperfOutputList
        retDataDict["Highest_Fill_level_1"] = iperfOutputList

    cmd = 'dsl_cpe_pipe g997csg -1 0 0\n'
    output = executeCommandviaTelnet(ser, cmd, 5, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        ser.close()
        return -1, retDataDict
    print(output[1])
    writeInLog(output[1], pfile)
    actuallinerate0, actuallinerate1 = fetchActualDataRate(output[1])
    retDataDict["DataRateAfterIperf_line0"] = actuallinerate0
    retDataDict["DataRateAfterIperf_line1"] = actuallinerate1
    if 1 == iperfExecutionStatus:
        sleepTimeData = sleepTimeComparison(bondingStat1, bondingStat2)
        for key in sleepTimeData.keys():
            retDataDict[key] = sleepTimeData[key]
    command5 = f"cat /sys/kernel/debug/ima_bond/tx\n"
    output = executeCommandviaTelnet(ser, command5, 3, r'root@ugwcpe:/#', 2, pfile)
    if -2 == output:
        ser.close()
        return -1, retDataDict
    writeInLog(output[1], pfile)
    ser.close()
    return 0, retDataDict

def showhelp(progname):
    print("Usage: {0} options".format(progname))
    print("       {0} -h|--help".format(progname))
    print(" Options:")
    print("  -t|--testfile <csvpathname>      CSV file pathname containing test definitions. Must be specified")
    print("  -u|--uschartfile <txtpathname>   text file pathname containing US chart definitions. Must be specified")
    print("  -s|--iperf-server <IP address|hostname>  IPerf server ipaddr/hostname. Must be specified")
    print("  -w|--wan-interface <wan-interface-name>  Name of the CPE WAN interface connected to DSLAM. Must be specified")
    print("  -c|--comport <Serial-port-name>  Name of the serial port connected to CPE. Must be specified")
    print("  -r|--relay-server  <IP address>  IP address of the relay-server. If not specified, {0} uses 192.168.1.1".format(progname))
    print("  -l|--linerate-comparison-tolerance <number>  Acceptable line rate closeness limit, in kbps.")
    print("                                   If not specified, {0} uses 3 kbps".format(progname)) 
    print("  -d|--setlinerate-delta <number>  Extra kbps for setlinerate.sh to work, in case no matching entry found in US Chart")
    print("                                   If not specified, {0} uses 4 kbps".format(progname)) 
    print("  -v|--iperf-version 2|3           IPerf client version. If not specified, {0} uses 3".format(progname))
    print("  -p|--iperf-protocol UDP|TCP      IPerf traffic protocol. If not specified, {0} uses UDP".format(progname))
    print("  -h|--help                        print this message and quit")

if __name__ == '__main__':
    
    options = {
        "iperf-version":'3', # argv[1]
        "iperf-protocol": 'UDP', #  argv[2]
        "iperf-server": 'ping.online.net', #  argv[4]
        "cpe-address":'192.168.1.1',
        "linerate-comparison-tolerance": 3,
        "setlinerate-sh-delta": 4,
        'end-of-options': None
    }

    try:
        opts, args = getopt.getopt(sys.argv[1:],"c:d:hl:p:r:s:t:u:v:w:",
                            [
                                "comport=",
                                "setlinerate-delta=",
                                "help",
                                "linerate-comparison-tolerance=",
                                "iperf-protocol=",
                                "relay-server=",
                                "iperf-server=",
                                "testfile=",
                                "uschartfile=",
                                "iperf-version=",
                                "wan-interface="
                            ])
    except getopt.GetoptError:
        showhelp(sys.argv[0])
        sys.exit(0)

    for opt, arg in opts:
        if opt == '-c':
            options['comport'] = arg
        elif opt == '-d':
            options['setlinerate-sh-delta'] = int(arg)
        elif opt == '-h':
            showhelp(sys.argv[0])
            sys.exit(0)
        elif opt == '-l':
            options['linerate-comparison-tolerance'] = int(arg)
        elif opt == '-p':
            options['iperf-protocol'] = arg.upper()
        elif opt == '-r':
            options['cpe-address'] = arg
        elif opt == '-s':
            options['iperf-server'] = arg
        elif opt == '-t':
            options['testfilename'] = arg
        elif opt == '-u':
            options['usratefilename'] = arg
        elif opt == '-v':
            options['iperf-version'] = arg
        elif opt == '-w':
            options['wanifname'] = arg
        else:
            print( "unknown option: {0}".format(opt))
            showhelp(sys.argv[0])
            sys.exit(0)

    if "testfilename" not in options: # argv[5]
        print( "-t option must be specified")
        showhelp(sys.argv[0])
        sys.exit(-1)

    if "usratefilename" not in options: # argv[6]
        print( "-u option must be specified")
        showhelp(sys.argv[0])
        sys.exit(-1)

    if "wanifname" not in options: # argv[3]
        print( "-w option must be specified")
        showhelp(sys.argv[0])
        sys.exit(-1)

    if "comport" not in options: # argv[7]
        print( "-c option must be specified")
        showhelp(sys.argv[0])
        sys.exit(-1)

    #Check iperf version
    if '2' == options['iperf-version']:
        iperf = 'iperf'
    elif '3' == options['iperf-version']:
        iperf = 'iperf3'
    else:
        print("Wrong iperf version ({0}), it must be either 2 or 3".format(options('iperf-version')))
        showhelp(sys.argv[0])
        sys.exit()

    version = options['iperf-version']
    serial_port = options[ 'comport' ]

    #Check protocol used
    if 'TCP' != options['iperf-protocol'] and 'UDP' != options['iperf-protocol']:
        print("Wrong protocol specified: {0}".format( options['iperf-protocol']))
        showhelp(sys.argv[0])
        sys.exit()

    protocol = options['iperf-protocol']

    if '2' == options['iperf-version'] and 'UDP' == protocol:
        print("Support of UDP with iperf2 is not provided")
        print("Please use only TCP with iperf2")
        print("TCP/UDP can be used with iperf3")
        sys.exit()

    configurationFileName = options['testfilename']
    USRateChartFileName = options['usratefilename']
        
    # Check health of the remaining system
    SystemCheck.check_as_master(server=options['cpe-address'])

    #Open automation and console log file
    autofilename = 'Automation.log'
    consolefilename = 'Console.log'
    summaryfile = 'Summary.csv'
    baudRate = 115200
    filenames = [autofilename, consolefilename, summaryfile]
    for filename in filenames:
        isFile = os.path.isfile(filename)
        if isFile:
            ans = input(f"Do you want to modify {filename} file(Y/N): ")
            if ans.lower() == 'n':
                sys.exit()

    #Open automation file
    try:
        pAutofile = open(autofilename, 'w')
    except OSError as e:
        print("File can not be opened, the program is terminated")
        sys.exit()
    print(f"Successfully open {autofilename} for automation log")

    #connect with DUT via serial port
    ser = openSerialPort(serial_port, baudRate, pAutofile)
    if -1 == ser:
        printAndLogMsg("Error in opening the serial port, exiting the current configuration", pAutofile)
        pAutofile.close()
        sys.exit()
    
    #login to DUT as a root user
    ret = serialLoginToDUT(ser, pAutofile)
    if -1 == ret:
        printAndLogMsg("DUT login as a root user failed, exiting the current configuration", pAutofile)
        pAutofile.close()
        ser.close()
    else:
        print(f"Login to DUT as a root user")
    
    #Read configuration parameter
    inputParams = readConfigFile(configurationFileName, pAutofile)
    if -1 == inputParams:
        printAndLogMsg("Exiting the program as configurations are wrong", pAutofile)
        pAutofile.close()
        sys.exit()
    
    #Read US data rate list
    USRateChartList = readUSRateChart(USRateChartFileName, pAutofile)
    if -1 == USRateChartList:
        printAndLogMsg("Exiting the program as US rate chart contains incorrect value", pAutofile)
        pAutofile.close()
        sys.exit()
    #pAutofile.write(f"{getTimestamp()} The US rate chat:\n")
    #writeInLog(USRateChartList, pAutofile)

    port = 48090
    serverSock, msg = connectWithServer(options['cpe-address'], port)
    printAndLogMsg(msg, pAutofile)
    if -1 == serverSock:
        sys.exit()

    res, output = executeCommandInClient(serverSock, "uname -r")
    if '0' == res:
        platformType = 'windows'
    else:
        platformType = 'linux'
    printAndLogMsg(f"The lan PC is a {platformType} pc", pAutofile)

    #Open file for console output
    try:
        pConsolefile = open(consolefilename, 'w')
    except OSError as e:
        print("File can not be opened, the program is terminated")
        sys.exit()
    print(f"Successfully open {consolefilename} for automation log")

    #Creating thread to dump console log
    th = threading.Thread(target=consoleLogDump, args=[ser, pConsolefile])
    th.start()

    #Open summary file
    try:
        pSummaryfile = open(summaryfile, 'w')
    except OSError as e:
        print("File can not be opened, the program is terminated")
        sys.exit()
    print(f"Successfully open {summaryfile} for automation log")


    # ====================================== Start Executing the test =========================================== #

    
    #Prepare Data structure to print in excel
    inputParamsDuplicate = {}
    keys = ["DSLAM", 'Expected_Throughput', "Actual_Throughput", "Loss", "Highest_Fill_level_0", "Highest_Fill_level_1", \
        "DataRateAfterIperf_line0", "DataRateAfterIperf_line1", "(0,.5]", "(.5,.75]", "(.75,1]", "(1,1.5]", "(1.5,2]", \
            "(2,2.5]", "(2.5,3]", "(3,5]", "(5,10]", ">10"]
    
    #Write header in csv 
    csvSeparator = ","
    mkeys = [f"\"{x}\""for x in keys]
    csvHeader = csvSeparator.join(mkeys)
    pSummaryfile.write(f"{csvHeader}\n")

    
    print(f"Successfully read all the configuration parameter from {configurationFileName}")
    #pAutofile.close()
    oneInputParama = {}

    #open telnet connection to DUT
    ser = ConnectWithDUT(pAutofile) #telnet
    if -1 == ser:
        printAndLogMsg(f"Error in opening the telnet connection with DUT, exsiting the current configuration", pAutofile)
        exit_event.set()
        sys.exit()
    
    ret = telnetLoginToDUT(ser, pAutofile)
    if -1 == ret:
        printAndLogMsg("DUT login as a root user failed, existing the current configuration", pAutofile)
        exit_event.set()
        ser.close()
        sys.exit()
    else:
        print(f"Login to DUT as a root user")

    #check existence of setlinerate.sh if not present then copy it
    ret, msg = IsSetlineratePresent(ser, pAutofile)
    printAndLogMsg(msg, pAutofile)
    if -1 == ret:
        print("setlinerate.sh is not present, coping setlinerate.sh")
        ret1, msg1 = copySetlinerate(ser, pAutofile)
        printAndLogMsg(msg1, pAutofile)
        if 1 != ret1:
            printAndLogMsg("Existing the program", pAutofile)
            executeCommandInClient(serverSock, "exit")
            exit_event.set()
            ser.close()
            sys.exit()
    ser.close()

    print( 'All is well' )

    for index in range(len(inputParams["noOfServerPort"])):

        for retries in range(10):

            for key in inputParams.keys():
                oneInputParama[key] = inputParams[key][index]
            DSLAM = oneInputParama["filename"]

            printAndLogMsg(f"================================================================================", pAutofile)
            if retries > 0:
                printAndLogMsg(f"===========================Test {DSLAM} RESTART===================================", pAutofile)
            else:
                printAndLogMsg(f"===========================Test {DSLAM} START===================================", pAutofile)
            printAndLogMsg(f"================================================================================", pAutofile)
            try:
                ret, dataToPrintInExcel = runFullTest(oneInputParama, options['iperf-version'],
                                            options['iperf-protocol'], options['wanifname'],
                                            options['iperf-server'], USRateChartList,
                                            options['linerate-comparison-tolerance'],
                                            options['setlinerate-sh-delta'],
                                            serverSock, platformType, keys, pAutofile)
            except Exception as e:
                print( "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE")
                printAndLogMsg(f"================= Test with DSLAM value {DSLAM} Fails for Automation failure: {e} =================\n\n", pAutofile)
                dataToPrintInExcel = {}
                dataToPrintInExcel["DSLAM"] = DSLAM
                for key in keys:
                    if key != "DSLAM":
                        dataToPrintInExcel[key] = "Automation failure"

                print( "Sleeping for 10 seconds for system to stabilize" )
                timeS.sleep(10)

                # Check health of the remaining system
                try:
                    serverSock.close()
                except:
                    pass
                timeS.sleep(2)
                SystemCheck.check_as_master(server=options['cpe-address'])
           
                #trace = traceback.format_exc(limit=2)
                #for line in trace:
                    #printAndLogMsg(trace, pAutofile)

                print( "Reconnecting to relay server" )
                try:
                    serverSock.close()
                except:
                    pass
                serverSock, msg = connectWithServer(options['cpe-address'], port)
                printAndLogMsg(msg, pAutofile)
                if -1 == serverSock:
                    sys.exit()

            else:
                print( "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy (ret = {0}): {1}".format(ret,dataToPrintInExcel))
                if -1 == ret:
                    printAndLogMsg(f"================= Test with DSLAM value {DSLAM} Fails =================\n\n", pAutofile)
                elif 0 == ret:
                    printAndLogMsg(f"================= Test with DSLAM value {DSLAM} Succeeded =================\n\n", pAutofile)
                break

        #dataToPrintInExcel["DSLAM"] = DSLAM
        values = dataToPrintInExcel.values()
        valuelist = list(values)
        valuelist1 = [f"\"{x}\"" for x in valuelist]
        csvData = csvSeparator.join(valuelist1)
        pSummaryfile.write(f"{csvData}\n")
        pSummaryfile.flush()
        
    #executeCommandInClient(serverSock, "exit")
    #generateOutputExcel(inputParamsDuplicate, pAutofile)
    pSummaryfile.close()
    exit_event.set()
    pAutofile.close()
