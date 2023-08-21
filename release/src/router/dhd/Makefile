all:
ifeq ($(or $(RTAC3200),$(SBRAC3200P)),y)
	-cp ./prebuilt/dhd_7 ./dhd
else
	-cp ./prebuilt/dhd_7114 ./dhd
endif
clean:
	rm -f *.o dhd

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install dhd $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/dhd

dhd:
ifeq ($(or $(RTAC3200),$(SBRAC3200P)),y)
	-cp ./prebuilt/dhd_7 ./dhd
else
	-cp ./prebuilt/dhd_7114 ./dhd
endif

