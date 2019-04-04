TARGET_TYPE := bin
TARGET := rf-pos

LOCAL_INC += -I$(LIBDIR)/libbasic 
LOCAL_SRC += main.c
LOCAL_LIB += -lbasic
LOCAL_LIB += -lpthread

make-cmd := $(APPSDIR)/mkcompile_h
clean-cmd := rm -rf compile.h $(TARGET).*

include $(TOPDIR)/rule.mk
