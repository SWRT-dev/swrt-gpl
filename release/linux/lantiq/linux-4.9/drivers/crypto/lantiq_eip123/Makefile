# eip-123 kernel style makefile

obj-$(CONFIG_CRYPTO_DEV_LANTIQ_EIP123) := umdevxs_k.o

umdevxs_k-y := \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_lkm.o \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_smbuf.o \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_ofdev.o \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_pcidev.o \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_chrdev.o \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_device.o \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_simdev.o \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_bufadmin.o \
    Sources/Integration/UMDevXS/KernelPart/src/umdevxs_interrupt.o

# add tool for some helper function
umdevxs_k-y += Sources/tool_eip123.o

# choose one of below for different bus or just simulation
#ccflags-y := -DUMDEVXS_PCI
ccflags-y := -DUMDEVXS_MEM
#ccflags-y := -DUMDEVXS_SIM

# do we seriously need the x86 flags for mips as well 
ccflags-y += -DARCH_X86

ccflags-y += \
    -I$(src)/Sources \
    -I$(src)/Sources/Kit/DriverFramework/v4/Basic_Defs/incl \
    -I$(src)/Sources/Kit/DriverFramework/v4/CLib_Abstraction/incl \
    -I$(src)/Sources/Kit/Log/incl \
    -I$(src)/Sources/Kit/Log/src/linux_kernel \
    -I$(src)/Sources/Integration/UMDevXS/KernelPart/incl \
    -I$(src)/Sources/Integration/UMDevXS/KernelPart/src
