include $(shell pwd)/config.mk

mod ?= null
dirs += tools
dirs += boot
dirs += os
dirs += libs
dirs += opensrc
dirs += demo
dirs += apps

.PHONY: all distclean cleanall install build clean prepare rootfs

all: $(foreach dir,$(dirs),make_all_$(dir))

prepare:
	@rm -rf $(BUILDDIR)
	@mkdir build > /dev/null
	@mkdir -p build/lib > /dev/null
	@mkdir -p build/bin > /dev/null
	@mkdir -p out/bin > /dev/null
	@mkdir -p out/hosttool > /dev/null
	@cd build;git clone https://gitee.com/OpenNuvoton/NUC970_U-Boot_v2016.11 u-boot-201611;cd -;
	@cd build;git clone https://gitee.com/OpenNuvoton/NUC970_Linux_Kernel linux-3.10.y;cd -;
	@tar -xf opensrc/rootfs/rootfs.tgz -C build/

distclean: cleanall
	@rm -rf $(OUTDIR)
	@rm -rf $(BUILDDIR)

cleanall: $(foreach dir,$(dirs),make_clean_$(dir))

install: $(foreach dir,$(dirs),make_install_$(dir))

build: $(foreach dir,$(dirs),make_submod_all_$(dir))

clean: $(foreach dir,$(dirs),make_submod_clean_$(dir))

rootfs:
	@echo "make rootfs"
	@rm -rf $(OUTDIR)/bin/rootfs*
	@cd $(OUTDIR)/hosttool; ./mksquashfs		$(BUILDDIR)/rootfs $(OUTDIR)/bin/rootfs_squashfs.bin -b 64k -comp xz; 	cd - > /dev/null;
	@cd $(OUTDIR)/hosttool; ./mkfs.jffs2 -d 	$(BUILDDIR)/rootfs -l -e 0x40000 -o $(OUTDIR)/bin/rootfs_jffs_256k.bin; cd - > /dev/null;
	@cd $(OUTDIR)/hosttool; ./mkfs.jffs2 -d 	$(BUILDDIR)/rootfs -l -e 0x20000 -o $(OUTDIR)/bin/rootfs_jffs_128k.bin; cd - > /dev/null;
	@cd $(OUTDIR)/hosttool; ./mkfs.jffs2 -d 	$(BUILDDIR)/rootfs -l -e 0x10000 -o $(OUTDIR)/bin/rootfs_jffs_64k.bin;  cd - > /dev/null;

define make_submod
make_submod_$(1)_$(2) : $(2)
	@if [ $(2) = ${mod} ]; then $(MAKE) -C $(2) $(1);fi
endef

define make
make_$(1)_$(2) : $(2)
	$(MAKE) -C $(2) $(1)
endef

$(foreach dir,$(dirs),$(eval $(call make_submod,all,$(dir))))
$(foreach dir,$(dirs),$(eval $(call make_submod,clean,$(dir))))
$(foreach dir,$(dirs),$(eval $(call make,all,$(dir))))
$(foreach dir,$(dirs),$(eval $(call make,clean,$(dir))))
$(foreach dir,$(dirs),$(eval $(call make,install,$(dir))))

