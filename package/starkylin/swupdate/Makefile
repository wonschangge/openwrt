#
#Copyright (c) 2018-2020 Allwinner Technology Co. Ltd.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=swupdate
PKG_DISTNAME:=swupdate
PKG_VERSION:=2023.12
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/sbabic/swupdate.git
PKG_SOURCE_VERSION:=2042e6dc0f5466bd428f4db18eaff638203150ad

SWUPDATE_SYM=$(if $(CONFIG_SWUPDATE_CUSTOM),CONFIG,DEFAULT)
SWUPDATE_IF_ENABLED=$(if $(CONFIG_SWUPDATE_$(SWUPDATE_SYM)_$(1)),$(2))

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_CFLAGS += -I$(STAGING_DIR)/include
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib
# always compiled
TARGET_LDLIBS += z yaml json-c config ubootenv

ifeq ($(CONFIG_SWUPDATE_CHANNEL_CURL),y)
  TARGET_LDLIBS += curl
else ifeq ($(CONFIG_SWUPDATE_DOCKER),y)
  TARGET_LDLIBS += curl
endif

ifeq ($(CONFIG_SWUPDATE_SSL_IMPL_OPENSSL),y)
  TARGET_LDLIBS += ssl crypto
endif

ifeq ($(CONFIG_SWUPDATE_DOWNLOAD),y)
  TARGET_LDLIBS += curl
endif

ifeq ($(CONFIG_SWUPDATE_SURICATTA),y)
  TARGET_LDLIBS += curl
endif

ifeq ($(CONFIG_SWUPDATE_RDIFFHANDLER),y)
  TARGET_LDLIBS += rsync
endif

ifeq ($(CONFIG_SWUPDATE_DELTA),y)
  TARGET_LDLIBS += zck zstd curl
endif

ifeq ($(CONFIG_SWUPDATE_ZSTD),y)
  TARGET_LDLIBS += zstd
endif

ifeq ($(CONFIG_SWUPDATE_SWUFORWARDER_HANDLER),y)
  TARGET_LDLIBS += curl websockets uriparser
endif

ifeq ($(CONFIG_SWUPDATE_LUA),y)
  TARGET_LDLIBS += lua
endif

ifeq ($(CONFIG_SWUPDATE_SURICATTA_LUA),y)
  TARGET_LDLIBS += lua
endif

ifeq ($(CONFIG_SWUPDATE_ARCHIVE),y)
  TARGET_LDLIBS += archive
endif

ifeq ($(CONFIG_SWUPDATE_DISKPART),y)
  TARGET_LDLIBS += fdisk
endif

ifeq ($(CONFIG_SWUPDATE_DISKPART_FORMAT),y)
  TARGET_LDLIBS += blkid
endif

ifeq ($(SWUPDATE_DISKFORMAT_HANDLER),y)
  TARGET_LDLIBS += blkid
endif

ifeq ($(CONFIG_SWUPDATE_UNIQUEUUID),y)
  TARGET_LDLIBS += blkid
endif

ifeq ($(CONFIG_SWUPDATE_EXT_FILESYSTEM),y)
  TARGET_LDLIBS += ext2fs uuid blkid ss com_err
endif

ifeq ($(CONFIG_SWUPDATE_BTRFS_FILESYSTEM),y)
  TARGET_LDLIBS += btrfs btrfsutil
endif

ifeq ($(CONFIG_SWUPDATE_BTRFS_FILESYSTEM_USELIBMKFS),y)
  TARGET_LDLIBS += btrfs btrfsutil
