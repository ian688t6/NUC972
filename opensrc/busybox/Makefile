include $(TOPDIR)/subdir.mk

CURDIR := $(shell pwd)
BUSYBOX_SOURCE := busybox-1.22.1

.PHONY: all clean install

all:
	@echo "make $(BUSYBOX_SOURCE)"; 
	@cp -rf busybox-config $(BUSYBOX_SOURCE)/.config	
	cd $(BUSYBOX_SOURCE); make -j8 CROSS_COMPILE=$(CROSS_COMPILE);make install; cd -;

clean:
	@echo "clean $(CURDIR)"
	make -C $(BUSYBOX_SOURCE) CROSS_COMPILE=$(CROSS_COMPILE) clean;

install:
	@echo "install $(CURDIR)"
	@cp -rf $(BUSYBOX_SOURCE)/_install/* $(BUILDDIR)/rootfs;

