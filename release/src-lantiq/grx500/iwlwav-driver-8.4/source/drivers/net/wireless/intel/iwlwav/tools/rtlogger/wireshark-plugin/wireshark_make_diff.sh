#!/bin/sh

CLEAN_WIRESHARK_DIR=clean/wireshark-1.2.2
MTLK_WIRESHARK_DIR=wireshark-1.2.2
DIFF_CMD="diff -u -r "
FILES_TO_DIFF="configure.in \
               epan/Makefile.am \
               Makefile.am \
               Makefile.nmake \
               packaging/nsis/Makefile.nmake \
               packaging/nsis/wireshark.nsi \
               packaging/nsis/Custom.nmake \
               packaging/nsis/custom_plugins.txt \
               plugins/Makefile.am \
               plugins/Makefile.nmake \
               "

for file in $FILES_TO_DIFF
do
  $DIFF_CMD $CLEAN_WIRESHARK_DIR/$file $MTLK_WIRESHARK_DIR/$file
done

