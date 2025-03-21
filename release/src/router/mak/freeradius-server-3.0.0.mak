# Makefile won't be generated by configure script, don't delete it.
freeradius-server-3.0.0: pcre-8.31 freeradius-server-3.0.0/configure
	@$(MAKE) -j8 -C $@

freeradius-server-3.0.0/configure:
	cd freeradius-server-3.0.0 && ./autogen.sh && \
	$(CONFIGURE) CC=$(CC)  CFLAGS="$(EXTRACFLAGS) -I$(SRCBASE) -I$(SRCBASE)/include -I$(TOP)/pcrc-8.31" \
	LDFLAGS="$(EXTRACFLAGS) -L$(TOP)/pcre-8.31/.libs -lpcre -L$(TOP)/talloc-2.1.1/bin/default -ltalloc -Wl,-rpath=$(TOP)/talloc-2.1.1/bin/default -lpthread -ldl" \
	--prefix=/usr/freeradius --datarootdir=/usr/freeradius/share --with-raddbdir=/usr/freeradius/raddb \
	--with-openssl-includes=$(TOP)/openssl/include --with-openssl-libraries=$(TOP)/openssl \
	--with-talloc-lib-dir=$(TOP)/talloc-2.1.1/bin/default --with-talloc-include-dir=$(TOP)/talloc-2.1.1 \
	--with-sqlite-lib-dir=$(TOP)/sqlite/.libs --with-sqlite-include-dir=$(TOP)/sqlite \
	--with-pcre-lib-dir=$(TOP)/pcre-8.31/.libs  --with-pcre-include-dir=$(TOP)/pcrc-8.31

freeradius-server-3.0.0-install: freeradius-server-3.0.0
	@$(SEP)
	install -D $</build/bin/radiusd $(INSTALLDIR)/$</usr/sbin/radiusd
	$(STRIP) $(INSTALLDIR)/$</usr/sbin/radiusd
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/lib
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/share/freeradius && mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/certs
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/attr_filter
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/preprocess
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/files
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/sql/cui/sqlite
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/sql/ippool/sqlite
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/sql/ippool-dhcp/sqlite
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/sql/main/sqlite
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/mods-enabled
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/policy.d
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/sites-available
	mkdir -p $(INSTALLDIR)/$</usr/freeradius/raddb/sites-enabled
	install -D $</build/lib/.libs/libfreeradius-eap.so $(INSTALLDIR)/$</usr/freeradius/lib/libfreeradius-eap.so
	install -D $</build/lib/.libs/libfreeradius-radius.so $(INSTALLDIR)/$</usr/freeradius/lib/libfreeradius-radius.so
	install -D $</build/lib/.libs/libfreeradius-server.so $(INSTALLDIR)/$</usr/freeradius/lib/libfreeradius-server.so
ifeq ($(HND_ROUTER_BE_4916),y)
	# to avoid error Makefile:221: *** libcreduction FATAL: Missing 32-bit libraries: libfreeradius-eap.so libfreeradius-radius.so libfreeradius-server.so.  Stop.
	mkdir -p $(INSTALLDIR)/$</usr/lib/
	touch  $(INSTALLDIR)/$</usr/lib/libfreeradius-eap.so
	touch $(INSTALLDIR)/$</usr/lib/libfreeradius-radius.so
	touch $(INSTALLDIR)/$</usr/lib/libfreeradius-server.so
endif
	install -D $</build/lib/.libs/rlm_always.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_always.so
	install -D $</build/lib/.libs/rlm_attr_filter.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_attr_filter.so
	install -D $</build/lib/.libs/rlm_cache.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_cache.so
	install -D $</build/lib/.libs/rlm_chap.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_chap.so
	install -D $</build/lib/.libs/rlm_detail.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_detail.so
	install -D $</build/lib/.libs/rlm_dhcp.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_dhcp.so
	install -D $</build/lib/.libs/rlm_digest.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_digest.so
	install -D $</build/lib/.libs/rlm_dynamic_clients.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_dynamic_clients.so
	install -D $</build/lib/.libs/rlm_eap.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap.so
	install -D $</build/lib/.libs/rlm_eap_gtc.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_gtc.so
	install -D $</build/lib/.libs/rlm_eap_leap.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_leap.so
	install -D $</build/lib/.libs/rlm_eap_md5.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_md5.so
	install -D $</build/lib/.libs/rlm_eap_mschapv2.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_mschapv2.so
	install -D $</build/lib/.libs/rlm_eap_peap.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_peap.so
