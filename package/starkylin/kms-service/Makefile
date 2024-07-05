include $(TOPDIR)/rules.mk

PKG_NAME:=kms-service
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

PKG_FLAGS:=nonshared
PKG_BUILD_FLAGS:=lto

include $(INCLUDE_DIR)/package.mk

TARGET_LDFLAGS += -lcurl -lssl -lcrypto -lmosquitto -lprotobuf -lstdc++ -ljsoncpp -DLOG_USE_COLOR

define Package/kms-service
  SECTION:=utils
  CATEGORY:=Starkylin embedded
  TITLE:=kms service for embedded devices
  MAINTAINER:=Chanj Wons <wangchangqi@kylinos.cn>
  DEPENDS:=+curl +libopenssl +libmosquitto-ssl +jsoncpp +protobuf
endef

define Package/kms-service/description
 This package contains kms-service.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/kms-service/install
  $(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kms-service $(1)/usr/sbin/kms-service
endef

$(eval $(call BuildPackage,kms-service))
