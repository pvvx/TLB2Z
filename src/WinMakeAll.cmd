@set TLSDK=D:\MCU\TelinkIoTStudio
@set PATH=%TLSDK%\bin;%TLSDK%\opt\tc32\bin;%TLSDK%\mingw\bin;%TLSDK%\opt\tc32\tc32-elf\bin;%PATH%
@set SWVER=_v0114
@del /Q .\bin
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ts01 POJECT_DEF="-DBOARD=BOARD_TS0001_TZ3000_RBZ -DSWS_PRINTF_MODE=0" ZNAME="B2Z:REL-BZ01"
@if not exist "bin\ts01%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=tb3f POJECT_DEF="-DBOARD=BOARD_TB03F_KIT -DSWS_PRINTF_MODE=0" ZNAME="B2Z:TB03F-bz"
@if not exist "bin\tb3f%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=zg807z POJECT_DEF="-DBOARD=BOARD_ZG807Z -DSWS_PRINTF_MODE=0" ZNAME="B2Z:ZG-807Z"
@if not exist "bin\tb3f%SWVER%.bin" goto :error
cd .\zigpy_ota
call update.cmd %SWVER%
cd ..
@exit
:error
echo "Error!"

         