#	install -D $</build/lib/.libs/rlm_eap_pwd.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_pwd.so
#	install -D $</build/lib/.libs/rlm_eap_sim.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_sim.so
	install -D $</build/lib/.libs/rlm_eap_tls.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_tls.so
	install -D $</build/lib/.libs/rlm_eap_ttls.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_ttls.so
	install -D $</build/lib/.libs/rlm_exec.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_exec.so
	install -D $</build/lib/.libs/rlm_expiration.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_expiration.so
	install -D $</build/lib/.libs/rlm_expr.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_expr.so
	install -D $</build/lib/.libs/rlm_files.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_files.so
	install -D $</build/lib/.libs/rlm_linelog.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_linelog.so
	install -D $</build/lib/.libs/rlm_logintime.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_logintime.so
	install -D $</build/lib/.libs/rlm_mschap.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_mschap.so
	install -D $</build/lib/.libs/rlm_pap.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_pap.so
	install -D $</build/lib/.libs/rlm_passwd.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_passwd.so
	install -D $</build/lib/.libs/rlm_preprocess.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_preprocess.so
	install -D $</build/lib/.libs/rlm_radutmp.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_radutmp.so
	install -D $</build/lib/.libs/rlm_realm.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_realm.so
	install -D $</build/lib/.libs/rlm_replicate.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_replicate.so
	install -D $</build/lib/.libs/rlm_soh.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_soh.so
	install -D $</build/lib/.libs/rlm_sql.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_sql.so
	install -D $</build/lib/.libs/rlm_sqlcounter.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_sqlcounter.so
	install -D $</build/lib/.libs/rlm_sqlippool.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_sqlippool.so
	install -D $</build/lib/.libs/rlm_sql_sqlite.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_sql_sqlite.so
	install -D $</build/lib/.libs/rlm_unix.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_unix.so
	install -D $</build/lib/.libs/rlm_utf8.so $(INSTALLDIR)/$</usr/freeradius/lib/rlm_utf8.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/libfreeradius-eap.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/libfreeradius-radius.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/libfreeradius-server.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_always.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_attr_filter.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_cache.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_chap.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_detail.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_dhcp.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_digest.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_dynamic_clients.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_gtc.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_leap.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_md5.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_mschapv2.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_peap.so
