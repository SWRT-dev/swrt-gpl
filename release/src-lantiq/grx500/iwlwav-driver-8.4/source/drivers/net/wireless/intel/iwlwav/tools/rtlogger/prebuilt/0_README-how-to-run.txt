Wavelog HOW-TO:
===============


RESTRICTIONS: We are supporting plugins for Wireshark 1.6 and 1.10 on Windows 32-bit system.
              Tested with the Wireshark versions 1.6.5, 1.6.7, 1.10.7 and 1.10.8.


 Step 1
--------

Copy wavelog plugin into Wireshark plugins folder

> 1_prepare-wireshark.bat



 Step 2
--------

Copy .SCD and .O files into prebuilt folder one-by-one or modify next script to automate.

> 2_copy-project-files.bat



 Step 3
--------

Run logger and Wireshark

> run-wireshark-logger.cmd 192.168.1.1 .

IP address 192.168.1.1 is for example and must be changed to proper board IP.
