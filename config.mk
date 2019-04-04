# prepare param

export TOPDIR=$(shell pwd)
export OUTDIR=$(TOPDIR)/out
export BUILDDIR=$(TOPDIR)/build
export OSDIR=$(TOPDIR)/os
export BOOTDIR=$(TOPDIR)/boot
export APPSDIR=$(TOPDIR)/apps
export TOOLDIR=$(TOPDIR)/tools
export LIBDIR=$(TOPDIR)/libs
export OPENSRCDIR=$(TOPDIR)/opensrc

export KERN_CONFIG=nuc972_defconfig
export BOOT_CONFIG=nuc970_defconfig

export OS_LDFLAGS := -L$(BUILDDIR)/lib
export CCFLAGS := -Os -Wall -Werror
export OS_INCLUDES := -I$(OSDIR)/include

export CC 				= gcc
export AR 				= ar
export LD 				= ld
export STRIP 			= strip
export OBJDUMP 			= objdump
export NM 				= nm
export CROSS_COMPILE	= arm-linux

