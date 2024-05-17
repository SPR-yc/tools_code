TARGET_BIN = list_test

SRC_FILE = \
		$(CUR_PATH)/main.c  \
		$(CUR_PATH)/list_api/list.c  

CUR_PATH = .


INCLUDE_DIRS := \
    $(CUR_PATH) \
	$(CUR_PATH)/list_api \

inc_paths := $(foreach inc,$(filter-out -I%,$(INCLUDE_DIRS)),$(addprefix -I, $(inc))) \
                $(filter -I%, $(INCLUDE_DIRS))

LIB_DIR := \
	$(STAGING_DIR)/usr/lib


LIBS = $(foreach n, $(LIBS_DIR), -L$(n))

CCFLAGS = -lpthread  -lrt

all:
#$(CXX) $(SRC_FILE) $(LOCAL_BIN_LDFLAGS) $(IN_LDFLAG) -o $@
	gcc $(SRC_FILE) $(inc_paths) $(LIBS) $(CCFLAGS) -o $(TARGET_BIN) 


clean:
	rm $(TARGET_BIN)
