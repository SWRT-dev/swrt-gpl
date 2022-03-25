include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=i2c-pct2075
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/i2c-pct2075
  CATEGORY:=DNI Packages
  TITLE:=Driver for I2C Thermal Sensor
  VERSION:=$(LINUX_VERSION)-$(PKG_RELEASE)
  FILES:= \
	$(PKG_BUILD_DIR)/i2c-pct2075.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,i2c-pct2075)
endef

ENET_MAKEOPTS= \
                ENETDIR="$(PKG_BUILD_DIR)" \
                PATH="$(KERNEL_STAGING_DIR)/bin:$(TARGET_PATH)" \
                ARCH="$(LINUX_KARCH)" \
                CROSS_COMPILE="$(TARGET_CROSS)"

define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) KERNELPATH="$(LINUX_DIR)" SUBDIRS=$(PKG_BUILD_DIR) MODVERDIR=$(PKG_BUILD_DIR)/modules modules $(ENET_MAKEOPTS)
endef

define Build/InstallDev
endef

define Build/UninstallDev
endef

define KernelPackage/i2c-pct2075/install
endef

$(eval $(call KernelPackage,i2c-pct2075))
