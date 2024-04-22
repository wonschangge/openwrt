#
# Copyright (C) 2024 kylinos.cn
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libarchive
PKG_DISTNAME:=$(PKG_NAME)
PKG_VERSION:=3.7.2
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/libarchive/libarchive.git
PKG_SOURCE_VERSION:=6468cd1f5c9b76e2c3b10cdd6938faf6b82823b6

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=rw streaming archive formats
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=+zlib +libopenssl +libxml2
endef

define Package/$(PKG_NAME)/description
Libarchive is a reusable C library for reading and writing a
variety of streaming archive formats.

https://www.libarchive.org/

endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libarchive.so* $(1)/usr/lib/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libarchive.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
