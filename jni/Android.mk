LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libgl2jni
LOCAL_CFLAGS    := -O3 -Wall
LOCAL_SRC_FILES := gl_code.cpp \
             shader.cpp program.cpp mesh.cpp texture.cpp scene.cpp render_visitor.cpp \
             framebuffer.cpp event.cpp font.cpp textarea.cpp animation.cpp world.cpp boxscene.cpp \
             boxworld.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 -lstdc++
LOCAL_SHARED_LIBRARIES := stlport_static
LOCAL_C_INCLUDES := /home/qrees/bin/android-ndk-r5b/sources/cxx-stl/stlport/stlport

include $(LOCAL_PATH)/Box2D/Collision/_addon.mk 
include $(LOCAL_PATH)/Box2D/Common/_addon.mk 
include $(LOCAL_PATH)/Box2D/Dynamics/_addon.mk 
include $(LOCAL_PATH)/clipper/_addon.mk 


include $(BUILD_SHARED_LIBRARY)
