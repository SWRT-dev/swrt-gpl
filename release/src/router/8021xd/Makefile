EXE:=8021xd

EXTRA_CFLAGS += -O2 -Wall -g $(CFLAGS)

# If you want to support multiple radius server for multiple bssid, add following line
EXTRA_CFLAGS +=  -DMULTIPLE_RADIUS=1

# If you want to debug daemon, add following line
EXTRA_CFLAGS +=  -DDBG=1

# If you want to support passpoint release 2 feature
EXTRA_CFLAGS +=  -DHOTSPOT_R2=1

# If you want to support passpoint release 3 feature
EXTRA_CFLAGS +=  -DHOTSPOT_R3=1

OBJS =	rtdot1x.o eloop.o eapol_sm.o radius.o md5.o  \
	config.o ieee802_1x.o  \
	sta_info.o   radius_client.o

# If you want support dynamic authorization service (das - RFC 5176)
EXTRA_CFLAGS += -DRADIUS_DAS_SUPPORT=1
OBJS += radius_das.o

#RADIUS_MAC_ACL_SUPPORT=y
ifdef RADIUS_MAC_ACL_SUPPORT
CFLAGS += -DRADIUS_MAC_ACL_SUPPORT
OBJS +=  ieee802_11_auth.o
endif

#FILS
CONFIG_FILS=y

ifdef CONFIG_FILS
EXTRA_CFLAGS += -I$(abspath ./utils)
EXTRA_CFLAGS += -I$(abspath ./)
EXTRA_CFLAGS += -DCONFIG_FILS=1 
OBJS += ./utils/misc.o
OBJS += ./utils/wpabuf.o
NEED_SHA384=y
NEED_SHA256=y
NEED_AES_SIV=y
NEED_AES=y

CONFIG_INTERNAL_AES=y
CONFIG_INTERNAL_SHA256=y
CONFIG_INTERNAL_SHA384=y
CONFIG_INTERNAL_SHA512=y
ifdef CONFIG_INTERNAL_SHA256
OBJS += ./crypto/sha256-internal.o
endif
ifdef CONFIG_INTERNAL_SHA384
EXTRA_CFLAGS += -DCONFIG_INTERNAL_SHA384=1
OBJS += ./crypto/sha384-internal.o
endif
ifdef CONFIG_INTERNAL_SHA512
EXTRA_CFLAGS += -DCONFIG_INTERNAL_SHA512=1
OBJS += ./crypto/sha512-internal.o
endif

ifdef NEED_SHA384
EXTRA_CFLAGS += -DCONFIG_SHA384=1
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), gnutls)
ifneq ($(CONFIG_TLS), wolfssl)
OBJS += ./crypto/sha384.o
endif
endif
endif
endif
OBJS += ./crypto/sha384-prf.o
endif

ifdef NEED_SHA256
EXTRA_CFLAGS += -DCONFIG_SHA256=1
ifneq ($(CONFIG_TLS), openssl)
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), gnutls)
ifneq ($(CONFIG_TLS), wolfssl)
OBJS += ./crypto/sha256.o
endif
endif
endif
endif
OBJS += ./crypto/sha256-prf.o
endif

AESOBJS = # none so far
ifdef CONFIG_INTERNAL_AES
AESOBJS += ./crypto/aes-internal.o ./crypto/aes-internal-enc.o
endif
ifdef NEED_AES_SIV
AESOBJS += ./crypto/aes-siv.o
NEED_AES_CTR=y
NEED_AES_OMAC1=y
endif
ifdef NEED_AES_CTR
AESOBJS += ./crypto/aes-ctr.o
endif
ifdef NEED_AES_OMAC1
ifneq ($(CONFIG_TLS), linux)
ifneq ($(CONFIG_TLS), wolfssl)
AESOBJS += ./crypto/aes-omac1.o
endif
endif
endif

ifdef NEED_AES
OBJS += $(AESOBJS)
endif

OBJS += ./common/ieee802_11_common.o
OBJS += ./drvcallbak/drv_hook.o
OBJS += ./drvcallbak/drv_mtk.o
OBJS += ./ap/pmk_cache_ctrl.o
OBJS += ./ap/wpa.o
OBJS += ./ap/fils_conf.o
OBJS += ./ap/fils_aead.o
OBJS += ./ap/fils.o
OBJS += ./ap/fils_hlp.o
endif #end of FILS

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(EXTRA_CFLAGS) -o $@ $(OBJS)

clean:
	-@rm -f *~ *.o $(EXE) *.d ap/*.o crypto/*.o utils/*.o common/*.o drvcallbak/*.o

$(OBJS): %.o : %.c
	$(CC) $(EXTRA_CFLAGS) -c $< -o $@
install: all
	install -D $(EXE) $(INSTALLDIR)/bin/$(EXE)
	$(STRIP) $(INSTALLDIR)/bin/$(EXE)
	cd $(INSTALLDIR)/bin && ln -sf $(EXE) rtinicapd 
