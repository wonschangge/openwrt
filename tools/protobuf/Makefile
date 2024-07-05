#
# Copyright (C) 2007-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=protobuf
PKG_VERSION:=3.17.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-cpp-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/google/protobuf/releases/download/v$(PKG_VERSION)
PKG_HASH:=51cec99f108b83422b7af1170afd7aeb2dd77d2bcbb7b6bad1f92509e9ccf8cb

PKG_MAINTAINER:=Ken Keys <kkeys@caida.org>
PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE
PKG_CPE_ID:=cpe:/a:google:protobuf

CMAKE_SOURCE_SUBDIR:=cmake

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_HOST_OPTIONS += \
	-Dprotobuf_BUILD_PROTOC_BINARIES=ON \
	-Dprotobuf_BUILD_TESTS=OFF \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_SKIP_RPATH=OFF \
	-DCMAKE_INSTALL_LIBDIR=lib \
	-DCMAKE_INSTALL_RPATH="${STAGING_DIR_HOSTPKG}/lib"

CMAKE_OPTIONS += \
	-Dprotobuf_BUILD_PROTOC_BINARIES=ON \
	-Dprotobuf_BUILD_TESTS=OFF \
	-Dprotobuf_WITH_ZLIB=ON \
	-DBUILD_SHARED_LIBS=ON

TARGET_LDFLAGS += -latomic $(if $(CONFIG_USE_GLIBC),-lm)

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/protoc $(STAGING_DIR_HOST)/bin/protoc
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/lib/google/protobuf
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/src/google/protobuf/any.proto $(STAGING_DIR_HOST)/lib/google/protobuf/any.proto
endef

define Host/Clean
	rm -rf $(STAGING_DIR_HOST)/bin/protoc
endef

$(eval $(call HostBuild))
