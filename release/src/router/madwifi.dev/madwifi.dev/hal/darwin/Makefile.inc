#
# Copyright (c) 2002-2005 Sam Leffler, Errno Consulting
# Copyright (c) 2002-2005 Atheros Communications, Inc.
# All rights reserved.
#
# $Id: //depot/sw/branches/sam_hal/darwin/Makefile.inc#3 $
#
HAL=	..
PUBLIC=	${HAL}/public
#
# Makefile for building the Atheros/MADWIFI HAL from source code on Linux.
#
# This file is intended to be called by the Makefile in the hal
# directory with TARGET set to the desired target platform.
#
# There is support for 3 "generations" of Atheros hardware devices:
#
# AH_SUPPORT_AR5210	802.11a only MAC
# AH_SUPPORT_AR5211	802.11a+802.11b MAC (also pure g, but not yet supported)
# AH_SUPPORT_AR5212	802.11a+802.11b+802.11g MAC
# AH_SUPPORT_AR5312     5212 MAC WiSoC (requires AH_SUPPORT_AR5212)
#
# AH_SUPPORT_5111	5111 RF support for 5212 et. al.
# AH_SUPPORT_5112	5112 RF support for 5212 et. al.
# AH_SUPPORT_2413	2413 RF support for 5212 et. al.
# AH_SUPPORT_5413	5413 RF support for 5212 et. al.
# AH_SUPPORT_2316	2416 RF (Cobra) support for 5212 et. al.
#
# and numerous optional debugging facilities (typically named AH_DEBUG_*):
#
# AH_DEBUG		enables debugging stuff that's controlled by the sysctl:
# 			hw.ath.hal.debug
# AH_ASSERT 		enables assertions that panic or trap into the debugger.
# AH_DEBUG_ALQ 		enables support for tracing register reads+writes
#			via alq
# AH_DEBUG_COUNTRY	accept the CTR_DEBUG country code (for debugging
#			the regulatory domain support)
# AH_REGOPS_FUNC	arrange for OS_REG_READ and OS_REG_WRITE to be function
#			calls and not inline expanded to memory references
# AH_ENABLE_AP_SUPPORT	enables AP-specific support not publicly available
#			(e.g. don't cap turbo power limits)
# AH_DISABLE_WME	disable HAL_TXQ_USE_LOCKOUT_BKOFF_DIS support
#
# AH_PRIVATE_DIAG	enable private diagnostic support
# AH_WRITE_EEPROM	permit EEPROM writes (use with AH_PRIVATE_DIAG)
# AH_WRITE_REGDOMAIN	permit EEPROM writes of the regulatory domain through
#			ah_setRegulatoryDomain (implies AH_WRITE_EEPROM)
# AH_SUPPORT_XR		enable XR support
# AH_SUPPORT_WRITE_REG	permit direct writing of any HAL register via ioctl
#

#
# Default settings...
#
AH_SUPPORT_AR5210?=1		# 5210 support
AH_SUPPORT_AR5211?=1		# 5211 support
AH_SUPPORT_AR5212?=1		# 5212 support
AH_SUPPORT_AR5312?=0		# 5312 support
AH_SUPPORT_5111?=1		# 5111 RF support
AH_SUPPORT_5112?=1		# 5112 RF support
AH_SUPPORT_2413?=1		# 2413 RF support
AH_SUPPORT_5413?=1		# 5413 RF support
AH_SUPPORT_2316?=0		# 2413 RF support

AH_DEBUG?=0			# debugging (e.g. hw.ath.hal.debug)
AH_ASSERT?=0			# assertions (pretty lame)
AH_DEBUG_ALQ?=0			# ALQ register tracing support
AH_REGOPS_FUNC?=1		# use inline memory references
AH_WRITE_REGDOMAIN?=0		# support EEPROM writes of the regulatory domain
AH_DEBUG_COUNTRY?=0		# accept CTR_DEBUG country code
AH_DISABLE_WME?=0		# disable HAL_TXQ_USE_LOCKOUT_BKOFF_DIS
AH_ENABLE_AP_SUPPORT?=0		# enable AP-specific support
AH_WRITE_EEPROM?=0		# support EEPROM writes
AH_PRIVATE_DIAG?=1		# non-public diagnostic support
AH_NEED_DESC_SWAP?=0		# Need to swap TX descriptor
AH_SUPPORT_XR?=0		# disable XR
AH_SUPPORT_DFS?=1		# support DFS
AH_SUPPORT_WRITE_REG?=1		# support ioctl writing of regs
AH_SUPPORT_11D?=1		# support 11d country code changes

