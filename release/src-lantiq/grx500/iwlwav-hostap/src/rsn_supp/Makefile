all: librsn_supp.a

clean:
	rm -f *~ *.o *.d *.gcno *.gcda *.gcov librsn_supp.a
	rm -f build.wpa_supplicant build.hostapd

install:
	@echo Nothing to be made.

include ../lib.rules

CFLAGS += -DCONFIG_IEEE80211R
CFLAGS += -DCONFIG_TDLS
CFLAGS += -DCONFIG_WNM
CFLAGS += -DIEEE8021X_EAPOL

LIB_OBJS= \
	pmksa_cache.o \
	wpa_ft.o \
	tdls.o \
	preauth.o \
	wpa.o \
	wpa_ie.o

librsn_supp.a: $(LIB_OBJS)
	$(AR) crT $@ $?

-include $(OBJS:%.o=%.d)
