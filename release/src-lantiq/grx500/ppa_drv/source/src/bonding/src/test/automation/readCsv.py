import csv
import re
from miscProc import *


"""
    Arguments:
        configurationFileName: full/absolute location of the configuration file to read data from
        pfile:                 file pointer to write logs
    
    Return:
        returns a dictionary which contain all the fetched data
        -1 if configurationFileName is invalid or some data are invalid
    
    Purpose:
        Read data from a csv file and validate those data
"""
def readConfigFile(configurationFileName, pfile):
    try:
        csv_file = open(configurationFileName,  mode='r', encoding='utf-8-sig')
    except OSError as e:
        printAndLogMsg(f"{configurationFileName} can not be opened, the program is terminated", pfile)
        return -1
    printAndLogMsg(f"Successfully open configuration file {configurationFileName}", pfile)
    csv_reader = csv.DictReader(csv_file)
    dict = {}
    
    dict["lineRateLine0"] = []
    dict["lineRateLine1"] = []
    dict["TXINBufferSizeLine0"] = []
    dict["TXINBufferSizeLine1"] = []
    dict["TxDelay_0"] = []
    dict["TxDelay_1"] = []
    dict["pingEndpoint"] = []
    dict["portRange"] = []
    dict["bandwidth"] = []
    dict["filename"] = []
    dict["Rate_Ratio_0"] = []
    dict["Rate_Ratio_1"] = []
    dict["SW_Shaper_%"] = []
    dict["iperfPath"] = []

    for rowNo, row in enumerate(csv_reader):
        
        try:
            dict["lineRateLine0"].append([row["US_Rate_0"], r'\s*^\d+\s*$', "lineRateLine0", "Desired line-rate of line-0, expressed in kbps"])
            dict["lineRateLine1"].append([row["US_Rate_1"], r'\s*^\d+\s*$', "lineRateLine1", "Desired line-rate of line-1, expressed in kbps"])
            dict["TXINBufferSizeLine0"].append([row["TXIN"], r'\s*^\d+\s*$', "TXINBufferSizeLine0", "TXIN buffer size line-0 expressed in number of ATM cells"])
            dict["TXINBufferSizeLine1"].append([row["TXIN"], r'\s*^\d+\s*$', "TXINBufferSizeLine1", "TXIN buffer size line-1 expressed in number of ATM cells"])
            dict["TxDelay_0"].append([row["TxDelay_0"], r'\s*[+-]?\d+\.?\d*\s*$', "TxDelay_0", "TxDelay for line-0, expressed in microseconds"])
            dict["TxDelay_1"].append([row["TxDelay_1"], r'\s*[+-]?\d+\.?\d*\s*$', "TxDelay_1", "TxDelay for line-1, expressed in microseconds"])
            #dict["TxDelayLine1"].append([row["p6"], r'\s*^\d+\s*$', "p6", "TxDelay for line-1, expressed in microseconds"])
            dict["pingEndpoint"].append([row["Ping_Endpoint"], r'.*', "pingEndpoint", "Host name or IP address of the ping endpoint"])
            dict["portRange"].append([row["Port_Range"], r'^\s*(\d+)\s*-\s*(\d+)\s*$', "portRange", "Port range"])
            dict["bandwidth"].append([row['SW_shaper_Rate'], r'^\s*\d+\s*$', "bandwidth", "bandwidth: #[KMG]"])
            dict["filename"].append([row["DSLAM"], r'.*', "filename", "First characters in the file name to identify"])
            dict["Rate_Ratio_0"].append([row["Rate_Ratio_0"], r'^\s*\d+\s*$', "Rate_Ratio_0", "Rate ratio of line 0 for echo command"])
            dict["Rate_Ratio_1"].append([row["Rate_Ratio_1"], r'^\s*\d+\s*$', "Rate_Ratio_1", "Rate ratio of line 0 for echo command"])
            dict["SW_Shaper_%"].append([row["SW_Shaper_%"], r'^\s*\d{1,2}\s*$', "SW_Shaper_%", "SW Shapper Percentage"])
            dict["iperfPath"].append([row["Iperf_Location"], r'.*', 'Iperf_Location', "location of iperf"])

        except KeyError as e:
            printAndLogMsg(f"{e}: Key not present in configuration file. Use comma(,) as a separator in line no {rowNo+1}", pfile)
            csv_file.close()
            return -1
        except Exception as e:
            printAndLogMsg(f"{e}: {type(e)} in line no {rowNo+1}", pfile)
            csv_file.close()
            return -1

    csv_file.close()
    #print(dict)
    for key in dict.keys():
        #print(key)
        #print(dict[key])
        for listNo, list in enumerate(dict[key]):
            value = list[0]
            regex = list[1]
        try:
            if not re.match(regex, value):
                printAndLogMsg(f"Error in {list[2]} value, it represents: {list[3]} in row no = {listNo+1}", pfile)
                return -1
        except Exception as e:
            printAndLogMsg(f"Error in {list[2]} value, it represents: {list[3]}", pfile)
            printAndLogMsg(f"Error type: {e}. Value retrived = {value} in row no = {listNo+1}", pfile)
            return -1
    dict["startingServerPort"] = []
    dict["noOfServerPort"] = []
    for list in dict["portRange"]:
        ports = list[0].split('-')
        if int(ports[0]) > int(ports[1]):
            printAndLogMsg(f"Port range must be in the format: <Starting Port Number>-<Ending Port Number>", pfile)
            return -1
        dict["startingServerPort"].append([int(ports[0])])
        dict["noOfServerPort"].append([int(ports[1]) - int(ports[0]) + 1])

    dict.pop("portRange")
    newData = {}
    for key in dict.keys():
        newData[key] = []
        for list in dict[key]:
            newData[key].append(list[0])
    return newData

def readUSRateChart(USRateChartFileName, pfile):
    try:
        csv_file = open(USRateChartFileName,  mode='r', encoding='utf-8-sig')
    except OSError as e:
        printAndLogMsg("{USRateChartFileName} can not be opened, the program is terminated", pfile)
        return -1
    printAndLogMsg(f"Successfully open configuration file {USRateChartFileName}", pfile)
    csv_reader = csv.DictReader(csv_file)
    USRateCharList = []
    for rowNo, row in enumerate(csv_reader):
        try:
            USRateCharList.append(row["Rate"])
        except KeyError as e:
            printAndLogMsg(f"{e}: Key not present in configuration file. Use comma(,) as a separator in line no {rowNo+1}", pfile)
            csv_file.close()
            return -1
        except Exception as e:
            printAndLogMsg(f"{e}: {type(e)} in line no {rowNo+1}", pfile)
            csv_file.close()
            return -1
    csv_file.close()
    regex = r"^\s*\d+\s*$"
    for index, value in enumerate(USRateCharList):
        if not re.match(regex, value): 
            printAndLogMsg(f"Value of US rate chart at line no {index+2} is invalid", pfile)
            return -1
    return USRateCharList