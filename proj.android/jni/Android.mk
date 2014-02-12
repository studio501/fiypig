LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libflypig_shared

LOCAL_MODULE_FILENAME := libflypig

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/**/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/**/**/**/**/*.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,CocosDenshion/android)