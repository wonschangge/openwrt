#
# Copyright (C) 2024 kylinos.cn
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zchunk
PKG_DISTNAME:=$(PKG_NAME)
PKG_VERSION:=1.3.2
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/zchunk/zchunk.git
PKG_SOURCE_VERSION:=bdd6d6fc150d53d89658bebbfa73a7bfd9a89cb4

PKG_LICENSE:=BSD 2-Clause

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/meson.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=zchunk lib
  URL:=$(PKG_SOURCE_URL)
endef

define Package/$(PKG_NAME)/description
A file format designed for highly efficient deltas while
maintaining good compression.

endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/zck.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libzck.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
