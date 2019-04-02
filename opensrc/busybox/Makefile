CURDIR := $(shell pwd)
BUSYBOX_SOURCE := busybox-1.22.1

.PHONY: all clean install

all:
	@echo "build $(BUILDDIR)/$(BUSYBOX_SOURCE)"
	@if [ ! -d $(BUILDDIR)/$(BUSYBOX_SOURCE) ]; then \
		echo "make $(BUSYBOX_SOURCE)"; \
		tar -jxf $(BUSYBOX_SOURCE).tar.bz2 -C $(BUILDDIR); \
	fi
	cp -rf config $(BUILDDIR)/$(BUSYBOX_SOURCE)/.config;
	cd $(BUILDDIR)/$(BUSYBOX_SOURCE); make -j8 CROSS_COMPILE=$(CROSS_COMPILE);make install; cd -;

clean:
	@echo "clean $(CURDIR)"
	@if [ -d $(BUILDDIR)/$(KERNEL_SOURCE) ]; then \
		make -C $(BUILDDIR)/$(BUSYBOX_SOURCE) CROSS_COMPILE=$(CROSS_COMPILE) clean; \
	fi

install:
	@echo "install $(CURDIR)"
	@cp -rf $(BUILDDIR)/$(BUSYBOX_SOURCE)/_install/* $(BUILDDIR)/rootfs;