#
# If 5312 support is enabled it's almost certain we're not going
# to use the 5210 or 5211 support so disable it to save some space.
#
ifeq ($(strip ${AH_SUPPORT_AR5312}),1)
AH_SUPPORT_AR5210=0
AH_SUPPORT_AR5211=0
AH_SUPPORT_AR5414=0
endif

#
# Basic build components.
#
OBJDIR=	obj/${TARGET}

COPTS=	-Werror -g -O -std=c99
INCS=	-I${OBJDIR} -I. -I..
CLEANFILES=	${OBJDIR}/.depend

include ${PUBLIC}/${TARGET}.inc

CFLAGS=	${INCS} ${COPTS}

HAL_SRCS=${HAL}/ah.c ${HAL}/ah_eeprom.c ${HAL}/ah_regdomain.c
HAL_OBJS=${OBJDIR}/ah.o ${OBJDIR}/ah_eeprom.o ${OBJDIR}/ah_regdomain.o

AR5210=	${HAL}/ar5210
HAL_SRCS+=${AR5210}/ar5210_attach.c ${AR5210}/ar5210_beacon.c \
	${AR5210}/ar5210_interrupts.c ${AR5210}/ar5210_keycache.c \
	${AR5210}/ar5210_misc.c ${AR5210}/ar5210_power.c \
	${AR5210}/ar5210_phy.c ${AR5210}/ar5210_recv.c \
	${AR5210}/ar5210_reset.c ${AR5210}/ar5210_xmit.c \
	${AR5210}/ar5210_radar.c
HAL_OBJS+=${OBJDIR}/ar5210_attach.o ${OBJDIR}/ar5210_beacon.o \
	${OBJDIR}/ar5210_interrupts.o ${OBJDIR}/ar5210_keycache.o \
	${OBJDIR}/ar5210_misc.o ${OBJDIR}/ar5210_power.o \
	${OBJDIR}/ar5210_phy.o ${OBJDIR}/ar5210_recv.o \
	${OBJDIR}/ar5210_reset.o ${OBJDIR}/ar5210_xmit.o \
	${OBJDIR}/ar5210_radar.o

AR5211=	${HAL}/ar5211
HAL_SRCS+=${AR5211}/ar5211_attach.c ${AR5211}/ar5211_beacon.c \
	${AR5211}/ar5211_interrupts.c ${AR5211}/ar5211_keycache.c \
	${AR5211}/ar5211_misc.c ${AR5211}/ar5211_power.c \
	${AR5211}/ar5211_phy.c ${AR5211}/ar5211_recv.c \
	${AR5211}/ar5211_reset.c ${AR5211}/ar5211_xmit.c \
	${AR5211}/ar5211_radar.c
HAL_OBJS+=${OBJDIR}/ar5211_attach.o ${OBJDIR}/ar5211_beacon.o \
	${OBJDIR}/ar5211_interrupts.o ${OBJDIR}/ar5211_keycache.o \
	${OBJDIR}/ar5211_misc.o ${OBJDIR}/ar5211_power.o \
	${OBJDIR}/ar5211_phy.o ${OBJDIR}/ar5211_recv.o \
	${OBJDIR}/ar5211_reset.o ${OBJDIR}/ar5211_xmit.o \
	${OBJDIR}/ar5211_radar.o

AR5212=	${HAL}/ar5212
HAL_SRCS+=${AR5212}/ar5212_attach.c ${AR5212}/ar5212_beacon.c \
	${AR5212}/ar5212_eeprom.c ${AR5212}/ar5212_gpio.c \
	${AR5212}/ar5212_interrupts.c ${AR5212}/ar5212_keycache.c \
	${AR5212}/ar5212_misc.c ${AR5212}/ar5212_power.c \
	${AR5212}/ar5212_phy.c ${AR5212}/ar5212_recv.c \
	${AR5212}/ar5212_reset.c ${AR5212}/ar5212_xmit.c \
	${AR5212}/ar5212_ani.c ${AR5212}/ar5212_radar.c
