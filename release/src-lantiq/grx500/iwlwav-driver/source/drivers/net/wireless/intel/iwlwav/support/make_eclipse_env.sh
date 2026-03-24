#!/bin/bash
# $Id$

echo "--== [ create helper directories and links ] ==--"

_kernel_dir=`echo $(get_kernel_dir)`
_cross_compile=`echo $(get_kernel_cross_compile)`
_base_toolchain_path=$_BASE_TOOLCHAIN_PATH

# Make link to kernel directory
ln -s ${_kernel_dir} ${srcdir}/builds/${CONFIG_ENVIRONMENT_NAME}/kernel
# Make link to objdump
ln -s ${_cross_compile}objdump ${srcdir}/builds/${CONFIG_ENVIRONMENT_NAME}/objdump

#### Copy kernel headers if requested ###
if [ "x${COPY_KERNEL_HEADERS}" == "xy" ]; then
    include="${srcdir}/headers"

    # Create include directory
    if [ ! -d ${include} ]; then
	mkdir -v ${include}
    else
	rm -f -r ${include}/*
    fi

    mkdir ${include}/config
    mkdir ${include}/kernel
    mkdir ${include}/libc

    # Copy kernel headers
    echo "* Copy kernel headers..."
    cp -r ${_kernel_dir}/include ${include}/kernel/include
    echo "* Copy net headers..."
    cp -r ${_kernel_dir}/net ${include}/kernel/net
    echo "* Copy arch headers..."
    cp -r ${_kernel_dir}/arch/${kernel_arch}/include ${include}/kernel/arch
    # Copy libc headers (currently may not work in yocto config)
    echo "* Copy libs headers..."
    cp -r ${_base_toolchain_path}/include ${include}/libc/include


    # Make links to config files
    ln -s ${srcdir}/builds/${CONFIG_ENVIRONMENT_NAME}/config.h ${include}/config
    ln -s ${srcdir}/builds/${CONFIG_ENVIRONMENT_NAME}/.config.h ${include}/config
    ln -s ${srcdir}/builds/${CONFIG_ENVIRONMENT_NAME}/wireless/driver/linux/loggroups.h ${include}/config
    ln -s ${srcdir}/builds/${CONFIG_ENVIRONMENT_NAME}/wireless/driver/linux/logmacros.h ${include}/config
fi

