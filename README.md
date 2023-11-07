
[ENGLISH](README_en.md) [中文](README.md)

## 编译测试

测试目标:MT7621 MT7986 IPQ4019 BCM4708

**状态** [![CircleCI](https://dl.circleci.com/status-badge/img/gh/SWRT-dev/swrt-gpl/tree/master.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/gh/SWRT-dev/swrt-gpl/tree/master)

=======

注意：
======

1. **不**要用 **root** 用户 git 和编译！！！

2. 国内用户编译前最好准备好梯子

## 编译

1. 首先装好 Ubuntu 64bit，推荐  Ubuntu  18 LTS x64 /  Mint 19.1

2. 命令行输入 `sudo apt-get update` ，然后输入
   `sudo apt-get -y install build-essential asciidoc binutils bison bzip2 gawk gettext git libncurses5-dev libz-dev patch python3.5 python2.7 unzip zlib1g-dev lib32gcc1 libc6-dev-i386 subversion flex uglifyjs git-core gcc-multilib p7zip p7zip-full msmtp libssl-dev texinfo libglib2.0-dev xmlto qemu-utils upx libelf-dev autoconf automake libtool autopoint device-tree-compiler g++-multilib antlr3 gperf wget libncurses5:i386 libelf1:i386 lib32z1 lib32stdc++6 gtk-doc-tools intltool binutils-dev cmake lzma liblzma-dev lzma-dev uuid-dev liblzo2-dev xsltproc dos2unix libstdc++5 docbook-xsl-* sharutils autogen shtool gengetopt libltdl-dev libtool-bin`

3. 使用 `git clone https://github.com/SWRT-dev/swrt-gpl` 命令下载好源代码
   
#### MTK7621

4. 使用 `git clone https://github.com/SWRT-dev/mtk-toolchains` 命令下载toolchains

5. 分别执行 `cd mtk-toolchains`
   `sudo ln -sf $(pwd)/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.24 /opt/`

6. 然后 `cd ../swrt-gpl/release/src-mtk-mips` 进入目录

7. 输入 `make rt-ax53u` 即可开始编译你要的固件了。
   
   rt-ac85p : `make rt-ac85p`
   
   r6800 : `make swrt-r6800`
   
   rm-ac2100 : `make swrt-rm-ac2100`
   
   jcg-q10pro : `make swrt-jcg-q10pro`
   
   h3c-tx1801 : `make swrt-h3c-tx1801`

   xm-cr660x : `make swrt-xm-cr660x`

   jcg-q20 : `make swrt-jcg-q20`

8. 编译完成后输出固件路径：swrt-gpl/release/src-mtk-mips/image

#### MTK7986

4. 使用 `git clone https://github.com/SWRT-dev/mtk-toolchains` 命令下载toolchains

5. 分别执行 `cd mtk-toolchains`
   `sudo ln -sf $(pwd)/toolchain-aarch64_cortex-a53_gcc-8.4.0_musl /opt/`

6. 然后 `cd ../swrt-gpl/release/src-mtk-arm` 进入目录

7. 输入 `make swrt-rm-ax6000` 即可开始编译你要的固件了。

8. 编译完成后输出固件路径：swrt-gpl/release/src-mtk-arm/image

#### QCA

4. 使用 `git clone https://github.com/SWRT-dev/qca-toolchains` 命令下载toolchains

5. 分别执行 `cd qca-toolchains`
	`sudo ln -sf $(pwd)/openwrt-gcc520_musl.arm /opt/`

6. 然后 `cd ../swrt-gpl/release/src-qca-ipq40xx` 进入目录

7. 输入 `make rt-ac82u` 即可开始编译你要的固件了。

	rt-ac95u : `make rt-ac95u`

8. 编译完成后输出固件路径：swrt-gpl/release/src-qca-ipq40xx/image


#### BCM470X

4. 然后 `cd ../swrt-gpl/release/src-bcm-470x` 进入目录

5. 输入 `make swrt-sbrac3200p` 即可开始编译你要的固件了。

	r7000p : `make swrt-r7000p`

6. 编译完成后输出固件路径：swrt-gpl/release/src-bcm-470x/image

## Donate

如果你觉得此项目对你有帮助，请捐助我们，以使项目能持续发展，更加完善。

### PayPal

[![Support via PayPal](https://cdn.rawgit.com/twolfson/paypal-github-button/1.0.0/dist/button.svg)](https://paypal.me/paldier9/)

### Alipay 支付宝

![alipay](doc/alipay_donate.jpg)

### Wechat 微信

![wechat](doc/wechat_donate.jpg)


