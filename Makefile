include $(shell pwd)/config.mk

mod :=
dirs += boot
dirs += os
dirs += opensrc

.PHONY: prepare all cleanall install build clean

prepare:
	@cd boot;git clone https://gitee.com/OpenNuvoton/NUC970_U-Boot_v2016.11 u-boot-201611;cd -;
	@cd os;git clone https://gitee.com/OpenNuvoton/NUC970_Linux_Kernel linux-3.10.y;cd -;

all: $(foreach dir,$(dirs),make_all_$(dir))

cleanall: $(foreach dir,$(dirs),make_clean_$(dir))

install: $(foreach dir,$(dirs),make_install_$(dir))

build: $(foreach dir,$(dirs),make_submod_all_$(dir))
	
clean: $(foreach dir,$(dirs),make_submod_clean_$(dir))

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

