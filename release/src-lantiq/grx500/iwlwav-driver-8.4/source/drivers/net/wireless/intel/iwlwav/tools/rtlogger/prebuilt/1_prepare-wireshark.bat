@echo off

set WIRESHARK_DIR="C:\Program Files\Wireshark"

if not exist %WIRESHARK_DIR% (
   echo . The Wireshark directory %WIRESHARK_DIR% does not exist.
   goto end
)

:: Remove obsolete plugin
FOR /D %%A IN (%WIRESHARK_DIR%\plugins\*)  DO del "%%A\wavelog.dll" 2>DEL
if not "%APPDATA%" == "" (
    del "%APPDATA%\Wireshark\plugins\wavelog.dll" 2>DEL
)

:: Copy new plugin to the wireshark plugins directory
FOR /D %%A IN (%WIRESHARK_DIR%\plugins\1.6.*)  DO copy wavelog-1.6.dll "%%A"
FOR /D %%A IN (%WIRESHARK_DIR%\plugins\1.1?.*) DO copy wavelog-1.10.dll "%%A"

:end
