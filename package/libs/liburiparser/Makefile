#
# Copyright (C) 2024 kylinos.cn
#

include $(TOPDIR)/rules.mk

PKG_NAME:=liburiparser
PKG_DISTNAME:=$(PKG_NAME)
PKG_VERSION:=0.8.4
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/uriparser/uriparser.git
PKG_SOURCE_VERSION:=1e99d3296bd3a0ae55555e227b49e821ee933c5a

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=RFC 3986 URI parsing
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=+zlib +libopenssl
endef

define Package/$(PKG_NAME)/description
uriparser is a strictly RFC 3986 compliant URI parsing and
handling library written in C.

https://uriparser.github.io

endef

CONFIGURE_ARGS += \
	--disable-test

# TODO: needs install doxygen & graphviz on host
# 	    needs update newest as buildroot

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/uriparser
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/uriparser/*.h $(1)/usr/include/uriparser
	$(CP) $(PKG_BUILD_DIR)/.libs/liburiparser.so* $(1)/usr/lib/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/.libs/liburiparser.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
