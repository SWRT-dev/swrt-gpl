# For external module build
PWD=$(shell pwd)

EXTRA_FLAGS += -I$(PWD)/ksmbd -I$(LINUXDIR)/include -DCONFIG_SMB_INSECURE_SERVER=1 -Wno-unused-function

all:
	cd tools && ./autogen.sh
	cd tools && ./configure --prefix=/usr --host=$(HOST) --disable-shared --enable-static  --libdir=/usr/lib \
	    CFLAGS="$(CFLAGS) -D_GNU_SOURCE -DNEED_PRINTF -Drpl_malloc=malloc -ffunction-sections -fdata-sections -Wl,--gc-sections" \
	    LDFLAGS="$(LDFLAGS) -ffunction-sections -fdata-sections -Wl,--gc-sections" \
	    LIBNL_CFLAGS="-I$(TOP)/libnl/include" \
	    LIBNL_LIBS="-L$(TOP)/libnl/lib/.libs -lnl-3 -lnl-genl-3" \
	    AR_FLAGS="cru" \
	    RANLIB="$(CROSS_COMPILE)ranlib"
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="$(EXTRA_FLAGS)" SUBDIRS=$(PWD)/ksmbd CONFIG_SMB_INSECURE_SERVER=y CONFIG_SMB_SERVER=m modules
	$(MAKE) -C tools all

install:
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="$(EXTRA_FLAGS)" SUBDIRS=$(PWD)/ksmbd INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
	@find $(INSTALLDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR) -name "*.ko" | xargs $(STRIPX)
	install -D tools/mountd/smbd_multicall $(INSTALLDIR)/usr/sbin/smbd_multicall
	$(STRIP) $(INSTALLDIR)/usr/sbin/smbd_multicall
	cd $(INSTALLDIR)/usr/sbin && ln -sf smbd_multicall ksmbd.mountd
	cd $(INSTALLDIR)/usr/sbin && ln -sf smbd_multicall ksmbd.adduser
	cd $(INSTALLDIR)/usr/sbin && ln -sf smbd_multicall ksmbd.addshare

clean:
	$(RM) *.o *.ko ksmbd/*.o ksmbd/*.ko ksmbd/*.mod.*
	$(MAKE) -C tools clean


