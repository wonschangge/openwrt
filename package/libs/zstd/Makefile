#
# Copyright (C) 2024 kylinos.cn
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zstd
PKG_DISTNAME:=$(PKG_NAME)
PKG_VERSION:=1.4.5
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/facebook/zstd.git
PKG_SOURCE_VERSION:=b706286adbba780006a47ef92df0ad7a785666b6

PKG_LICENSE:=BSD

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=zstd lib
  URL:=$(PKG_SOURCE_URL)
endef

define Package/$(PKG_NAME)/description
Zstandard, or zstd as short version, is a fast lossless
compression algorithm, targeting real-time compression
scenarios at zlib-level and better compression ratios

The selection of other packages will enable some features:
xz, lz4 and/or zlib will enable support for corresponding
compression formats

endef

CONFIGURE_ARGS += \
	--enable-shared

define Build/Compile
	$(call Build/Compile/Default, \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CC="$(TARGET_CC)" \
		lib \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/lib/zstd.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/lib/libzstd.so* $(1)/usr/lib/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/lib/libzstd.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
