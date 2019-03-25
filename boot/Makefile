CURDIR := $(shell pwd)
UBOOT_SOURCE := u-boot-201611

.PHONY: all clean install

all:
	@echo "build $(CURDIR)"
	@if [ -d $(UBOOT_SOURCE) ]; then \
		echo "make $(UBOOT_SOURCE)"; \
	fi

clean:
	@echo "clean $(CURDIR)"
	@if [ -d $(UBOOT_SOURCE) ]; then \
		make -C $(UBOOT_SOURCE) clean; \
	fi

install:
	@echo "install $(CURDIR)"

