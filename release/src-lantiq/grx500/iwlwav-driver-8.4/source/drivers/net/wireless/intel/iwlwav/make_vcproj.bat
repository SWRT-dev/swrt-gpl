@ECHO OFF
SET CURDIR=%~p0
SET CURDIR=%CURDIR:\=;%
SET WBLD_DIR=support\wbld_scripts

for /F "tokens=* delims=;" %%i IN ("%CURDIR%") DO CALL :LAST_SUBDIR %%i

ECHO "Building VCPROJ (%VCPROJ_FNAME%)..."
perl %WBLD_DIR%\mtlk_list_files.pl . mtlk_list_files.cfg %WBLD_DIR%\msvc_all.vcproj.cfg.template NO_FILTERS > %VCPROJ_FNAME% < %WBLD_DIR%\msvc_all.vcproj.template
ECHO "Done (%VCPROJ_FNAME%)!"

SET CURDIR=
SET VCPROJ_FNAME=

:LAST_SUBDIR
IF "%1"=="" (
  GOTO :EOF
)

SET VCPROJ_FNAME=msvc_all_[%1].vcproj
SHIFT

GOTO :LAST_SUBDIR
