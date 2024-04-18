#
# Copyright (C) 2024 kylinos.cn
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libwebsockets
PKG_DISTNAME:=$(PKG_NAME)
PKG_VERSION:=4.3.3
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/warmcat/libwebsockets.git
PKG_SOURCE_VERSION:=4415e84c095857629863804e941b9e1c2e9347ef

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=lightweight websockets lib
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=+zlib +libopenssl
endef

define Package/$(PKG_NAME)/description
Libwebsockets is a lightweight pure C library built to use
minimal CPU and memory resources, and provide fast throughput
in both directions.

https://libwebsockets.org/

endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(CP) -r $(PKG_INSTALL_DIR)/usr/include/libwebsockets/ $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwebsockets.so* $(1)/usr/lib/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwebsockets.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
