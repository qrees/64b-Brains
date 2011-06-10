#include <jni.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "gl_code.h"

// Some hacks for eclipse parser
#ifdef __CDT_PARSER__
#define JNIEXPORT
#define JNICALL
#endif


JNIEnv * _env;
timeval touch_time;

void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s", name, v);
}

void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)", op, error);
    }
}

AScene scene;

bool setupGraphics(int w, int h) {
    gettimeofday(&touch_time, NULL);
    
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
    
    LOGI("setupGraphics(%d, %d)", w, h);
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    scene = 0; // Reset old scene in case it was already created
    scene = new MainScene(w, h);
    return true;
}

void renderFrame() {
    scene->renderFrame();
    //scene->hit_check();
}


void touchEvent(int x, int y){
    timeval curr_time;
    double diff;
    double t1, t2;
    gettimeofday(&curr_time, NULL);
    t1 = (double)(curr_time.tv_sec) + (double)(curr_time.tv_usec)/1000000.0f;
    t2 = (double)(touch_time.tv_sec) + (double)(touch_time.tv_usec)/1000000.0f;
    diff = t1 - t2;
    if(diff < 0.1f)
        return;
    touch_time = curr_time;
    AEvent event = new ClickEvent(x, y);
    scene->addEvent(event);
}

char * load_asset(const char * source) {
    jstring jstr = _env->NewStringUTF(source);
    jclass jclass = _env->FindClass("info/qrees/android/brains/GL2JNILib");
    jmethodID messageMe = _env->GetStaticMethodID(jclass, "loadAsset",
            "(Ljava/lang/String;)Ljava/lang/String;");
    jobject result = _env->CallStaticObjectMethod(jclass, messageMe, jstr);

    const char* str = _env->GetStringUTFChars((jstring) result, NULL);
    LOGI("Loaded asset %s:\n%s", source, str);
    int len = strlen(str);
    char * ret_str = new char[len + 1];
    strcpy(ret_str, str);
    ret_str[len] = 0;
    _env->ReleaseStringUTFChars((jstring) result, str);
    return ret_str;
}

unsigned char * load_raw(const char * source) {
    
    /*
     * Get java method id to load bitmap and call this method
     */
    jstring jstr = _env->NewStringUTF(source);
    jclass jclass = _env->FindClass("info/qrees/android/brains/GL2JNILib");
    jmethodID messageMe = _env->GetStaticMethodID(jclass, "loadRaw", "(Ljava/lang/String;)[B");
    jbyteArray result = (jbyteArray)_env->CallStaticObjectMethod(jclass, messageMe, jstr);
    if(result == NULL)
        return 0;
    /*
     * Copy bitmap data from java array and release that array
     */
    jsize length = _env->GetArrayLength(result);
    unsigned char * buf = new unsigned char[length];
    signed char * copy = _env->GetByteArrayElements(result, 0);
    for(int i =0; i < length; i++)
        buf[i] = copy[i];
    _env->ReleaseByteArrayElements(result, copy, 0);
    return buf;
}

u_char * load_bitmap(const char * source) {
    
    /*
     * Get java method id to load bitmap and call this method
     */
    jstring jstr = _env->NewStringUTF(source);
    jclass jclass = _env->FindClass("info/qrees/android/brains/GL2JNILib");
    jmethodID messageMe = _env->GetStaticMethodID(jclass, "loadBitmap", "(Ljava/lang/String;)[B");
    jbyteArray result = (jbyteArray)_env->CallStaticObjectMethod(jclass, messageMe, jstr);
    if(result == NULL)
        return 0;
    /*
     * Copy bitmap data from java array and release that array
     */
    jsize length = _env->GetArrayLength(result);
    unsigned char * buf = new unsigned char[length];
    signed char * copy = _env->GetByteArrayElements(result, 0);
    for(int i =0; i < length; i++)
        buf[i] = copy[i];
    _env->ReleaseByteArrayElements(result, copy, 0);
    return buf;
}

extern "C" {
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_init(
        JNIEnv * env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_step(
        JNIEnv * env, jobject obj);
JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_loadShader(
        JNIEnv * env, jobject obj, jstring javaString, jint type);
JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_createProgram(
        JNIEnv * env, jobject obj, jint vertexShader, jint pixelShader);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_touch(
        JNIEnv * env, jobject obj, jint width, jint height);
}

jint throwJNI(const char *message) {
    jclass exClass;
    const char *className = "java/lang/Exception";
    exClass = _env->FindClass(className);
    return _env->ThrowNew(exClass, message);
}

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_init(
        JNIEnv * env, jobject obj, jint width, jint height) {
    _env = env;
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_step(
        JNIEnv * env, jobject obj) {
    _env = env;
    renderFrame();
}

JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_loadShader(
        JNIEnv * env, jobject obj, jstring javaString, jint shaderType) {
    _env = env;
    const char *nativeString = env->GetStringUTFChars(javaString, 0);

    LOGI("Loading shader: %s", nativeString);
    //GLuint shader = loadShader(shaderType, nativeString);

    env->ReleaseStringUTFChars(javaString, nativeString);
    return 0;
}

JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_createProgram(
        JNIEnv * env, jobject obj, jint vertexShader, jint pixelShader) {
    _env = env;
    return 0;
}

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_touch(
        JNIEnv * env, jobject obj, jint width, jint height) {
    _env = env;
    touchEvent(width, height);
}
