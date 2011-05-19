#include <jni.h>

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

void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s", name, v);
}

void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)", op, error);
    }
}

//GLuint gProgram;
GLuint gvPositionHandle;
AShader vertexShader(new Shader());
AShader fragmentShader(new Shader());
AProgram program(new Program());

GLfloat vertexPos[3 * 3] = { //
        0.0f,  0.5f, 0.0f,   //
       -0.5f, -0.5f, 0.0f,   //
        0.5f, -0.5f, 0.0f    //
    };
GLfloat colors[4 * 4] = {       //
        1.0f, 0.0f, 0.0f, 1.0f, //
        0.0f, 1.0f, 0.0f, 1.0f, //
        0.0f, 0.0f, 1.0f, 1.0f, //
        1.0f, 1.0f, 1.0f, 1.0f  //
    };
GLfloat colorsRGB[3 * 3] = {       //
        1.0f, 0.0f, 0.0f, //
        0.0f, 1.0f, 0.0f, //
        1.0f, 1.0f, 1.0f, //
    };

Scene scene;
AMesh root;
const unsigned char * texture;

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    texture = load_raw("images/background.pkm");
    char * gVertexShader = load_asset("shaders/vertex_attrib.gls");
    char * gFragmentShader = load_asset("shaders/fragment_attrib.gls");
    vertexShader->load(gVertexShader, GL_VERTEX_SHADER);
    fragmentShader->load(gFragmentShader, GL_FRAGMENT_SHADER);
    program->make(vertexShader, fragmentShader);
    if (!program->isValid()) {
        LOGE("Could not create program.");
        return false;
    }
    program->activateColor();
    program->activatePosition();

    LOGI("setupGraphics(%d, %d)", w, h);
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    
    //unsigned char * texture = load_bitmap("images/images/background.pkm");
    
    Group * group = new Group();
    Mesh * mesh = new Rectangle();
    root = AMesh(group);
    AMesh a_mesh = AMesh(mesh);
    mesh->setProgram(program);
    mesh->setColor(colors, 4);
    group->addObject(a_mesh);
    
    ATexture tex(new Texture());
    tex->load_pkm(texture);
    mesh->setTexture(tex);
    
    return true;
}


void renderFrame() {
    static float grey;
    grey = 0.5f;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
    
    LOGI("Rendering frame");
    program->activate();
    program->bindColor(colors);
    program->bindPosition(vertexPos);
    
    checkGlError("glUseProgram");

    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
    root->draw();
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

extern "C" {
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_init(
        JNIEnv * env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_step(
        JNIEnv * env, jobject obj);
JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_loadShader(
        JNIEnv * env, jobject obj, jstring javaString, jint type);
JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_createProgram(
        JNIEnv * env, jobject obj, jint vertexShader, jint pixelShader);
}
;

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
    
    return program->getName();
}
