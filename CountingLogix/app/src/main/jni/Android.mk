LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


include ../../sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := CountingLogix
LOCAL_SRC_FILES := jni_part.cpp cimUtils.cpp Blob.cpp Img.cpp cimProj.cpp ColonyCell.cpp RollingBall.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
