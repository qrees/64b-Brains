LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libgl2jni
LOCAL_CFLAGS    := -g -Wall
LOCAL_SRC_FILES := gl_code.cpp \
             shader.cpp program.cpp mesh.cpp texture.cpp scene.cpp render_visitor.cpp \
             framebuffer.cpp event.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 -lstdc++
LOCAL_SHARED_LIBRARIES := stlport_static
LOCAL_C_INCLUDES := /home/qrees/bin/android-ndk-r5b/sources/cxx-stl/stlport/stlport

include $(BUILD_SHARED_LIBRARY)
