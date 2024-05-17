TARGET_BIN = mque_test
CUR_PATH = .

SRC_FILE = \
		$(CUR_PATH)/main.c  \
		$(CUR_PATH)/mque_api/que.c \
		$(CUR_PATH)/mque_mono/mono_que.c \
		$(CUR_PATH)/test_1.c  \
		$(CUR_PATH)/test_2.c



INCLUDE_DIRS := \
    $(CUR_PATH) \
	$(CUR_PATH)/mque_api \
	$(CUR_PATH)/mque_mono 

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
