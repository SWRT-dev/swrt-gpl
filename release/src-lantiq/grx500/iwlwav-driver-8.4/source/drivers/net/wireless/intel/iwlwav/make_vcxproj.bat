@ECHO OFF
SET CURDIR=%~p0
SET CURDIR=%CURDIR:\=;%
SET WBLD_DIR=support\wbld_scripts

for /F "tokens=* delims=;" %%i IN ("%CURDIR%") DO CALL :LAST_SUBDIR %%i

ECHO "Building VCXPROJ (%VCXPROJ_FNAME%)..."
perl %WBLD_DIR%\mtlk_list_files.pl . mtlk_list_files.cfg %WBLD_DIR%\msvc_all.vcxproj.cfg.template %WBLD_DIR%\msvc_all.vcxproj.cfg_list.template > %VCXPROJ_FNAME% < %WBLD_DIR%\msvc_all.vcxproj.template
perl %WBLD_DIR%\mtlk_list_files.pl . mtlk_list_files.cfg %WBLD_DIR%\msvc_all.vcxproj.cfg.template %WBLD_DIR%\msvc_all.vcxproj.cfg_list.template > %VCXPROJ_FILTERS_FNAME% < %WBLD_DIR%\msvc_all.vcxproj.filters.template
ECHO "Done (%VCXPROJ_FNAME%)!"

SET CURDIR=
SET VCXPROJ_FNAME=
SET VCPXROJ_FILTERS_FNAME=

:LAST_SUBDIR
IF "%1"=="" (
  GOTO :EOF
)

SET VCXPROJ_FNAME=msvc_all_[%1].vcxproj
SET VCXPROJ_FILTERS_FNAME=%VCXPROJ_FNAME%.filters
SHIFT

GOTO :LAST_SUBDIR
