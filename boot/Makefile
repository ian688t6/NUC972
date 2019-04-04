CURDIR := $(shell pwd)
UBOOT_SOURCE := u-boot-201611

.PHONY: all clean install

all:
	@echo "build $(CURDIR)"
	@if [ -d $(BUILDDIR)/$(UBOOT_SOURCE) ]; then \
		echo "make $(UBOOT_SOURCE)"; \
		cp -rf patch/* $(BUILDDIR)/$(UBOOT_SOURCE)/; \
		make -C $(BUILDDIR)/$(UBOOT_SOURCE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) $(BOOT_CONFIG); \
		cd $(BUILDDIR)/$(UBOOT_SOURCE); make -j8 ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE); cd -; \
	fi

clean:
	@echo "clean $(CURDIR)"
	@if [ -d $(BUILDDIR)/$(UBOOT_SOURCE) ]; then \
		make -C $(BUILDDIR)/$(UBOOT_SOURCE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) clean; \
	fi

install:
	@echo "install $(CURDIR)"
	@cp $(BUILDDIR)/$(UBOOT_SOURCE)/u-boot.bin $(OUTDIR)/bin
	@cp $(BUILDDIR)/$(UBOOT_SOURCE)/spl/u-boot-spl.bin $(OUTDIR)/bin

