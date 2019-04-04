.PHONY:all clean install

all: FORCE
	
clean: FORCE

install:
	@echo "install  script"
	@cp -rf ./S70initenv $(BUILDDIR)/rootfs/etc/init.d
	@chmod u+x $(BUILDDIR)/rootfs/etc/init.d/S70initenv
	
FORCE:
