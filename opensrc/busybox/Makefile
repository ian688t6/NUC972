CURDIR := $(shell pwd)
OPEN_SOURCE := busybox-1.22.1

.PHONY: all clean install

all:
	@echo "build $(BUILDDIR)/$(OPEN_SOURCE)"
	@if [ ! -d $(BUILDDIR)/$(OPEN_SOURCE) ]; then \
		echo "make $(OPEN_SOURCE)"; \
		tar -jxf $(OPEN_SOURCE).tar.bz2 -C $(BUILDDIR); \
	fi
	cp -rf config $(BUILDDIR)/$(OPEN_SOURCE)/.config;
	cd $(BUILDDIR)/$(OPEN_SOURCE); make -j8 CROSS_COMPILE=$(CROSS_COMPILE);make install; cd -;

clean:
	@echo "clean $(CURDIR)"
	@if [ -d $(BUILDDIR)/$(OPEN_SOURCE) ]; then \
		make -C $(BUILDDIR)/$(OPEN_SOURCE) CROSS_COMPILE=$(CROSS_COMPILE) clean; \
	fi

install:
	@echo "install $(CURDIR)"
	@cp -rf $(BUILDDIR)/$(OPEN_SOURCE)/_install/* $(BUILDDIR)/rootfs;

