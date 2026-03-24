Introduction
   To enable automation of atm-bonding throughput tests to be carried out in customer lab.
   Each test is identical, with different parameters.

Steps to run
   1. Build the automation_relayd executable to be run in the CPE:
        $> make -f Makefile.automation
   2. Tftp it into any location of the CPE and run it:
        $> ./automation_relayd
   3. Build the automation-stub executable to be run in Laptop-1 (Windows-10 LAN PC) of Lumen lab:
        pyinstaller --onefile automation-stub.py
   4. Copy it into Laptop-1 (LAN PC) of Lumen lab and run it after opening the Command Prompt (or Powershell)
        C:\Users\Foo> automation-stub.exe
	  NOTE:
	    1. It has a few options that can be inspected using --help|-h option
   5. Create the test-parameters CSV file, as well as the Upstream chart file
   6. Build the automation executable to be run in Laptop-2 (Windows-10 Master PC) of Lumen lab:
        pyinstaller --onefile automation.py
   7. Copy it into Laptop-2 (Master PC) of Lumen lab and run it after opening the Command Prompt (or Powershell)
        C:\Users\Bar> automation.exe \
		      -t <testparameterscsv> \
			  -u <Upstream chart texr file> \
			  -s <iperf-server-hostname> \
			  -w <CPE-wan-interface-name> \
			  -c <CPE-com-port-name>

Three output files get generated:
   1. summary.csv
   2. automation.log
   3. console.log
   The results of each test are specified in the summary.csv file
