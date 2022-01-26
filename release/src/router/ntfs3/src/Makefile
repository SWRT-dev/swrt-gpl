ccflags-y += -I$(src) -include compat.h
ccflags-y += -DCONFIG_NTFS3_LZX_XPRESS
#ccflags-y += -DCONFIG_NTFS3_FS_POSIX_ACL

obj-m := ntfs3.o

ntfs3-y :=	compat.o \
		attrib.o \
		attrlist.o \
		bitfunc.o \
		bitmap.o \
		dir.o \
		fsntfs.o \
		frecord.o \
		file.o \
		fslog.o \
		inode.o \
		index.o \
		lznt.o \
		namei.o \
		record.o \
		run.o \
		super.o \
		upcase.o \
		xattr.o

ntfs3-y += $(addprefix lib/,\
		decompress_common.o \
		lzx_decompress.o \
		xpress_decompress.o \
		)
