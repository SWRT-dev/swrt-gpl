
[ENGLISH](README_en.md) [中文](README.md)

## Code compilation test

Target:MT7986 IPQ4019

**status** [![CircleCI](https://dl.circleci.com/status-badge/img/gh/SWRT-dev/swrt-gpl/tree/master.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/gh/SWRT-dev/swrt-gpl/tree/master)

=======

NOTE：
======

1. **DO NOT USE** **root** user for git or compilation!!!
2. if you are in china, you need a network proxy

## Compilation

1. Install Ubuntu 64bit，Ubuntu 18 LTS x64 and Mint 19.1 are recommended

2. Run `sudo apt-get update` in terminal, and then run

`
sudo apt-get -y install build-essential asciidoc binutils bzip2 gawk gettext git libncurses5-dev libz-dev patch python3.5 python2.7 unzip zlib1g-dev lib32gcc1 libc6-dev-i386 subversion flex uglifyjs git-core gcc-multilib p7zip p7zip-full msmtp libssl-dev texinfo libglib2.0-dev xmlto qemu-utils upx libelf-dev autoconf automake libtool autopoint device-tree-compiler g++-multilib antlr3 gperf wget libncurses5:i386 libelf1:i386 lib32z1 lib32stdc++6 gtk-doc-tools intltool binutils-dev cmake lzma liblzma-dev lzma-dev uuid-dev liblzo2-dev xsltproc dos2unix libstdc++5 docbook-xsl-* sharutils autogen shtool gengetopt libltdl-dev libtool-bin
`

3. Run `git clone https://github.com/SWRT-dev/swrt-gpl` to clone the source code 
   
#### MTK7621

4. Run `git clone https://github.com/SWRT-dev/mtk-toolchains` to clone the toolchains

5. Run `cd mtk-toolchains` to enter the directory, and follow commands step by step 

	`sudo ln -sf $(pwd)/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.24 /opt/`

6. Run `cd ../swrt-gpl/release/src-mtk-mips` to enter the directory

7. Build firmware

	rt-ax53u : `make rt-ax53u`

	rt-ac85p : `make rt-ac85p`

	r6800 : `make swrt-r6800`

	rm-ac2100 : `make swrt-rm-ac2100`

	jcg-q10pro : `make swrt-jcg-q10pro`

	h3c-tx1801 : `make swrt-h3c-tx1801`

	xm-cr660x : `make swrt-xm-cr660x`

8. Build result will be produced to `swrt-gpl/release/src-mtk-mips/image` directory

#### MTK7986

4. Run `git clone https://github.com/SWRT-dev/mtk-toolchains` to clone the toolchains

5. Run `cd mtk-toolchains` to enter the directory, and follow commands step by step 

   `sudo ln -sf $(pwd)/toolchain-aarch64_cortex-a53_gcc-8.4.0_musl /opt/`

6. Run `cd ../swrt-gpl/release/src-mtk-arm` to enter the directory

7. Build firmware

	rm-ax6000 : `make swrt-rm-ax6000`

8. Build result will be produced to `swrt-gpl/release/src-mtk-arm/image` directory

#### QCA

4. Run `git clone https://github.com/SWRT-dev/qca-toolchains` to clone the toolchains

5. Run `cd qca-toolchains` to enter the directory, and follow commands step by step 

	`sudo ln -sf $(pwd)/openwrt-gcc520_musl.arm /opt/`

6. Run `cd ../swrt-gpl/release/src-qca-ipq40xx` to enter the directory

7. Build firmware

	rt-ac82u : `make rt-ac82u`

	rt-ac95u : `make rt-ac95u`

8. Build result will be produced to `swrt-gpl/release/src-qca-ipq40xx/image` directory

This source code is promised to be compiled successfully.

You can use this source code freely, but please link this GitHub repository when redistributing. Thank you for your cooperation!

## Donate

If you think this project is helpful to you, please donate to us so that the project can continue to develop and become more complete. 

### PayPal

[![Support via PayPal](https://cdn.rawgit.com/twolfson/paypal-github-button/1.0.0/dist/button.svg)](https://paypal.me/paldier9/)

### Alipay 支付宝

![alipay](doc/alipay_donate.jpg)

### Wechat 微信
  
![wechat](doc/wechat_donate.jpg)


