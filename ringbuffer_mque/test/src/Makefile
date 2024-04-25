include $(TOPDIR)/rules.mk
include $(BUILD_DIR)/kernel.mk
include $(BUILD_DIR)/package.mk

CUR_PATH := .




TARGET_BIN := test_pcm_capture

SRC_PATHS = $(CUR_PATH)
#CFILES = $(foreach n, $(SRC_PATHS), $(wildcard $(n)/*.c)) 
#C++FILES = $(foreach n, $(SRC_PATHS), $(wildcard $(n)/*.cc $(n)/*.cpp)) 

# reduce app size by manual
CFILES = $(CUR_PATH)/test.c \
		$(CUR_PATH)/vir_audio/pcm_vir.c  \
		$(CUR_PATH)/mque_api/que.c   \
		$(CUR_PATH)/ringbuffer/ringbuffer.c


OBJS = $(CFILES:%=%.o) $(C++FILES:%=%.o)
DEPS = $(OBJS:%=%.d)

INCLUDES_DIR := \
	$(CUR_PATH)/ringbuffer \
	$(CUR_PATH)/mque_api  \
	$(CUR_PATH)/vir_audio   \
	$(KERNEL_DIR)/drivers/usb/gadget/function \
	$(KERNEL_DIR)/user_headers/include \
	$(STAGING_DIR)/usr/include/ \
	$(STAGING_DIR)/usr/include/rt_media \
	$(STAGING_DIR)/usr/include/eyesee-mpp \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/include/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/include/utils/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/include/media/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/include/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/libisp/include/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/libisp/include/V4l2Camera/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/libisp/include/V4l2Camera/linux/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/libisp/isp_tuning/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/AudioLib/osal/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/AudioLib/midware/decoding/include/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/AudioLib/osal/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/libcedarc/include/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/include_FsWriter/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/include_stream/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/include_muxer/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/agc_lib/include/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/LIBRARY/libISE/include/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/middleware/media/include/component/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/system/public/libion/include/ \
	$(STAGING_DIR)/usr/include/eyesee-mpp/system/public/include \
	$(STAGING_DIR)/usr/include/eyesee-mpp/system/public/include/utils 

LIBS_DIR := \
	$(STAGING_DIR)/usr/lib  \
	$(STAGING_DIR)/target/rootfs/lib
	

INCLUDES = $(foreach n, $(INCLUDES_DIR), -I$(n))
LIBS = $(foreach n, $(LIBS_DIR), -L$(n))

#SHARED_LIBS_NAME = $(foreach n, $(LIBS_DIR), $(wildcard $(n)/*.so)) 
#STATIC_LIBS_NAME = $(foreach n, $(LIBS_DIR), $(wildcard $(n)/*.a)) 
# reduce app size by manual
#SHARED_LIBS_NAME = libasound.so librt_media.so
SHARED_LIBS_NAME = 
STATIC_LIBS_NAME = 


SHARED_LIBS := \
	-lpthread $(foreach n, $(SHARED_LIBS_NAME), -l$(patsubst lib%,%,$(patsubst %.so,%,$(notdir $(n)))))
STATIC_LIBS := \
	$(foreach n, $(STATIC_LIBS_NAME), -l$(patsubst lib%,%,$(patsubst %.a,%,$(notdir $(n)))))

CEDARX_EXT_CFLAGS += -D__OS_LINUX
CCFLAGS = -c -g -fPIC $(CEDARX_EXT_CFLAGS) $(INCLUDES)
CXXFLAGS = -c -g -fPIC $(CEDARX_EXT_CFLAGS) $(INCLUDES)
STRIP=arm-openwrt-linux-muslgnueabi-strip

LOCAL_BIN_FLAGS := \
	$(LDFLAGS) \
	$(CFLAGS) \
	$(LIBS) \
	-Wl,-Bstatic -Wl,--start-group $(STATIC_LIBS) -Wl,--end-group \
	-Wl,-Bdynamic -Wl,--start-group $(SHARED_LIBS) -Wl,--end-group

.PHONY: all clean

all:$(TARGET_BIN)

$(TARGET_BIN):$(OBJS)
	@echo $(STATIC_LIBS) =======
	@echo $(SHARED_LIBS) -------
	$(CXX) $^ $(LOCAL_BIN_FLAGS) -o $@
	$(STRIP) $(TARGET_BIN)

$(filter %.c.o , $(OBJS)): %.o:%
	$(CC) $(CCFLAGS) -o $@  $< -MD -MP -MF $(@:%=%.d)

$(filter %.cpp.o %.cc.o, $(OBJS)): %.o:%
	$(CXX) $(CXXFLAGS) -o $@  $< -MD -MP -MF $(@:%=%.d)
	
clean:
	$(RM) -rf $(TARGET_BIN)
	$(RM) -rf $(OBJS)
	$(RM) -rf $(DEPS)
	$(RM) -rf *.o
	$(RM) -rf *.d

-include $(DEPS)

