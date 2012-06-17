#include <jni.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <unistd.h>

#include "gl_code.h"

// Some hacks for eclipse parser
#ifdef __CDT_PARSER__
#define JNIEXPORT
#define JNICALL
#endif


pthread_t thread = NULL;
JNIEnv * _env, *_game_env = NULL;
timeval touch_time;
static JavaVM *gJavaVM;
AScene scene;

bool running;
/**
 * Game thread
 */
void *gameThread(void*data){
    if(gJavaVM == NULL) {
    	LOGE("No JVM available");
        return NULL;
    }

    gJavaVM->AttachCurrentThread(&_game_env, NULL);
    if(_game_env == NULL){
    	LOGE("Failed to create JVM envirement for game thread");
		return NULL;
	}
    LOGI("Created game thread");
    running = true;
    while(running){
    	LOGI("Call _process_events");
    	scene->_process_events();
		LOGI("finished _process_events");
    }
    LOGI("Ending game thread");
	gJavaVM->DetachCurrentThread();
	return NULL;
}


void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s", name, v);
}

void _checkGlError(const char* op, int line, const char * file) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("%s[%i]: after %s() glError (0x%x)", file, line, op, error);
    }
}


bool setupGraphics(int w, int h) {
    gettimeofday(&touch_time, NULL);
    
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
    
    LOGI("setupGraphics(%d, %d)", w, h);
    glViewport(0, 0, w, h);
    checkGlError("glViewport");

	if(thread){
		pthread_kill(thread, SIGKILL);
		thread = NULL;
	}
    scene = 0; // Reset old scene in case it was already created
    scene = new MainScene(w, h);

	pthread_create(&thread, NULL, gameThread, NULL);
    return true;
}

void renderFrame() {
    scene->_renderFrame();
}


void moveEvent(int x, int y){
    timeval curr_time;
    double diff;
    double t1, t2;
    gettimeofday(&curr_time, NULL);
    t1 = (double)(curr_time.tv_sec) + (double)(curr_time.tv_usec)/1000000.0f;
    t2 = (double)(touch_time.tv_sec) + (double)(touch_time.tv_usec)/1000000.0f;
    diff = t1 - t2;
    if(diff < 0.1f)
        return;
    //LOGI("Move %i %i", x, y);
    touch_time = curr_time;
    AEvent event = new MoveEvent(x, y);
    scene->addEvent(event);
}

void downEvent(int x, int y){
    gettimeofday(&touch_time, NULL);
    scene->hit(x, y);
}

void upEvent(int x, int y){
    AEvent event = new UpEvent(x, y);
    scene->addEvent(event);
}

void touchEvent(int x, int y, int action){
    switch (action) {
        case ACTION_DOWN:
            downEvent(x, y);
            break;
        case ACTION_MOVE:
            moveEvent(x, y);
            break;
        case ACTION_UP:
            upEvent(x, y);
            break;
        default:
            LOGE("Unknown even type %i", action);
            break;
    }
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

unsigned char * load_raw(const char * source, size_t * file_size) {
    
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
    memcpy(buf, copy, length);
    _env->ReleaseByteArrayElements(result, copy, JNI_ABORT);
    
    /*
     * Return size of returned data if file_size pointer is given.
     */
    if(file_size)
        *file_size = length;
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
    memcpy(buf, copy, length);
    _env->ReleaseByteArrayElements(result, copy, 0);
    return buf;
}

/**
 * XXX: load* methods should probably be moved to separate loader class
 * 
 * loadBitmap loads image from *source file in assets directory.
 * It actually makes call to static Java method that returns parsed image data
 * with additional header. Data is in form:
 *     
 *     4 bytes  width
 *     4 bytes  height
 *     4 bytes  format (565, 4444, 8888 etc.)
 *     width*height*bpp  pixels, bpp depends on format
 * This is done to overcome need to create separe class in Java and link it with
 * C++ code.
 */
ATexture loadBitmap(const char * source){
    u_char * texture = load_bitmap(source);
    GLuint *w = (GLuint*)texture;
    GLuint *h = (GLuint*)texture+1;
    GLuint *c = (GLuint*)texture+2;
    ATexture tex(new Texture());
    tex->load(*w, *h, *c, texture+TEXTURE_HEADER);
    delete[] texture;
    return tex;
}

/**
 * loads compressed texture in pkm format. Data from this texture can be directly
 * feed to OpenGL if GL_ETC1_RGB8_OES extension is supported.
 */
ATexture loadTexture(const char * source){
    const u_char * texture = load_raw(source);
    ATexture tex(new Texture());
    tex->load_pkm(texture);
    delete[] texture;
    return tex;
}

/**
 * Loads OpenGL shader in text format from assets directory. Two shaders
 * (vertex and fragment) are required to create OpenGL Program. Program is
 * required to draw anything on screen.
 */
AShader loadShader(const char * source, GLuint type){
    const char * shader_text = load_asset(source);
    AShader  shader = AShader(new Shader());
    shader->load(shader_text, type);
    delete shader_text;
    return shader;
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
        JNIEnv * env, jobject obj, jint x, jint y);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_motionevent(
        JNIEnv * env, jobject obj, jint x, jint y, jint action);
/**
 * Activity events
 */
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onApplicationCreate(
        JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onApplicationTerminate(
        JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onCreate(
        JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onResume(
        JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onPause(
        JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onDestroy(
        JNIEnv * env, jobject obj);
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
    env->ReleaseStringUTFChars(javaString, nativeString);
    return 0;
}

JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_createProgram(
        JNIEnv * env, jobject obj, jint vertexShader, jint pixelShader) {
    _env = env;
    return 0;
}

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_touch(
        JNIEnv * env, jobject obj, jint x, jint y) {
    _env = env;
    downEvent(x, y);
}

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_motionevent(
        JNIEnv * env, jobject obj, jint x, jint y, jint action) {
    _env = env;
    touchEvent(x, y, action);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv *env;
	gJavaVM = vm;
	LOGI("JNI_OnLoad called");
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("Failed to get the environment using GetEnv()");
		return -1;
	}
	return JNI_VERSION_1_4;
}


JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onCreate(
        JNIEnv * env, jobject obj){
	LOGI("onCreate");

}

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onResume(
        JNIEnv * env, jobject obj){
	LOGI("onResume");
}
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onPause(
        JNIEnv * env, jobject obj){
	LOGI("onPause");
}
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onDestroy(
        JNIEnv * env, jobject obj){
	LOGI("onDestroy");
	running = false;
    AEvent event = new InvalidateScene();
    scene->addEvent(event);
	pthread_join(thread, NULL);
}


JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onApplicationCreate(
        JNIEnv * env, jobject obj){
	LOGI("onApplicationCreate");
}

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_onApplicationTerminate(
        JNIEnv * env, jobject obj){
	LOGI("onApplicationTerminate");
}
