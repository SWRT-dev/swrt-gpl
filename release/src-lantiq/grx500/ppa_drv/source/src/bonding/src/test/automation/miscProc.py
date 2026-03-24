import datetime
import xlsxwriter
import re
"""
    Arguments:
        None
    
    Return:
        Timestamp in specified format to write in logs
    
    Purpose:
        Get the current timestamp
"""
def getTimestamp():
    current_time = datetime.datetime.now()
    format_time = current_time.strftime("%Y")+'-'+current_time.strftime("%m")+'-'+current_time.strftime("%d")+' '+\
    current_time.strftime("%H")+':'+current_time.strftime("%M")+':'+current_time.strftime("%S")+'.'+current_time.strftime("%f")
    timeString ='[AUTO ' + format_time +' AUTO]'
    return timeString
    
"""
    Arguments:
        data:   data to write in logs, in list format
        pfile:  file pointer to the log file

    Returns:
        Nothing

    Purpose:
        Write the given data in the logs 

"""

def writeInLog(data, pfile):
    for line in data:
        line = line.replace('\n', '')
        line = line.replace('\r', '')
        pfile.write(f"{getTimestamp()} {line}\n")
    return

def printAndLogMsg(msg, pfile):
    print(msg)
    pfile.write(f"{getTimestamp()} {msg}\n")
    return 

def generateOutputExcel(dataToDump, pfile):
    """current_time = datetime.datetime.now()
    format_time = current_time.strftime("%Y")+'-'+current_time.strftime("%m")+'-'+current_time.strftime("%d")+'T'+\
    current_time.strftime("%H")+'-'+current_time.strftime("%M")+'-'+current_time.strftime("%Y")+'.'+current_time.strftime("%f")"""
    filename = "summary.xlsx"

    #create file (workbook) and worksheet
    try:
        outWorkbook = xlsxwriter.Workbook(filename)
    except Exception as e:
        pfile(f"{getTimestamp()} Error in creating excel file\n")
        return -1
    outSheet = outWorkbook.add_worksheet()

    #Setting format
    cell_format = outWorkbook.add_format({'text_wrap': True})
    cell_format.set_align('center')

    #Declare Headers
    outSheet.write("E6", "DSLAM", cell_format)
    outSheet.write("F6", "Expected Throughput [Mbit/s]", cell_format)
    outSheet.write("G6", "Actual Throughput [Mbit/s]", cell_format)
    outSheet.write("H6", "loss", cell_format)
    outSheet.write("I6", "Highest Fill level (0)", cell_format)
    outSheet.write("J6", "Highest Fill level (1)", cell_format)
    outSheet.write("K6", "Data Rate After Iperf (0)", cell_format)
    outSheet.write("L6", "Data Rate After Iperf (1)", cell_format)
    outSheet.write("M6", "(0,.5]", cell_format)
    outSheet.write("N6", "(.5,.75]", cell_format)
    outSheet.write("O6", "(.75,1]", cell_format)
    outSheet.write("P6", "(1,1.5]", cell_format)
    outSheet.write("Q6", "(1.5,2]", cell_format)
    outSheet.write("R6", "(2,2.5]", cell_format)
    outSheet.write("S6", "(2.5,3]", cell_format)
    outSheet.write("T6", "(3,5]", cell_format)
    outSheet.write("U6", "(5,10]", cell_format)
    outSheet.write("V6", ">10", cell_format)
   

    #Set width of column
    outSheet.set_column('E:E', 9.33)
    outSheet.set_column('F:F', 20.33)
    outSheet.set_column('G:G', 16.33)
    outSheet.set_column('H:H', 16.33)
    outSheet.set_column('I:I', 12.33)
    outSheet.set_column('J:J', 12.33)
    outSheet.set_column('K:K', 14.33)
    outSheet.set_column('L:L', 14.33)

    startWritingData = 6
    for index, data in enumerate(dataToDump["DSLAM"]):
        outSheet.write(startWritingData+index, 4, data, cell_format)

    for index, data in enumerate(dataToDump["Expected_Throughput"]):
        outSheet.write(startWritingData+index, 5, data, cell_format)

    for index, data in enumerate(dataToDump["Actual_Throughput"]):
        outSheet.write(startWritingData+index, 6, data, cell_format)

    for index, data in enumerate(dataToDump["Loss"]):
        outSheet.write(startWritingData+index, 7, data, cell_format)

    for index, data in enumerate(dataToDump["Highest_Fill_level_0"]):
        outSheet.write(startWritingData+index, 8, data, cell_format)

    for index, data in enumerate(dataToDump["Highest_Fill_level_1"]):
        outSheet.write(startWritingData+index, 9, data, cell_format)

    for index, data in enumerate(dataToDump["DataRateAfterIperf_line0"]):
        outSheet.write(startWritingData+index, 10, data, cell_format)
    
    for index, data in enumerate(dataToDump["DataRateAfterIperf_line1"]):
        outSheet.write(startWritingData+index, 11, data, cell_format)

    for index, data in enumerate(dataToDump["(0,.5]"]):
        outSheet.write(startWritingData+index, 12, data, cell_format)
    
    for index, data in enumerate(dataToDump["(.5,.75]"]):
        outSheet.write(startWritingData+index, 13, data, cell_format)
    
    for index, data in enumerate(dataToDump["(.75,1]"]):
        outSheet.write(startWritingData+index, 14, data, cell_format)
    
    for index, data in enumerate(dataToDump["(1,1.5]"]):
        outSheet.write(startWritingData+index, 15, data, cell_format)
    
    for index, data in enumerate(dataToDump["(1.5,2]"]):
        outSheet.write(startWritingData+index, 16, data, cell_format)
    
    for index, data in enumerate(dataToDump["(2,2.5]"]):
        outSheet.write(startWritingData+index, 17, data, cell_format)
    
    for index, data in enumerate(dataToDump["(2.5,3]"]):
        outSheet.write(startWritingData+index, 18, data, cell_format)
    
    for index, data in enumerate(dataToDump["(3,5]"]):
        outSheet.write(startWritingData+index, 19, data, cell_format)
    
    for index, data in enumerate(dataToDump["(5,10]"]):
        outSheet.write(startWritingData+index, 20, data, cell_format)
    
    for index, data in enumerate(dataToDump[">10"]):
        outSheet.write(startWritingData+index, 21, data, cell_format)

    outWorkbook.close()
    return 0

