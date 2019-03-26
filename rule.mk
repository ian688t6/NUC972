LOCAL_SRC ?= $(shell find ${LOCAL_CUR} -name *.c)
LOCAL_OBJ := $(patsubst %.c,%.o,$(LOCAL_SRC))
TARGET_BIN := bin
TARGET_SO := so
TARGET_AR := ar

COLOR_BLUE = \E[1;34m
COLOR_RES = \E[0m

.PHONY:all clean install

all: makecmd $(TARGET)
makecmd:
ifneq ($(make-cmd), )
	$(make-cmd)
endif

$(TARGET):$(LOCAL_OBJ)
	@echo -e "${COLOR_BLUE}[ $(TARGET_TYPE): $(TARGET) ]${COLOR_RES}"
ifeq ($(TARGET_TYPE),$(TARGET_BIN))
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(INCLUDES) $(LDFLAGS)
	$(CROSS_COMPILE)$(OBJDUMP) -d $@ > $@.dump
	$(CROSS_COMPILE)$(OBJDUMP) -h $@ > $@.map
	$(CROSS_COMPILE)$(NM) $@ | sort > $@.sym
	$(CROSS_COMPILE)$(NM) -S --size-sort $@ > $@.nm
	$(CROSS_COMPILE)$(STRIP) $@
	@cp -rf $@ $(BUILDDIR)/bin
endif
ifeq ($(TARGET_TYPE),$(TARGET_SO))
	$(CROSS_COMPILE)$(CC) -shared $^ -o $@ $(LDFLAGS)
	$(CROSS_COMPILE)$(STRIP) $@
	@cp -rf $@ $(BUILDDIR)/lib
endif
ifeq ($(TARGET_TYPE),$(TARGET_AR))
	$(CROSS_COMPILE)$(AR) crs $@ $^
	@cp -rf $@ $(BUILDDIR)/lib
endif

%.o:%.c
ifeq ($(TARGET_TYPE),$(TARGET_BIN))
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) $(INCLUDES) $(LOCAL_INC) -c $^ -o $@
endif
ifeq ($(TARGET_TYPE),$(TARGET_SO))
	$(CROSS_COMPILE)$(CC) -shared -fPIC $(CCFLAGS) $(INCLUDES) $(LOCAL_INC) $(LOCAL_LIB) -c $^ -o $@
endif
ifeq ($(TARGET_TYPE),$(TARGET_AR))
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) $(INCLUDES) $(LOCAL_INC) $(LOCAL_LIB) -c $^ -o $@
endif

clean:
ifneq ($(clean-cmd), )
	$(clean-cmd)
endif
	@echo "rm $(TARGET) $(LOCAL_OBJ)"
	@rm $(LOCAL_OBJ)
	@rm $(TARGET)

install:
	@echo "install $(TARGET)"
ifeq ($(TARGET_TYPE),$(TARGET_SO))
	@cp $(BUILDDIR)/lib/$(TARGET) $(BUILDDIR)/rootfs/lib
endif
ifeq ($(TARGET_TYPE),$(TARGET_BIN))
	@cp $(BUILDDIR)/bin/$(TARGET) $(BUILDDIR)/rootfs/bin
endif
ifeq ($(TARGET_TYPE),$(TARGET_AR))
	@cp $(BUILDDIR)/lib/$(TARGET) $(BUILDDIR)/rootfs/lib
endif

