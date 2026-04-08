@ECHO OFF

REM Remote build script name
SET REMOTE_BUILD_SCRIPT=mkproj
REM The Windows path where the remote build script will launch build command
SET REMOTE_PATH=%~dp0
REM Build command to be launched by remote build script
SET REMOTE_CMD=
REM Remote user to login thu SSH
SET REMOTE_USER=%WBLD_USER%
REM SSH key file
SET SSH_KEY_FILE=%WBLD_KEY%
REM SSH key file
SET BUILD_SCRIPT_PARAMS=

REM The Windows path of home folder on remote build server
REM WARNING: we presume the drive letter (network drive root) points to home folder on remote build server
SET SCRIPTS_HOME=%~d0
REM The Windows path where mtlkwin.bat and remote build script are placed
SET SCRIPTS_PATH=%~dp0
REM Remote build server network name/IP
SET REMOTE_ADDR=
REM Unique Remote build script name - used during this concrete run
SET UNIQUE_BUILD_SCRIPT_COPY_NAME=

IF [%REMOTE_USER%] == [] (
    REM we fill it here to show in help screen if needed
    SET REMOTE_USER=%USERNAME%
)

:ParseParamsLoop
IF    [%1]==[]               GOTO ParseParamsContinue
IF /I [%1]==[-c]             GOTO ParamRemoteCmd
IF /I [%1]==[--remote-cmd]   GOTO ParamRemoteCmd
IF /I [%1]==[-p]             GOTO ParamRemotePath
IF /I [%1]==[--remote-path]  GOTO ParamRemotePath
IF /I [%1]==[-u]             GOTO ParamRemoteUser
IF /I [%1]==[--remote-user]  GOTO ParamRemoteUser
IF /I [%1]==[-k]             GOTO ParamSSHKeyFile
IF /I [%1]==[--ssh_key-file] GOTO ParamSSHKeyFile
IF /I [%1]==[-h]             GOTO ParamHelp
IF /I [%1]==[--help]         GOTO ParamHelp
:ParseParamsShift
SHIFT
GOTO ParseParamsLoop

:ParamRemotePath
SET REMOTE_PATH=%2
SHIFT
GOTO ParseParamsShift

:ParamRemoteCmd
SET REMOTE_CMD=%2
SHIFT
GOTO ParseParamsShift

:ParamRemoteUser
SET REMOTE_USER=%2
SHIFT
GOTO ParseParamsShift

:ParamSSHKeyFile
SET SSH_KEY_FILE=%2
SHIFT
GOTO ParseParamsShift

:ParamHelp
CALL :ShowHelp
GOTO End

:ParseParamsContinue
CALL :ExtractRemoteAddrAndHome %~d0

IF [%REMOTE_ADDR%] == [] (
    ECHO ERROR: Can't extract REMOTE_ADDR by %~d0!
	CALL :ShowHelp
	GOTO End
)

IF [%REMOTE_CMD%] == [] (
    ECHO ERROR: remote_cmd must be specified!
	CALL :ShowHelp
	GOTO End
) ELSE (
    SET BUILD_SCRIPT_PARAMS=%BUILD_SCRIPT_PARAMS% -x '%REMOTE_CMD%'
)

IF NOT [%REMOTE_PATH%] == [] (
    SET BUILD_SCRIPT_PARAMS=%BUILD_SCRIPT_PARAMS% -w '%REMOTE_PATH%'
)

IF [%SSH_KEY_FILE%] == [] (
    ECHO ERROR: ppk_file be either specified in command line or by WBLD_KEY environment varialble!
    CALL :ShowHelp
    GOTO End
)

CALL :PrepareUniqueMKProj
plink.exe -ssh %REMOTE_USER%@%REMOTE_ADDR% -X -i %SSH_KEY_FILE% "sh $WINBLD_BASE_DIR/%UNIQUE_BUILD_SCRIPT_COPY_NAME% %BUILD_SCRIPT_PARAMS%"
CALL :RollbackUniqueMKProj
GOTO End

:ExtractRemoteAddrAndHome
FOR /F "tokens=1,2,3" %%A in ('NET USE ^| FINDSTR /I %1') DO (
  FOR /F "tokens=1* delims=\" %%K in ("%%C") DO (
    SET REMOTE_ADDR=%%K
  )
)
GOTO :EOF

:PrepareUniqueMKProj
SET UNIQUE_BUILD_SCRIPT_COPY_NAME=%REMOTE_BUILD_SCRIPT%
FOR /F "tokens=1,2,3 delims=/ " %%A in ('DATE /T') do ( 
    SET UNIQUE_BUILD_SCRIPT_COPY_NAME=%UNIQUE_BUILD_SCRIPT_COPY_NAME%_%%A_%%B_%%C
)
FOR /F "tokens=1,2 delims=: " %%A in ('TIME /T') do ( 
    SET UNIQUE_BUILD_SCRIPT_COPY_NAME=%UNIQUE_BUILD_SCRIPT_COPY_NAME%_%%A_%%B
)
COPY %SCRIPTS_PATH%\%REMOTE_BUILD_SCRIPT% %SCRIPTS_HOME%\%UNIQUE_BUILD_SCRIPT_COPY_NAME%
GOTO :EOF

:RollbackUniqueMKProj
DEL %SCRIPTS_HOME%\%UNIQUE_BUILD_SCRIPT_COPY_NAME%
GOTO :EOF

:ShowHelp
    ECHO Usage mkwin.bat [params]
	ECHO --
	ECHO  Parameters (M - mandatory, O - optional):
	ECHO   -c [remote_cmd]
	ECHO   --remote-cmd [remote_cmd]   [M] Remote command to execute
	ECHO   -p [remote_path]
	ECHO   --remote-path [remote_path] [O] Remote path to CD before the command execution (in Windows form)
	ECHO   -u [remote_user]
	ECHO   --remote-user [remote_user] [O] Remote user to be used for SSH login. Could be defined by WBLD_USER
	ECHO                                   environment variable. Default: %REMOTE_USER%
	ECHO   -k [ppk_file]
	ECHO   --ssh-key-file [ppk_file]   [M] SSH Key file to use for login. Could be defined by WBLD_KEY 
	ECHO                                   environment variable.
	ECHO   -h
	ECHO   --help              [0] Show this help
GOTO :EOF

:End
SET UNIQUE_BUILD_SCRIPT_COPY_NAME=
SET REMOTE_ADDR=
SET SCRIPTS_PATH=
SET SCRIPTS_HOME=

SET BUILD_SCRIPT_PARAMS=
SET SSH_KEY_FILE=
SET REMOTE_USER=
SET REMOTE_CMD=
SET REMOTE_PATH=
SET REMOTE_BUILD_SCRIPT=
