#
# Copyright (C) 2024 kylinos.cn
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libyaml
PKG_DISTNAME:=$(PKG_NAME)
PKG_VERSION:=0.2.5
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/yaml/libyaml.git
PKG_SOURCE_VERSION:=2c891fc7a770e8ba2fec34fc6b545c672beb37e6

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=yaml lib
  URL:=$(PKG_SOURCE_URL)
endef

define Package/$(PKG_NAME)/description
LibYAML is a YAML 1.1 parser and emitter written in C.

http://pyyaml.org/wiki/LibYAML

endef

define Build/Prepare
	$(call Build/Prepare/Default)
	(cd $(PKG_BUILD_DIR) && cmake $(PKG_BUILD_DIR))
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/local/include/yaml.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/local/lib/libyaml.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
