@echo off

del *.scd
del *.scdm
del *.o

set working_path="."
set builds_path="Z:\projects\205_REL_5.1.0\builds\ugw6.1-grx350"

copy "%builds_path%"\binaries\wls\driver\*.scd "%working_path%"
copy "%builds_path%"\tools\mtlkroot\linux\mtlkroot.o "%working_path%"
copy "%builds_path%"\wireless\driver\linux\mtlk.o "%working_path%"

run-wireshark-logger.cmd 192.168.1.1 "%working_path%"
