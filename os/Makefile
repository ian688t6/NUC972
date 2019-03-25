CURDIR := $(shell pwd)
KERNEL_SOURCE := linux-3.10.y

.PHONY: all clean install

all:
	@echo "build $(CURDIR)"
	@if [ -d $(KERNEL_SOURCE) ]; then \
		echo "make $(KERNEL_SOURCE)"; \
	fi

clean:
	@echo "clean $(CURDIR)"
	@if [ -d $(KERNEL_SOURCE) ]; then \
		make -C $(KERNEL_SOURCE) clean; \
	fi

install:
	@echo "install $(CURDIR)"