endif

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Starkylin embedded
  TITLE:=software update for embedded system
  DEFAULT:=y if TARGET_bcm27xx
  DEPENDS:=+zlib +libjson-c libconfig libubootenv \
  +SWUPDATE_DOWNLOAD:curl \
  +SWUPDATE_CHANNEL_CURL:curl \
  +SWUPDATE_DOCKER:curl \
  +SWUPDATE_DELTA:curl \
  +SWUPDATE_SURICATTA:curl \n
  +SWUPDATE_SSL_IMPL_OPENSSL:libopenssl \
  +SWUPDATE_RDIFFHANDLER:librsync \
  +SWUPDATE_DELTA:zchunk \
  +SWUPDATE_DELTA:zstd \
  +SWUPDATE_ZSTD:zstd \
  +SWUPDATE_SWUFORWARDER_HANDLER:curl \
  +SWUPDATE_SWUFORWARDER_HANDLER:libwebsockets \
  +SWUPDATE_SWUFORWARDER_HANDLER:liburiparser \
  +SWUPDATE_LUA:lua \
  +SWUPDATE_SURICATTA_LUA:lua \
  +SWUPDATE_ARCHIVE:libarchive \
  +SWUPDATE_DISKPART:libfdisk \
  +SWUPDATE_DISKPART_FORMAT:libblkid \
  +SWUPDATE_DISKFORMAT_HANDLER:libblkid \
  +SWUPDATE_UNIQUEUUID:libblkid \
  +SWUPDATE_EXT_FILESYSTEM:libext2fs \
  +SWUPDATE_BTRFS_FILESYSTEM:btrfs-progs \
  +SWUPDATE_BTRFS_FILESYSTEM_USELIBMKFS:btrfs-progs
  MENU:=1
endef

# +SWUPDATE_$(SWUPDATE_SYM)_MTD:mtd-utils \
# +SWUPDATE_$(SWUPDATE_SYM)_AWBOOT_HANDLER:ota-burnboot \

define Package/$(PKG_NAME)/description
  SWUpdate provides a reliable way to update the software on an embedded system.
  Sources are hosted at https://github.com/sbabic/swupdate
endef

define Package/$(PKG_NAME)/config
	source "$(SOURCE)/Config.in"
endef

# yes 'n' | $(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS) oldconfig
# grep 'CONFIG_SWUPDATE_$(SWUPDATE_SYM)' $(TOPDIR)/.config | sed -e "s,\\(# \)\\?CONFIG_SWUPDATE_$(SWUPDATE_SYM)_\\(.*\\),\\1CONFIG_\\2,g" >> $(PKG_BUILD_DIR)/.config
# $(CP) swupdate.config $(PKG_BUILD_DIR)/.config
define Build/Configure

ifeq ($(CONFIG_SWUPDATE_CUSTOM),y)
	grep 'CONFIG_SWUPDATE' $(TOPDIR)/.config | sed -e "s,\\(# \)\\?CONFIG_SWUPDATE_\\(.*\\),\\1CONFIG_\\2,g" >> $(PKG_BUILD_DIR)/.config
	cat $(PKG_BUILD_DIR)/.config
else
	$(CP) swupdate.config $(PKG_BUILD_DIR)/.config
endif

endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		MAKE="$(MAKE)" \
		ARCH="$(TARGET_ARCH)" \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LD="$(TARGET_CC)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		LDLIBS="$(TARGET_LDLIBS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CONFIG_PREFIX="$(PKG_INSTALL_DIR)" \
		install
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/swupdate
	$(INSTALL_DIR) $(1)/www
	$(CP) ./swupdate/* $(1)/etc/swupdate/
	$(CP) -r ./var/www/swupdate $(1)/www/
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/swupdate $(1)/sbin

	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libswupdate.so* $(1)/usr/lib/

ifneq ($(CONFIG_SWUPDATE_SURICATTA_HAWKBIT),)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/hawkbitcfg $(1)/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/sendtohawkbit $(1)/sbin
endif

ifeq ($(CONFIG_SWUPDATE_CONFIG_REMOVE_PROGRESS),)
########################## Reduce Size - progress
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/swupdate-progress $(1)/sbin
endif
ifeq ($(CONFIG_SWUPDATE_CONFIG_REMOVE_CLIENT),)
########################## Reduce Size - client
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/swupdate-client $(1)/sbin
endif
ifeq ($(CONFIG_SWUPDATE_CONFIG_REMOVE_AUTORUN),)
########################## Reduce Size - autorun
	$(INSTALL_BIN) ./swupdate_autorun.init $(1)/etc/init.d/S99swupdate_autorun
	$(INSTALL_BIN) ./S80swupdate $(1)/etc/init.d/S80swupdate
	$(INSTALL_BIN) ./swupdate_cmd.sh $(1)/sbin
	$(INSTALL_BIN) ./set_boot_partition_a.sh $(1)/sbin
	$(INSTALL_BIN) ./set_boot_partition_b.sh $(1)/sbin
endif

endef

$(eval $(call BuildPackage,$(PKG_NAME)))
