#!/bin/sh

set -x
[ $# -eq 7 ] || {
    echo "SYNTAX: $0 <file> <persistentfs image> <bootfsA image> <bootfsB image> <rootfs image> <bootfs size> <rootfs size>"
    exit 1
}

OUTPUT="$1"
PERSISTENTFS="$2"
BOOTAFS="$3"
BOOTBFS="$4"
ROOTFS="$5"
BOOTFSSIZE="$6"
ROOTFSSIZE="$7"
PERSISTENTFSSIZE=64

head=4
sect=63
align=1024

# ptgen - OpenWrt自带工具 - 划定分区表
set $(ptgen \
    -v \
    -e \
    -o $OUTPUT \
    -h $head \
    -s $sect \
    -l $align \
    -N persistent   -t c        -p ${PERSISTENTFSSIZE}M \
    -N boota        -t c        -p ${BOOTFSSIZE}M \
    -N bootb        -t c        -p ${BOOTFSSIZE}M \
    -N roota        -t 83       -p ${ROOTFSSIZE}M \
    -N rootb        -t 83       -p ${ROOTFSSIZE}M
)

# 起始偏移量 8192 * 512 / 1024 / 1024 = 4M
# 每区块为 512 字节
ALIGN_OFFSET="$(($align * 2))"
PERSISTENTFS_OFFSET="$(($PERSISTENTFSSIZE * 1024 * 1024 / 512))"
BOOTFS_OFFSET="$(($BOOTFSSIZE * 1024 * 1024 / 512))"
ROOTFS_OFFSET="$(($ROOTFSSIZE * 1024 * 1024 / 512))"

# MBR1
PERSISTENT_OFFSET="$ALIGN_OFFSET"
# MBR2
BOOTA_OFFSET="$(($PERSISTENT_OFFSET + $PERSISTENTFS_OFFSET + $ALIGN_OFFSET))"
# MBR3
BOOTB_OFFSET="$(($BOOTA_OFFSET + $BOOTFS_OFFSET + $ALIGN_OFFSET))"
# EBR1
ROOTA_OFFSET="$(($BOOTB_OFFSET + $BOOTFS_OFFSET + $ALIGN_OFFSET + $ALIGN_OFFSET))"
# EBR2
ROOTB_OFFSET="$(($ROOTA_OFFSET + $ROOTFS_OFFSET + $ALIGN_OFFSET))"

# 写入 boot.img、rootfs.img。区块大小必须是512。
# persistent
dd bs=512 if="$PERSISTENTFS" of="$OUTPUT" seek="$PERSISTENT_OFFSET" conv=notrunc
# bootA
dd bs=512 if="$BOOTAFS" of="$OUTPUT" seek="$BOOTA_OFFSET" conv=notrunc
# bootB
dd bs=512 if="$BOOTBFS" of="$OUTPUT" seek="$BOOTB_OFFSET" conv=notrunc
# rootA
dd bs=512 if="$ROOTFS" of="$OUTPUT" seek="$ROOTA_OFFSET" conv=notrunc
# rootB
dd bs=512 if="$ROOTFS" of="$OUTPUT" seek="$ROOTB_OFFSET" conv=notrunc