HAL_OBJS+=${OBJDIR}/ar5212_attach.o ${OBJDIR}/ar5212_beacon.o \
	${OBJDIR}/ar5212_eeprom.o ${OBJDIR}/ar5212_gpio.o \
	${OBJDIR}/ar5212_interrupts.o ${OBJDIR}/ar5212_keycache.o \
	${OBJDIR}/ar5212_misc.o ${OBJDIR}/ar5212_power.o \
	${OBJDIR}/ar5212_phy.o ${OBJDIR}/ar5212_recv.o \
	${OBJDIR}/ar5212_reset.o ${OBJDIR}/ar5212_xmit.o \
	${OBJDIR}/ar5212_ani.o ${OBJDIR}/ar5212_radar.o

AR5312=	${HAL}/ar5312
HAL_SRCS+=${AR5312}/ar5312_attach.c ${AR5312}/ar5312_interrupts.c \
	${AR5312}/ar5312_eeprom.c \
	${AR5312}/ar5312_gpio.c ${AR5312}/ar5315_gpio.c \
	${AR5312}/ar5312_misc.c ${AR5312}/ar5312_power.c \
	${AR5312}/ar5312_reset.c \
	${AR5312}/ar5312_bsp.c
HAL_OBJS+=${OBJDIR}/ar5312_attach.o ${OBJDIR}/ar5312_interrupts.o \
	${OBJDIR}/ar5312_eeprom.o \
	${OBJDIR}/ar5312_gpio.o ${OBJDIR}/ar5315_gpio.o \
	${OBJDIR}/ar5312_misc.o ${OBJDIR}/ar5312_power.o \
	${OBJDIR}/ar5312_reset.o \
	${OBJDIR}/ar5312_bsp.o

HAL_SRCS+=${AR5212}/ar5111.c
HAL_OBJS+=${OBJDIR}/ar5111.o

HAL_SRCS+=${AR5212}/ar5112.c
HAL_OBJS+=${OBJDIR}/ar5112.o

HAL_SRCS+=${AR5212}/ar2413.c
HAL_OBJS+=${OBJDIR}/ar2413.o

HAL_SRCS+=${AR5212}/ar5413.c
HAL_OBJS+=${OBJDIR}/ar5413.o

HAL_SRCS+=${AR5212}/ar2316.c
HAL_OBJS+=${OBJDIR}/ar2316.o

SRCS+=	${HAL_SRCS}
CLEANFILES+= ${OBJDIR}/opt_ah.h ${HAL_OBJS} \
	${OBJDIR}/t.o ${OBJDIR}/hal.mangle ${OBJDIR}/hal-release.o

#
# NB: These are typically overridden in the target-specific specification.
#
LD?=	ld
STRIP?=	strip
NM?=	nm
SED?=	sed
OBJCOPY?=objcopy
#
# These are specific to the host doing the build.
#
AWK?=	awk
UUENCODE?=uuencode
CVS?=	cvs

ALL=	${OBJDIR}/hal.o

all:	${ALL}

${OBJDIR}/hal.o: ${HAL_OBJS} Makefile
	${LD} -o $@ -r ${LDOPTS} ${HAL_OBJS}

#
# The released object file is constructed by stripping all possible symbols
# and mangling any that remain.  The latter is necessary because GCC stubornly
# uses various symbols for relocation when it doesn't need to.  Look at the
# script syms.awk to understand the symbol munging.
#
${OBJDIR}/hal.mangle: ${HAL_SRCS} ${PUBLIC}/syms.awk
	${STRIP} --strip-unneeded -o ${OBJDIR}/t.o ${OBJDIR}/hal.o
	${NM} -a ${OBJDIR}/t.o | grep -v ' U' | sed \
		-e '/ath_hal_probe/d' \
		-e '/ath_hal_attach/d' \
		-e '/_ath_hal_attach/d' \
		-e '/ath_hal_detach/d' \
		-e '/ath_hal_init_channels/d' \
		-e '/ath_hal_getwirelessmodes/d' \
		-e '/ath_hal_computetxtime/d' \
		-e '/ath_hal_mhz2ieee/d' \
		-e '/ath_hal_ieee2mhz/d' \
		-e '/ath_hal_version/d' \
		-e '/ath_hal_buildopts/d' \
		| ${AWK} -f ${PUBLIC}/syms.awk > ${OBJDIR}/hal.mangle
	rm -f t.o
${OBJDIR}/hal-release.o: ${OBJDIR}/hal.o ${OBJDIR}/hal.mangle
	${STRIP} --strip-unneeded -o ${OBJDIR}/hal-release.o ${OBJDIR}/hal.o
	${OBJCOPY} `cat ${OBJDIR}/hal.mangle` ${OBJDIR}/hal-release.o

