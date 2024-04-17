#
# Copyright (C) 2024 kylinos.cn
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libubootenv
PKG_DISTNAME:=$(PKG_NAME)
PKG_VERSION:=0.3.5
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/sbabic/libubootenv.git
PKG_SOURCE_VERSION:=3f4d15e36ceb58085b08dd13f3f2788e9299877b

PKG_LICENSE:=MIT LGPL-2.1+ CC0-1.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

PKG_BUILD_DEPENDS:=libyaml zlib

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ubootenv lib
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=+libyaml +zlib
endef

define Package/$(PKG_NAME)/description
libubootenv is a library that provides a hardware independent 
way to access to U-Boot environment.
 
The package provides the fw_printenv / fw_setenv utilities, 
new tools build from the library and not from U-Boot.

endef

define Build/Prepare
	$(call Build/Prepare/Default)
	(cd $(PKG_BUILD_DIR) && cmake $(PKG_BUILD_DIR))
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/local/include/libuboot.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/local/lib/libubootenv.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
