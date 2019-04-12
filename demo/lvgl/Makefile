TARGET_TYPE := bin
TARGET := lvgl_demo

LOCAL_INC := -I$(OPENSRCDIR)/littlevgl 
LOCAL_SRC += main.c
LOCAL_LIB := -llvgldrv
LOCAL_LIB += -llvgl

CCFLAGS += -DUSE_FBDEV
make-cmd := $(APPSDIR)/mkcompile_h
clean-cmd := rm -rf compile.h $(TARGET).*

include $(TOPDIR)/rule.mk
