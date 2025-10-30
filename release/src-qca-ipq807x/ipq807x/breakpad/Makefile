# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS+= \
	-DTARGET="native"  \
	-DTARGET_NAME=\''native'\' \
	-I$(STAGING_DIR)/usr/include/breakpad

NEW_CXXFLAGS=$(filter-out -Wno-pointer-sign,$(TARGET_CXXFLAGS))

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/breakpad/configure),)
	ln -sf $(SDK_DIR_PLATFORM)/breakpad source
	cd source && $(CONFIGURE) --prefix=/usr --bindir=/usr/sbin --libdir=/usr/lib
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
else
#	mkdir -p $(PKG_BUILD_DIR)
#	cp prebuilt/$(BUILD_NAME)/breakpad $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
#	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
#	install -m 755 -D $(PKG_BUILD_DIR)/breakpad $(INSTALLDIR)/usr/sbin
#	@$(STRIP) $(INSTALLDIR)/usr/sbin/breakpad

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/breakpad/configure),)
	mkdir -p $(STAGING_DIR)/usr/include/breakpad
	mkdir -p $(STAGING_DIR)/usr/include/breakpad/client/linux/crash_generation
	mkdir -p $(STAGING_DIR)/usr/include/breakpad/client/linux/handler
	mkdir -p $(STAGING_DIR)/usr/include/breakpad/client/linux/dump_writer_common
	mkdir -p $(STAGING_DIR)/usr/include/breakpad/client/linux/minidump_writer
	mkdir -p $(STAGING_DIR)/usr/include/breakpad/common
	mkdir -p $(STAGING_DIR)/usr/include/breakpad/common/linux
	mkdir -p $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	mkdir -p $(STAGING_DIR)/usr/include/breakpad/third_party/lss
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/crash_generation/crash_generation_client.h $(STAGING_DIR)/usr/include/breakpad/client/linux/crash_generation
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/handler/exception_handler.h $(STAGING_DIR)/usr/include/breakpad/client/linux/handler
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/handler/minidump_descriptor.h $(STAGING_DIR)/usr/include/breakpad/client/linux/handler
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/handler/microdump_extra_info.h $(STAGING_DIR)/usr/include/breakpad/client/linux/handler
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/dump_writer_common/mapping_info.h $(STAGING_DIR)/usr/include/breakpad/client/linux/dump_writer_common
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/dump_writer_common/thread_info.h $(STAGING_DIR)/usr/include/breakpad/client/linux/dump_writer_common
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/dump_writer_common/raw_context_cpu.h $(STAGING_DIR)/usr/include/breakpad/client/linux/dump_writer_common
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/minidump_writer/linux_dumper.h $(STAGING_DIR)/usr/include/breakpad/client/linux/minidump_writer
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/minidump_writer/minidump_writer.h $(STAGING_DIR)/usr/include/breakpad/client/linux/minidump_writer
	$(CP) $(PKG_BUILD_DIR)/src/common/memory_range.h $(STAGING_DIR)/usr/include/breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/common/memory_allocator.h $(STAGING_DIR)/usr/include/breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/common/scoped_ptr.h $(STAGING_DIR)/usr/include/breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/common/using_std_string.h $(STAGING_DIR)/usr/include/breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/common/basictypes.h $(STAGING_DIR)/usr/include/breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/common/linux/file_id.h $(STAGING_DIR)/usr/include/breakpad/common/linux
	$(CP) $(PKG_BUILD_DIR)/src/common/linux/guid_creator.h $(STAGING_DIR)/usr/include/breakpad/common/linux
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/breakpad_types.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_format.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_cpu_amd64.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_cpu_arm.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_cpu_arm64.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_cpu_mips.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_cpu_ppc64.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_cpu_ppc.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_cpu_sparc.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_cpu_x86.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_exception_linux.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_exception_mac.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_exception_ps3.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_exception_solaris.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/google_breakpad/common/minidump_exception_win32.h $(STAGING_DIR)/usr/include/breakpad/google_breakpad/common
	$(CP) $(PKG_BUILD_DIR)/src/third_party/lss/linux_syscall_support.h $(STAGING_DIR)/usr/include/breakpad/third_party/lss
	$(CP) $(PKG_BUILD_DIR)/src/client/linux/libbreakpad_client.a $(STAGING_DIR)/usr/lib
endif