${OBJDIR}/hal-symrelease.o: ${OBJDIR}/hal.o 
	cp ${OBJDIR}/hal.o ${OBJDIR}/hal-symrelease.o

#
# XXX should validate the release to make sure, for example, that AH_DEBUG
#     was not used
#
release: ${OBJDIR}/hal-release.o
	rm -f ${PUBLIC}/${TARGET}.hal.o.uu
	(${SED} -e '1,/^$$/d' ${HAL}/COPYRIGHT; \
	 ${SED} -n -e '/ATH_HAL_VERSION/p' ${HAL}/version.h; \
	 ${UUENCODE} ${OBJDIR}/hal-release.o hal.o) > ${PUBLIC}/${TARGET}.hal.o.uu
	cp ${OBJDIR}/opt_ah.h ${PUBLIC}/${TARGET}.opt_ah.h

symrelease: ${OBJDIR}/hal-symrelease.o
	rm -f ${PUBLIC}/${TARGET}.hal.o.uu
	(${SED} -e '1,/^$$/d' ${HAL}/COPYRIGHT; \
	 ${SED} -n -e '/ATH_HAL_VERSION/p' ${HAL}/version.h; \
	 ${UUENCODE} ${OBJDIR}/hal-symrelease.o hal.o) > ${PUBLIC}/${TARGET}.hal.o.uu
	cp ${OBJDIR}/opt_ah.h ${PUBLIC}/${TARGET}.opt_ah.h


${OBJDIR}/ah.o:	${HAL}/ah.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ah_eeprom.o:	${HAL}/ah_eeprom.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ah_regdomain.o:	${HAL}/ah_regdomain.c
	${CC} -c -o $@ ${CFLAGS} $<

${OBJDIR}/ar5210_attach.o: ${AR5210}/ar5210_attach.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_beacon.o: ${AR5210}/ar5210_beacon.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_interrupts.o: ${AR5210}/ar5210_interrupts.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_keycache.o: ${AR5210}/ar5210_keycache.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_misc.o:	${AR5210}/ar5210_misc.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_power.o:	${AR5210}/ar5210_power.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_phy.o:	${AR5210}/ar5210_phy.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_radar.o:	${AR5210}/ar5210_radar.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_recv.o:	${AR5210}/ar5210_recv.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_reset.o:	${AR5210}/ar5210_reset.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5210_xmit.o:	${AR5210}/ar5210_xmit.c
	${CC} -c -o $@ ${CFLAGS} $<

${OBJDIR}/ar5211_attach.o: ${AR5211}/ar5211_attach.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_beacon.o: ${AR5211}/ar5211_beacon.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_interrupts.o: ${AR5211}/ar5211_interrupts.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_keycache.o: ${AR5211}/ar5211_keycache.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_misc.o:	${AR5211}/ar5211_misc.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_power.o:	${AR5211}/ar5211_power.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_phy.o:	${AR5211}/ar5211_phy.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_radar.o:	${AR5211}/ar5211_radar.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_recv.o:	${AR5211}/ar5211_recv.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_reset.o:	${AR5211}/ar5211_reset.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5211_xmit.o:	${AR5211}/ar5211_xmit.c
	${CC} -c -o $@ ${CFLAGS} $<

${OBJDIR}/ar5212_ani.o: ${AR5212}/ar5212_ani.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_attach.o: ${AR5212}/ar5212_attach.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_beacon.o: ${AR5212}/ar5212_beacon.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_eeprom.o: ${AR5212}/ar5212_eeprom.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_gpio.o: ${AR5212}/ar5212_gpio.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_interrupts.o: ${AR5212}/ar5212_interrupts.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_keycache.o: ${AR5212}/ar5212_keycache.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_misc.o:	${AR5212}/ar5212_misc.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_power.o:	${AR5212}/ar5212_power.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_phy.o:	${AR5212}/ar5212_phy.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_radar.o:	${AR5212}/ar5212_radar.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_recv.o:	${AR5212}/ar5212_recv.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_reset.o:	${AR5212}/ar5212_reset.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5212_xmit.o:	${AR5212}/ar5212_xmit.c
	${CC} -c -o $@ ${CFLAGS} $<