#	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_pwd.so
#	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_sim.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_tls.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_eap_ttls.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_exec.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_expiration.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_expr.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_files.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_linelog.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_logintime.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_mschap.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_pap.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_passwd.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_preprocess.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_radutmp.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_realm.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_replicate.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_soh.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_sql.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_sqlcounter.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_sqlippool.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_sql_sqlite.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_unix.so
	$(STRIP) $(INSTALLDIR)/$</usr/freeradius/lib/rlm_utf8.so
	cp -rf $</share/* $(INSTALLDIR)/$</usr/freeradius/share/freeradius
	cp -f $</raddb/dictionary $(INSTALLDIR)/$</usr/freeradius/raddb/dictionary
	cp -f $</raddb/radiusd.conf $(INSTALLDIR)/$</usr/freeradius/raddb/radiusd.conf
	cp -f $</raddb/test_user.sql $(INSTALLDIR)/$</usr/freeradius/raddb/test_user.sql
	install -D $</raddb/mods-available/always $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/always
	install -D $</raddb/mods-available/attr_filter $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/attr_filter
	install -D $</raddb/mods-available/cache_eap $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/cache_eap
	install -D $</raddb/mods-available/chap $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/chap
	install -D $</raddb/mods-available/detail $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/detail
	install -D $</raddb/mods-available/detail.log $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/detail.log
	install -D $</raddb/mods-available/dhcp $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/dhcp
	install -D $</raddb/mods-available/digest $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/digest
	install -D $</raddb/mods-available/dynamic_clients $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/dynamic_clients
	install -D $</raddb/mods-available/eap $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/eap
	install -D $</raddb/mods-available/echo $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/echo
	install -D $</raddb/mods-available/exec $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/exec
	install -D $</raddb/mods-available/expiration $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/expiration
	install -D $</raddb/mods-available/expr $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/expr
	install -D $</raddb/mods-available/files $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/files
	install -D $</raddb/mods-available/linelog $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/linelog
	install -D $</raddb/mods-available/logintime $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/logintime
	install -D $</raddb/mods-available/mschap $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/mschap
	install -D $</raddb/mods-available/ntlm_auth $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/ntlm_auth
	install -D $</raddb/mods-available/pap $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/pap
	install -D $</raddb/mods-available/passwd $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/passwd
	install -D $</raddb/mods-available/preprocess $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/preprocess
	install -D $</raddb/mods-available/radutmp $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/radutmp
	install -D $</raddb/mods-available/realm $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/realm
	install -D $</raddb/mods-available/replicate $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/replicate
	install -D $</raddb/mods-available/soh $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/soh
	install -D $</raddb/mods-available/sql $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/sql
	install -D $</raddb/mods-available/sradutmp $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/sradutmp
	install -D $</raddb/mods-available/unix $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/unix
	install -D $</raddb/mods-available/utf8 $(INSTALLDIR)/$</usr/freeradius/raddb/mods-available/utf8
	cp -rf $</raddb/mods-config/attr_filter/* $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/attr_filter/
	cp -rf $</raddb/mods-config/preprocess/* $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/preprocess/
	cp -rf $</raddb/mods-config/files/* $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/files/
	cp -rf $</raddb/mods-config/sql/cui/sqlite/* $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/sql/cui/sqlite/
	cp -rf $</raddb/mods-config/sql/ippool/sqlite/* $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/sql/ippool/sqlite/
	cp -rf $</raddb/mods-config/sql/ippool-dhcp/sqlite/* $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/sql/ippool-dhcp/sqlite/
	cp -rf $</raddb/mods-config/sql/main/sqlite/* $(INSTALLDIR)/$</usr/freeradius/raddb/mods-config/sql/main/sqlite/
	cp -rf $</raddb/mods-enabled/* $(INSTALLDIR)/$</usr/freeradius/raddb/mods-enabled/
	cp -rf $</raddb/policy.d/* $(INSTALLDIR)/$</usr/freeradius/raddb/policy.d/
	cp -rf $</raddb/sites-available/default $(INSTALLDIR)/$</usr/freeradius/raddb/sites-available/
	cp -rf $</raddb/sites-available/inner-tunnel $(INSTALLDIR)/$</usr/freeradius/raddb/sites-available/
	cp -rf $</raddb/sites-enabled/* $(INSTALLDIR)/$</usr/freeradius/raddb/sites-enabled/
	cd $(TOP)/$</raddb/certs && make
	cp -f $(TOP)/$</raddb/certs/server.pem $(INSTALLDIR)/$</usr/freeradius/raddb/certs/
	cp -f $(TOP)/$</raddb/certs/ca.pem $(INSTALLDIR)/$</usr/freeradius/raddb/certs/
	cp -f $(TOP)/$</raddb/certs/dh $(INSTALLDIR)/$</usr/freeradius/raddb/certs/
	cd $(INSTALLDIR)/$</usr/freeradius/raddb/mods-enabled && ln -sf ../mods-available/sql sql

freeradius-server-3.0.0-clean:
	-@$(MAKE) -C freeradius-server-3.0.0 distclean
