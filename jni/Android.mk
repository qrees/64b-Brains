LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libgl2jni
LOCAL_CFLAGS    := -Werror
LOCAL_SRC_FILES := gl_code.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 -lstdc++
APP_STL := stlport_static
LOCAL_STATIC_LIBRARIES := stlport_static
LOCAL_C_INCLUDES := /home/qrees/bin/android-ndk-r5b/sources/cxx-stl/stlport/stlport

include $(BUILD_SHARED_LIBRARY)
