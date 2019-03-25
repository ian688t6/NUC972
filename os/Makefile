CURDIR := $(shell pwd)
KERNEL_SOURCE := linux-3.10.y

.PHONY: all clean install

all:
	@if [ -d $(BUILDDIR)/$(KERNEL_SOURCE) ]; then \
		echo "make $(KERNEL_SOURCE)"; \
		cp -rf patch/* $(BUILDDIR)/$(KERNEL_SOURCE)/; \
		make -C $(BUILDDIR)/$(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) $(KERN_CONFIG); \
		cd $(BUILDDIR)/$(KERNEL_SOURCE); make -j8 ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) uImage; cd -; \
	fi

clean:
	@echo "clean $(CURDIR)"
	@if [ -d $(BUILDDIR)/$(KERNEL_SOURCE) ]; then \
		make -C $(BUILDDIR)/$(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) clean; \
	fi

install:
	@echo "install $(CURDIR)"
	@cp $(BUILDDIR)/$(KERNEL_SOURCE)/arch/arm/boot/uImage $(OUTDIR)/bin

