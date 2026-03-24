import serial
from miscProc import getTimestamp

"""
    1 for read and return string
    2 for readline and return list

"""
def executeCommandviaSerialPort(ser, cmd, timeout, readSwitch):
    ser.timeout = timeout
    ser.write(cmd.encode('ascii'))
    if 1 == readSwitch:
        output = ser.read(900000)
        #print(output)
        output = output.decode('ascii')
    elif 2 == readSwitch:
        data = []
        line = ser.readline()
        data.append(line.decode('ascii'))
        while (r'root@ugwcpe:/#' not in line.decode('ascii')):
            line = ser.readline()
            #line = line.decode('ascii')
            data.append(line.decode('ascii'))
        output = data
        
    return output
    
def loginAsAdmin(ser):
    cmd = 'admin\n'
    executeCommandviaSerialPort(ser, cmd, 3, 1) # provide username
    executeCommandviaSerialPort(ser, cmd, 3, 1) # provide password
    cmd = '\n'
    output = executeCommandviaSerialPort(ser, cmd, 3, 1)
    if r'admin@ugwcpe' in output:
        return 1
    else:
        return -1
        
def loginAsRoot(ser):
    cmd = 'sudo /bin/ash --login\n'
    executeCommandviaSerialPort(ser, cmd, 3, 1) # provide username
    cmd = 'admin\n'
    executeCommandviaSerialPort(ser, cmd, 3, 1) # provide password
    cmd = '\n'
    output = executeCommandviaSerialPort(ser, cmd, 3, 1)
    if r'root@ugwcpe' in output:
        return 1
    else:
        return -1

def serialLoginToDUT(ser, pfile):
    cmd = '\n'
    #ser.timeout = 120
    output = executeCommandviaSerialPort(ser, cmd, 3, 1)
    loginSuccessfull = False
    """if r'Password' in output:
        cmd = 'admin\n'
        executeCommandviaSerialPort(ser, cmd, 1) # provide password
        cmd = '\n'
        output = executeCommandviaSerialPort(ser, cmd, 1)"""
        
    if r'ugwcpe.intel.com login' in output:
        res = loginAsAdmin(ser)
        if 1 == res:
            pfile.write(f"{getTimestamp()} Successfully login as user:admin\n")
        elif -1 == res:
            pfile.write(f"{getTimestamp()} user:admin login unsuccefull\n")

        res = loginAsRoot(ser)
        if 1 == res:
            pfile.write(f"{getTimestamp()} Successfully login as user:root\n")
            loginSuccessfull = True
        elif -1 == res:
            pfile.write(f"{getTimestamp()} user:root login unsuccefull\n")
            
    elif r'admin@ugwcpe' in output:
        pfile.write(f"{getTimestamp()} Already logged in as user:admin\n")
        res = loginAsRoot(ser)
        if 1 == res:
            pfile.write(f"{getTimestamp()} Successfully login as user:root\n")
            loginSuccessfull = True
        elif -1 == res:
            pfile.write(f"{getTimestamp()} user:root login unsuccefull\n")

    elif r'root@ugwcpe' in output:
        pfile.write(f"{getTimestamp()} Already logged in as user:root\n")
        loginSuccessfull = True
    if False == loginSuccessfull:
        return -1
    else:
        return 1

def openSerialPort(serial_port, baudRate, pfile):
    try:
        ser = serial.Serial(serial_port, baudRate)
    except serial.SerialException as e:
        error_string = f"Serial device at {serial_port} can not be opened or configured\n"
        pfile.write(f"{getTimestamp()} {error_string}\n")
        #print(f"{error_string}Exsiting the program")
        return -1
    except ValueError as e:
        error_string = f"Parameters are out of range for serial device at {serial_port}\n"
        pfile.write(f"{getTimestamp()} {error_string}\n")
        #print(f"{error_string}Exsiting the program")
        return -1
    pfile.write(f"{getTimestamp()} Successfully open serial device at {serial_port}\n")
    print(f"Successfully open serial device at {serial_port}")
    return ser