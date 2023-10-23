#!/bin/sh

CURRENT_PATH=`pwd`
LINUX_ROOT_PATH=
MODULE_NAME=proc_dbg
DRIVER_DIR=drivers/net/ifxmips_ppa

if [[ $# -gt 0 && -d $1 ]]; then
  LINUX_ROOT_PATH=$1
fi

if [ "$LINUX_ROOT_PATH" = "" ]; then
  LINUX_ROOT_PATH=$CURRENT_PATH/../../../../wireline_sw_linux26/ifx_wl_linux
fi

echo $LINUX_ROOT_PATH

COMPILER_PATH=`which mips-linux-gcc`
if [ ! -f $COMPILER_PATH ]; then
  echo Can not find compiler!
  exit
else
  COMPILER_PATH=`dirname $COMPILER_PATH`
fi

echo $COMPILER_PATH

COMPILER_INCLUDE_PATH=$COMPILER_PATH/../lib/gcc/mips-linux-uclibc/3.4.4/include


#cd $LINUX_ROOT_PATH
#cd drivers/atm
#rm -f $MODULE_NAME.c $MODULE_NAME.h $MODULE_NAME.mod.c $MODULE_NAME.mod.o $MODULE_NAME.o $MODULE_NAME.ko
#ln -s $CURRENT_PATH/$MODULE_NAME.c
#ln -s $CURRENT_PATH/$MODULE_NAME.h
#ln -s $CURRENT_PATH/$MODULE_NAME.mod.c
#cd ../..
cd $LINUX_ROOT_PATH
cd $DRIVER_DIR
ln -s $CURRENT_PATH/$MODULE_NAME.c
ln -s $CURRENT_PATH/$MODULE_NAME.mod.c
cd -

echo  mips-linux-gcc -Wp,-MD,$DRIVER_DIR/.$MODULE_NAME.o.d  -nostdinc -isystem $COMPILER_INCLUDE_PATH -D__KERNEL__ -Iinclude -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -O2  -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -Iinclude/asm-mips/ifx -Iinclude/asm-mips/mach-generic -fomit-frame-pointer -g  -Wdeclaration-after-statement    -DMODULE -mlong-calls  -D"KBUILD_STR(s)=#s" -D"KBUILD_BASENAME=KBUILD_STR($MODULE_NAME)"  -D"KBUILD_MODNAME=KBUILD_STR($MODULE_NAME)" -c -o $DRIVER_DIR/$MODULE_NAME.o $DRIVER_DIR/$MODULE_NAME.c
  mips-linux-gcc -Wp,-MD,$DRIVER_DIR/.$MODULE_NAME.o.d  -nostdinc -isystem $COMPILER_INCLUDE_PATH -D__KERNEL__ -Iinclude -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -O2  -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -Iinclude/asm-mips/ifx -Iinclude/asm-mips/mach-generic -fomit-frame-pointer -g  -Wdeclaration-after-statement    -DMODULE -mlong-calls  -D"KBUILD_STR(s)=#s" -D"KBUILD_BASENAME=KBUILD_STR($MODULE_NAME)"  -D"KBUILD_MODNAME=KBUILD_STR($MODULE_NAME)" -c -o $DRIVER_DIR/$MODULE_NAME.o $DRIVER_DIR/$MODULE_NAME.c

echo  mips-linux-gcc -Wp,-MD,drivers/atm/.$MODULE_NAME.mod.o.d  -nostdinc -isystem $COMPILER_INCLUDE_PATH -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -O2  -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -Iinclude/asm-mips/ifx -Iinclude/asm-mips/mach-generic -fomit-frame-pointer -g  -Wdeclaration-after-statement     -D"KBUILD_STR(s)=#s" -D"KBUILD_BASENAME=KBUILD_STR($MODULE_NAME.mod)"  -D"KBUILD_MODNAME=KBUILD_STR($MODULE_NAME)" -DMODULE -mlong-calls  -c -o $DRIVER_DIR/$MODULE_NAME.mod.o $DRIVER_DIR/$MODULE_NAME.mod.c
  mips-linux-gcc -Wp,-MD,$DRIVER_DIR/.$MODULE_NAME.mod.o.d  -nostdinc -isystem $COMPILER_INCLUDE_PATH -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -O2  -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -Iinclude/asm-mips/ifx -Iinclude/asm-mips/mach-generic -fomit-frame-pointer -g  -Wdeclaration-after-statement     -D"KBUILD_STR(s)=#s" -D"KBUILD_BASENAME=KBUILD_STR($MODULE_NAME.mod)"  -D"KBUILD_MODNAME=KBUILD_STR($MODULE_NAME)" -DMODULE -mlong-calls  -c -o $DRIVER_DIR/$MODULE_NAME.mod.o $DRIVER_DIR/$MODULE_NAME.mod.c

echo  mips-linux-ld  -m elf32btsmip -r -o $DRIVER_DIR/$MODULE_NAME.ko $DRIVER_DIR/$MODULE_NAME.o $DRIVER_DIR/$MODULE_NAME.mod.o
  mips-linux-ld  -m elf32btsmip -r -o $DRIVER_DIR/$MODULE_NAME.ko $DRIVER_DIR/$MODULE_NAME.o $DRIVER_DIR/$MODULE_NAME.mod.o


cd $DRIVER_DIR
rm -f  $MODULE_NAME.c $MODULE_NAME.mod.c $MODULE_NAME.mod.o $MODULE_NAME.o
if [ -f $MODULE_NAME.ko ]; then
  mv $MODULE_NAME.ko $CURRENT_PATH
  echo Build Successfully!
else
  echo Build Fail!
fi

