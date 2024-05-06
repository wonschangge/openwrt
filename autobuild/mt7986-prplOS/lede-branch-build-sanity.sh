#!/bin/bash
source ./autobuild/lede-build-sanity.sh

#get the brach_name
temp=${0%/*}
branch_name=${temp##*/}

rm -rf ${BUILD_DIR}/package/network/services/hostapd
cp -fpR ${BUILD_DIR}/./../mac80211_package/package/network/services/hostapd ${BUILD_DIR}/package/network/services

rm -rf ${BUILD_DIR}/package/libs/libnl-tiny
cp -fpR ${BUILD_DIR}/./../mac80211_package/package/libs/libnl-tiny ${BUILD_DIR}/package/libs

rm -rf ${BUILD_DIR}/package/network/utils/iw
cp -fpR ${BUILD_DIR}/./../mac80211_package/package/network/utils/iw ${BUILD_DIR}/package/network/utils

rm -rf ${BUILD_DIR}/package/network/utils/iwinfo
cp -fpR ${BUILD_DIR}/./../mac80211_package/package/network/utils/iwinfo ${BUILD_DIR}/package/network/utils

rm -rf ${BUILD_DIR}/package/kernel/mac80211
cp -fpR ${BUILD_DIR}/./../mac80211_package/package/kernel/mac80211 ${BUILD_DIR}/package/kernel

cp -fpR ${BUILD_DIR}/./../mac80211_package/package/kernel/mt76 ${BUILD_DIR}/package/kernel

#prplos bypass some feed patch
rm -f ${BUILD_DIR}/autobuild/openwrt_patches-21.02/mtk_soc/09*.patch
rm -f ${BUILD_DIR}/autobuild/openwrt_patches-21.02/mtk_soc/8000-uboot-mediatek-makefile.patch

#do prepare stuff
prepare

#hack mt7986 config5.4
echo "CONFIG_NETFILTER=y" >> ./target/linux/mediatek/mt7986/config-5.4
echo "CONFIG_NETFILTER_ADVANCED=y" >> ./target/linux/mediatek/mt7986/config-5.4
echo "CONFIG_RELAY=y" >> ./target/linux/mediatek/mt7986/config-5.4

#hack hostapd config
echo "CONFIG_MBO=y" >> ./package/network/services/hostapd/files/hostapd-full.config
echo "CONFIG_WPS_UPNP=y"  >> ./package/network/services/hostapd/files/hostapd-full.config

prepare_mac80211

#prepare_final ${branch_name}
rm -rf ${BUILD_DIR}/target/linux/mediatek/patches-5.4/0303-mtd-spinand-disable-on-die-ECC.patch

#prepare prplOS 
#refactor feed config
echo "src-git packages https://gerrit.mediatek.inc/openwrt/feeds/packages;openwrt-21.02" > ${BUILD_DIR}/feeds.conf.default
echo "src-git luci https://gerrit.mediatek.inc/openwrt/feeds/luci;openwrt-21.02" >> ${BUILD_DIR}/feeds.conf.default
echo "src-git routing https://gerrit.mediatek.inc/openwrt/feeds/routing;openwrt-21.02" >> ${BUILD_DIR}/feeds.conf.default
echo "src-git telephony https://git.openwrt.org/feed/telephony.git;openwrt-21.02" >> ${BUILD_DIR}/feeds.conf.default
echo "src-git mtk_openwrt_feed https://git01.mediatek.com/openwrt/feeds/mtk-openwrt-feeds" >> ${BUILD_DIR}/feeds.conf.default
#Official latest prplos
#git clone https://gitlab.com/prpl-foundation/prplos/prplos.git
#cp prplos/scripts/gen_config.py ${BUILD_DIR}/scripts/
#cp prplos/scripts/feed ${BUILD_DIR}/scripts/
#cp -r prplos/profiles ${BUILD_DIR}/

#Internal good tag prplos yml/script
cp ${BUILD_DIR}/autobuild/${branch_name}/scripts_20220518/gen_config.py ${BUILD_DIR}/scripts
cp ${BUILD_DIR}/autobuild/${branch_name}/scripts_20220518/feeds ${BUILD_DIR}/scripts
cp -r ${BUILD_DIR}/autobuild/${branch_name}/profiles_20220518 ${BUILD_DIR}/profiles

# prplos build
cp ${BUILD_DIR}/autobuild/${branch_name}/mtk_profiles/*.* ${BUILD_DIR}/profiles
${BUILD_DIR}/scripts/gen_config.py prpl sah mmx mt7986

# do internal prplos patch 
do_patch ${BUILD_DIR}/autobuild/${branch_name}/patches || exit 1
mkdir -p ${BUILD_DIR}/feeds/feed_prpl/ambiorix/libs/libsahtrace/patches
cp ${BUILD_DIR}/autobuild/${branch_name}/999-fix-install-libsahtrace-error.patch ${BUILD_DIR}/feeds/feed_prpl/ambiorix/libs/libsahtrace/patches
rm ${BUILD_DIR}/package/network/config/netifd/files/etc/udhcpc.user
echo "CONFIG_PACKAGE_wpad-openssl=y" >> ${BUILD_DIR}/.config
echo "CONFIG_PACKAGE_hostapd-utils=y" >> ${BUILD_DIR}/.config
echo "CONFIG_PACKAGE_prplmesh=y" >> ${BUILD_DIR}/.config
#endif prplos patch

make defconfig
make -j8

#step2 build
#if [ -z ${1} ]; then
#	build ${branch_name} -j1 || [ "$LOCAL" != "1" ]
#fi
