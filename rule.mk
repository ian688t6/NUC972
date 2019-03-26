LDFLAG := 
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
endif
ifeq ($(TARGET_TYPE),$(TARGET_SO))
endif
ifeq ($(TARGET_TYPE),$(TARGET_AR))
endif

%.o:%.c
ifeq ($(TARGET_TYPE),$(TARGET_BIN))
endif
ifeq ($(TARGET_TYPE),$(TARGET_SO))
endif
ifeq ($(TARGET_TYPE),$(TARGET_AR))
endif

clean:
ifneq ($(clean-cmd), )
	$(clean-cmd)
endif
#	@echo "rm $(TARGET) $(LOCAL_OBJ)"
#	@rm $(LOCAL_OBJ)
#	@rm $(TARGET)

install:
	@echo "install $(TARGET)"
ifeq ($(TARGET_TYPE),$(TARGET_SO))
endif
ifeq ($(TARGET_TYPE),$(TARGET_BIN))
endif
ifeq ($(TARGET_TYPE),$(TARGET_AR))
endif

