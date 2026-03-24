
CC=mips-linux-gcc
STRIP=mips-linux-strip

CFLAGS='-O2 -Wall -I ../../../../wireline_sw_linux26/ifx_wl_linux/include/'

echo $CC $CFLAGS -o ppacmd ppacmd.c

$CC $CFLAGS -o ppacmd ppacmd.c
$STRIP  ppacmd

