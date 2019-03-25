LOCAL_CUR := $(shell pwd)

COLOR_GREEN = \E[1;32m
COLOR_RES = \E[0m
subdir_clean := ${addsuffix _clean,${sub_dir}}
subdir_install := ${addsuffix _install,${sub_dir}}

.PHONY:all clean install
.PHONY:$(sub_dir) $(subdir_clean)

all:$(sub_dir)
$(sub_dir):
	@echo -e "${COLOR_GREEN}[ subdir: $@ ]${COLOR_RES}"
	@make -C $@ all

clean:$(subdir_clean)
$(subdir_clean):
	@make -C ${patsubst %_clean,%,$@} clean

install:$(subdir_install)
$(subdir_install):
	@make -C ${patsubst %_install,%,$@} install

