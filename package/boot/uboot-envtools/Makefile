#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uboot-envtools
PKG_DISTNAME:=u-boot
PKG_VERSION:=2023.04
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
    https://ftp.denx.de/pub/u-boot \
    https://mirror.cyberbits.eu/u-boot \
    ftp://ftp.denx.de/pub/u-boot
PKG_HASH:=e31cac91545ff41b71cec5d8c22afd695645cd6e2a442ccdacacd60534069341
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_DISTNAME)-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=fstools

PKG_LICENSE:=GPL-2.0 GPL-2.0+
PKG_LICENSE_FILES:=Licenses/README

PKG_FLAGS:=nonshared

PKG_BUILD_PARALLEL:=1

PKG_FILE_DEPENDS += $(GENERIC_PLATFORM_DIR)/base-files $(PLATFORM_COMMON_DIR)/base-files $(PLATFORM_DIR)/base-files

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/uboot-envtools
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Boot Loaders
  TITLE:=read/modify U-Boot bootloader environment
  URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/uboot-envtools/description
 This package includes tools to read and modify U-Boot bootloader environment.
endef

define Build/Configure
	touch $(PKG_BUILD_DIR)/include/config.h
	mkdir -p $(PKG_BUILD_DIR)/include/config
	touch $(PKG_BUILD_DIR)/include/config/auto.conf
	mkdir -p $(PKG_BUILD_DIR)/include/generated
	touch $(PKG_BUILD_DIR)/include/generated/autoconf.h
endef

MAKE_FLAGS += \
	TARGET_CFLAGS="$(TARGET_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS)" \
	no-dot-config-targets=envtools \
	envtools

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		TARGET_CFLAGS="$(TARGET_CFLAGS)" \
		TARGET_LDFLAGS="$(TARGET_LDFLAGS)" \
		no-dot-config-targets=envtools \
		envtools
endef

define Package/uboot-envtools/conffiles
/etc/config/ubootenv
/etc/fw_env.config
/etc/fw_sys.config
endef

define Package/uboot-envtools/install
	$(INSTALL_DIR) $(STAGING_DIR)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/env/lib.a $(STAGING_DIR)/usr/lib/libubootenv.a
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/env/*.o $(STAGING_DIR)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/env/fw_printenv $(1)/usr/sbin
	$(LN) fw_printenv $(1)/usr/sbin/fw_setenv
	$(INSTALL_BIN) ./files/fw_printsys $(1)/usr/sbin
	$(INSTALL_BIN) ./files/fw_setsys $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DATA) ./files/uboot-envtools.sh $(1)/lib
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/fw_env.config $(1)/etc
	if [ -e $(GENERIC_PLATFORM_DIR)/base-files/etc/fw_env.config ]; then \
		$(INSTALL_DATA) $(GENERIC_PLATFORM_DIR)/base-files/etc/fw_env.config $(1)/etc; \
	fi
	if [ -e $(PLATFORM_COMMON_DIR)/base-files/etc/fw_env.config ]; then \
		$(INSTALL_DATA) $(PLATFORM_COMMON_DIR)/base-files/etc/fw_env.config $(1)/etc; \
	fi
	if [ -e $(PLATFORM_DIR)/base-files/etc/fw_env.config ]; then \
		$(INSTALL_DATA) $(PLATFORM_DIR)/base-files/etc/fw_env.config $(1)/etc; \
	fi
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(if $(wildcard ./files/$(BOARD)_$(SUBTARGET)), \
		$(INSTALL_DATA) ./files/$(BOARD)_$(SUBTARGET) \
		$(1)/etc/uci-defaults/30_uboot-envtools, \
		$(if $(wildcard ./files/$(BOARD)), \
			$(INSTALL_DATA) ./files/$(BOARD) \
			$(1)/etc/uci-defaults/30_uboot-envtools \
		) \
	)
endef

$(eval $(call BuildPackage,uboot-envtools))
