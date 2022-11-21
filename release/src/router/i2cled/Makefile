# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
# Copyright 2019, PandoraBox Team.
# Copyright 2022, SWRTdev.
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND (PandoraBox Team/SWRTdev) GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.

PWD=$(shell pwd)

all:
ifeq ($(RALINK),y)
ifeq ($(R6800),y)
	@[ -f driver/gpio-sx1503-leds.c ] || cp -f $(PWD)/prebuild/R6800/i2cleds.ko $(PWD)/driver/
	@[ -f driver/gpio-sx1503-leds.c ] && $(MAKE) -C $(LINUXDIR) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD)/driver modules
else ifeq ($(RMAX6000),y)
	@[ -f driver/rmax6000-leds.c ] || cp -f $(PWD)/prebuild/RMAX6000/i2cleds.ko $(PWD)/driver/
	@[ -f driver/rmax6000-leds.c ] && $(MAKE) -C $(LINUXDIR) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD)/driver modules
endif
else ifeq ($(QCA),y)
ifeq ($(RAX120),y)
	@[ -f driver/rax120-leds.c ] || cp -f $(PWD)/prebuild/RAX120/i2cleds.ko $(PWD)/driver/
	@[ -f driver/rax120-leds.c ] && $(MAKE) -C $(LINUXDIR) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD)/driver modules
endif
endif

install:
	$(MAKE) -C $(LINUXDIR) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD)/driver INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
	@find $(INSTALLDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR) -name "*.ko" | xargs $(STRIPX)

clean:
	$(RM) *.o *.ko driver/*.o driver/*.ko driver/*.mod.*

