copy .\Release\BCLSockServer.exe G:\Software\_PC_SW\Latest_Versions\Installers\BCL_WLS
copy BCLHyp.dll G:\Software\_PC_SW\Latest_Versions\Installers\BCL_WLS
pause
call "C:\Program Files\GkSetup ECommerce-Edition V1.93\BldStd.EXE" /I:d:\work\bcl\installer.SOP
pause 
copy "C:\Program Files\GkSetup ECommerce-Edition V1.93\Output\BCL For Hyperion.EXE" "G:\Software\_PC_SW\Latest_Versions\BCL_WLS\BCL_For_Hyperion.EXE"
pause