${OBJDIR}/ar5312_attach.o: ${AR5312}/ar5312_attach.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5312_bsp.o: ${AR5312}/ar5312_bsp.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5312_eeprom.o: ${AR5312}/ar5312_eeprom.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5312_gpio.o: ${AR5312}/ar5312_gpio.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5315_gpio.o: ${AR5315}/ar5315_gpio.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5312_interrupts.o: ${AR5312}/ar5312_interrupts.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5312_misc.o:	${AR5312}/ar5312_misc.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5312_power.o: ${AR5312}/ar5312_power.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5312_reset.o:	${AR5312}/ar5312_reset.c
	${CC} -c -o $@ ${CFLAGS} $<

${OBJDIR}/ar5111.o: ${AR5212}/ar5111.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5112.o: ${AR5212}/ar5112.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar2413.o: ${AR5212}/ar2413.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar5413.o: ${AR5212}/ar5413.c
	${CC} -c -o $@ ${CFLAGS} $<
${OBJDIR}/ar2316.o: ${AR5212}/ar2316.c
	${CC} -c -o $@ ${CFLAGS} $<

${OBJDIR}/opt_ah.h:
	test -d ${OBJDIR} || mkdir -p ${OBJDIR}
ifeq ($(strip ${AH_SUPPORT_AR5210}),1)
	echo "#define AH_SUPPORT_AR5210	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_AR5211}),1)
	echo "#define AH_SUPPORT_AR5211	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_AR5212}),1)
	echo "#define AH_SUPPORT_AR5212	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_AR5312}),1)
	echo "#define AH_SUPPORT_AR5312	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_5111}),1)
	echo "#define AH_SUPPORT_5111	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_5112}),1)
	echo "#define AH_SUPPORT_5112	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_2413}),1)
	echo "#define AH_SUPPORT_2413	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_5413}),1)
	echo "#define AH_SUPPORT_5413	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_2316}),1)
	echo "#define AH_SUPPORT_2316	1"	>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_ASSERT}),1)
	echo "#define AH_ASSERT	1"		>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_DEBUG_ALQ}),1)
	echo "#define AH_DEBUG_ALQ 1"		>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_DEBUG}),1)
	echo "#define AH_DEBUG	1"		>> ${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_WRITE_REGDOMAIN}),1)
	echo "#define AH_SUPPORT_WRITE_REGDOMAIN 1" >>${OBJDIR}/opt_ah.h
	echo "#define AH_SUPPORT_WRITE_EEPROM 1">>${OBJDIR}/opt_ah.h
else
ifeq ($(strip ${AH_WRITE_EEPROM}),1)
	echo "#define AH_SUPPORT_WRITE_EEPROM 1">>${OBJDIR}/opt_ah.h
endif
endif
ifeq ($(strip ${AH_DEBUG_COUNTRY}),1)
	echo "#define AH_DEBUG_COUNTRY 1"	>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_REGOPS_FUNC}),1)
	echo "#define AH_REGOPS_FUNC 1"		>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_DISABLE_WME}),1)
	echo "#define AH_DISABLE_WME 1"		>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_ENABLE_AP_SUPPORT}),1)
	echo "#define AH_ENABLE_AP_SUPPORT 1"	>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_PRIVATE_DIAG}),1)
	echo "#define AH_PRIVATE_DIAG 1"	>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_NEED_DESC_SWAP}),1)
	echo "#define AH_NEED_DESC_SWAP 1"	>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_XR}),1)
	echo "#define AH_SUPPORT_XR	1"	>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_DFS}),1)
	echo "#define AH_SUPPORT_DFS	1"	>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_WRITE_REG}),1)
	echo "#define AH_SUPPORT_WRITE_REG	1"	>>${OBJDIR}/opt_ah.h
endif
ifeq ($(strip ${AH_SUPPORT_11D}),1)
	echo "#define AH_SUPPORT_11D		1"	>>${OBJDIR}/opt_ah.h
endif

clean:
	rm -f ${ALL} ${OBJS} ${CLEANFILES}
	-rmdir -p ${OBJDIR}

depend: beforedepend .depend afterdepend
${OBJDIR}/.depend: ${SRCS} ${OBJDIR}/opt_ah.h
	rm -f ${OBJDIR}/.depend
	${CC} -M ${CFLAGS} ${SRCS} | \
		sed 's,\(.*\)\.o:,${OBJDIR}/\1.o:,g' > ${OBJDIR}/.depend
afterdepend:
-include ${OBJDIR}/.depend
