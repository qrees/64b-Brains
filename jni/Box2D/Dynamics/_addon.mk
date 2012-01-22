include $(LOCAL_PATH)/Box2D/Dynamics/Contacts/_addon.mk 
include $(LOCAL_PATH)/Box2D/Dynamics/Joints/_addon.mk 
LOCAL_SRC_FILES += \
Box2D/Dynamics/b2Body.cpp \
Box2D/Dynamics/b2ContactManager.cpp \
Box2D/Dynamics/b2Fixture.cpp \
Box2D/Dynamics/b2Island.cpp \
Box2D/Dynamics/b2World.cpp \
Box2D/Dynamics/b2WorldCallbacks.cpp
