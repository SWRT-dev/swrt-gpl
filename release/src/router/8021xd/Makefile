ifeq ($(CONFIG_SUPPORT_OPENWRT), y)
EXE:=8021xd

EXTRA_CFLAGS += -O2 -Wall -g $(CFLAGS)

# If you want to support multiple radius server for multiple bssid, add following line
EXTRA_CFLAGS +=  -DMULTIPLE_RADIUS=1

# If you want to debug daemon, add following line
EXTRA_CFLAGS +=  -DDBG=1

# If you want to support passpoint release 2 feature
EXTRA_CFLAGS +=  -DHOTSPOT_R2=1

OBJS =	rtdot1x.o eloop.o eapol_sm.o radius.o md5.o  \
	config.o ieee802_1x.o  \
	sta_info.o   radius_client.o

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(EXTRA_CFLAGS) -o $@ $(OBJS)

clean:
	-@rm -f *~ *.o $(EXE) *.d

$(OBJS): %.o : %.c
	$(CC) $(EXTRA_CFLAGS) -c $< -o $@

install: all
	install -D $(EXE) $(INSTALLDIR)/bin/$(EXE)
	$(STRIP) $(INSTALLDIR)/bin/$(EXE)
	cd $(INSTALLDIR)/bin && ln -sf $(EXE) rtinicapd	
	
else

#PLATFORM = 5VT
#PLATFORM = PC
#PLATFORM = STAR
PLATFORM = RT2880

ifeq ($(PLATFORM),RT2880)
CROSS_COMPILE = mipsel-linux-
endif

ifeq ($(PLATFORM),5VT)
CROSS_COMPILE = /opt/crosstool/uClibc_v5te_le_gcc_4_1_1/bin/arm-linux-
endif

ifeq ($(PLATFORM),STAR)
CROSS_COMPILE = /opt/star/tools/arm-linux/bin/arm-linux-
endif

ifeq ($(PLATFORM),PC)
CROSS_COMPILE =
endif

CC = $(CROSS_COMPILE)gcc

ifndef CFLAGS
CFLAGS = -O2 -Wall -g
endif

# If you want to support multiple radius server for multiple bssid, add following line
CFLAGS +=  -DMULTIPLE_RADIUS

# If you want to debug daemon, add following line
CFLAGS +=  -DDBG 

# If you want to support passpoint release 2 feature
CFLAGS +=  -DHOTSPOT_R2

OBJS =	rtdot1x.o eloop.o eapol_sm.o radius.o md5.o  \
	config.o ieee802_1x.o  \
	sta_info.o   radius_client.o

all: rtdot1xd

rtdot1xd: $(OBJS)
	$(CC) -o rt2860apd $(OBJS)

clean:
	rm -f core *~ *.o rt2860apd *.d

#
# add by YYHuang@Ralink
romfs:
	$(ROMFSINST) rt2860apd /bin/rt2860apd
	ln -sf rt2860apd $(ROMFSDIR)/bin/rtinicapd
	ln -sf rt2860apd $(ROMFSDIR)/bin/rtwifi3apd	
	ln -sf rt2860apd $(ROMFSDIR)/bin/rt2860apd_x
	

install: all
	install -D rt2860apd $(INSTALLDIR)/bin/rt2860apd
	$(STRIP) $(INSTALLDIR)/bin/rt2860apd
	cd $(INSTALLDIR)/bin && ln -sf rt2860apd rtinicapd 	

-include $(OBJS:%.o=%.d)

endif
