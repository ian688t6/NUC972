CURDIR := $(shell pwd)
KERNEL_SOURCE := linux-3.10.y

.PHONY: all clean install

all:
	@echo "build $(CURDIR)"
	@if [ -d $(KERNEL_SOURCE) ]; then \
		echo "make $(KERNEL_SOURCE)"; \
		cp -rf patch/* $(KERNEL_SOURCE)/; \
		make -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) nuc972_defconfig; \
		cd $(KERNEL_SOURCE); make -j8 ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) uImage; cd -; \
	fi

clean:
	@echo "clean $(CURDIR)"
	@if [ -d $(KERNEL_SOURCE) ]; then \
		make -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) clean; \
	fi

install:
	@echo "install $(CURDIR)"
	@cp $(KERNEL_SOURCE)/arch/arm/boot/uImage $(OUTDIR)/bin

