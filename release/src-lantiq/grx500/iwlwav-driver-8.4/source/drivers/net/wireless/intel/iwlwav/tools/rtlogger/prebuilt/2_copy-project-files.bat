@echo off

del *.scd
del *.scdm
del *.o

set working_path="."
set builds_path="..\..\..\builds"

for /d %%a in (%builds_path%\*) do (
	copy "%%a"\binaries\wls\driver\*.scd "%working_path%"
	copy "%%a"\tools\mtlkroot\linux\mtlkroot.o "%working_path%"
	copy "%%a"\wireless\driver\linux\mtlk.o "%working_path%"
)

rem run-wireshark-logger.cmd 192.168.1.1 "%working_path%"
