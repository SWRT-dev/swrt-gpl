
NOTE：
=
1. **DO NOT USE** **root** user for git or compilation!!!
2. if you are in china, you need a network proxy

## Compilation

1. Install Ubuntu 64bit，recommend Ubuntu 18 LTS x64 / Mint 19.1

2. Enter `sudo apt-get update` in terminal, then enter
`
sudo apt-get -y install build-essential asciidoc binutils bzip2 gawk gettext git libncurses5-dev libz-dev patch python3.5 python2.7 unzip zlib1g-dev lib32gcc1 libc6-dev-i386 subversion flex uglifyjs git-core gcc-multilib p7zip p7zip-full msmtp libssl-dev texinfo libglib2.0-dev xmlto qemu-utils upx libelf-dev autoconf automake libtool autopoint device-tree-compiler g++-multilib antlr3 gperf wget libncurses5:i386 libelf1:i386 lib32z1 lib32stdc++6 gtk-doc-tools intltool binutils-dev cmake lzma liblzma-dev lzma-dev uuid-dev liblzo2-dev xsltproc dos2unix libstdc++5 docbook-xsl-* sharutils autogen shtool gengetopt libltdl-dev libtool-bin
`

3. Enter `git clone https://github.com/SWRT-dev/rtax53u` to download source code 

4. Enter `git clone https://github.com/SWRT-dev/mtk-toolchains` to download toolchains

5. Enter the following commands step by step `cd mtk-toolchains`

	`sudo ln -sf $(pwd)/toolchain-aarch64_cortex-a53+neon-vfpv4_gcc-5.4.0_glibc-2.24 /opt/`

	`sudo ln -sf $(pwd)/lede-toolchain-mediatek-mt7629_gcc-5.4.0_musl-1.1.24_eabi.Linux-x86_64 /opt/`

	`sudo ln -sf $(pwd)/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.24 /opt/`

	`sudo ln -sf $(pwd)/toolchain-aarch64_cortex-a53_gcc-8.4.0_glibc /opt/`

	mt7622 continue to the next step

	`cd toolchain-aarch64_cortex-a53+neon-vfpv4_gcc-5.4.0_glibc-2.24/lib`

	`tar xvJf libc.a.tar.xz`

6. Then enter `cd ../rtax53u/release/src-mtk-mips` to enter folder

7. Enter `make rt-ax53u` to start compiling the firmware

	rt-ac85p : `make rt-ac85p`

	r6800 : `make swrt-r6800`

	rm-ac2100 : `make rm-ac2100`

	jcg-q10pro : `make swrt-jcg-q10pro`

	h3c-tx1801 : `make swrt-h3c-tx1801`

8. The firmware is in rtax53u/release/src-mtk-mips/image

## Donate

If you think this project is helpful to you, please donate to us so that the project can continue to develop and become more complete. 

### PayPal

[![Support via PayPal](https://cdn.rawgit.com/twolfson/paypal-github-button/1.0.0/dist/button.svg)](https://paypal.me/paldier9/)

### Alipay 支付宝

![alipay](doc/alipay_donate.jpg)

### Wechat 微信
  
![wechat](doc/wechat_donate.jpg)


