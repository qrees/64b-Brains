#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

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


char * load_asset(const char * source);
jint throwJNI(const char *message);

JNIEnv * _env;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s", name, v);
}

static void checkGlError(const char* op) {
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

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    
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
    
    unsigned char * texture = load_bitmap("images/images/background.pkm");
    
    Group * group = new Group();
    Mesh * mesh = new Rectangle();
    root = AMesh(group);
    AMesh a_mesh = AMesh(mesh);
    mesh->setProgram(program);
    mesh->setColor(colors, 4);
    group->addObject(a_mesh);
    
    return true;
}

Shader::Shader() {
    valid = false;
    _id = 0;
    _source = string("");
}
Shader::~Shader(){
    if(_id)
        glDeleteShader(_id);
}
Shader::Shader(const char * source, GLenum shaderType) {
    load(source, shaderType);
}
void Shader::load(const char*source, GLenum shaderType){
    _source = string(source);
    _type = shaderType;
    _id = 0;
    _log = 0;
    _compile();
}
char * Shader::getInfo() {
    if (_log)
        delete[] _log;
    _log = 0;
    GLint infoLen = 0;
    glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &infoLen);
    LOGE("Log length %i", infoLen);
    if (infoLen) {
        _log = new char[infoLen];
        if (_log)
            return _log;
        else
            LOGE("Failed to allocate memory for program log");
    }
    return _log;
}

bool Shader::isValid() {
    return _id != 0 & valid;
}

GLuint Shader::_compile() {
    _id = glCreateShader(_type);
    if (_id) {
        const char * source = _source.c_str();
        glShaderSource(_id, 1, &source, NULL);
        glCompileShader(_id);
        GLint compiled = 0;
        glGetShaderiv(_id, GL_COMPILE_STATUS, &compiled);
        if (compiled != GL_TRUE) {
            LOGE("Failed to compile shader: %s", _source.c_str());
            char * info = getInfo();
            if (info)
                LOGE("Could not compile shader %d:\n%s",
                        _type, info);
            glDeleteShader(_id);
            _id = 0;
        } else {
            LOGI("Shader compilled succesfully %d", _id);
            valid = true;
        }
    } else {
        LOGE("Failed to create shader");
    }
    return _id;
}
GLuint Shader::getName() {
    return _id;
}


Program::Program(AShader vertexShader, AShader fragmentShader) {
    _id = 0;
    make(vertexShader, fragmentShader);
}
Program::Program(){
    _id = 0;
}
Program::~Program(){
    if(getName())
        glDeleteProgram(getName());
}
void Program::activateColor(){
    activateAttribute("a_color", LOC_COLOR);
}
void Program::activatePosition(){
    activateAttribute("a_position", LOC_POSITION);
}
void Program::activateAttribute(const char *name, GLuint location){
    attribs[location] = glGetAttribLocation(getName(), name);
}
void Program::activate(){
    glUseProgram(getName());
}
void Program::bindColor(const void *data){
    bindAttribute(COLOR_ATTRIB, 4, GL_FLOAT, 0, data);
}
void Program::bindColorRGB(const void *data){
    bindAttribute(COLOR_ATTRIB, 3, GL_FLOAT, 0, data);
}
void Program::bindPosition(const void *data){
    bindAttribute(POSITION_ATTRIB, 3, GL_FLOAT, 0, data);
}
void Program::bindPosition(GLuint buf_id){
    bindBuffer(POSITION_ATTRIB, buf_id, 3, GL_FLOAT, 0, 0);
}
void Program::bindNormal(GLuint buf_id){
    bindBuffer(NORMAL_ATTRIB, buf_id, 3, GL_FLOAT, 0, 0);
}
void Program::bindTexture(GLuint buf_id){
    bindBuffer(TEXTURE_ATTRIB, buf_id, 2, GL_FLOAT, 0, 0);
}
void Program::bindColor(GLuint buf_id){
    bindBuffer(COLOR_ATTRIB, buf_id, 4, GL_FLOAT, 0, 0);
}
void Program::bindAttribute(GLuint location, GLuint size, GLenum type, GLuint stride, const void *data){
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, size, type, GL_FALSE, stride, data);
}
void Program::bindBuffer(GLuint location, GLuint buf_id, GLuint size, GLenum type, GLuint stride, const void * offset){
    LOGI("Bind buffer id %d at location %d", buf_id, location);
    glBindBuffer(GL_ARRAY_BUFFER, buf_id);
    checkGlError("glBindBuffer");
    glEnableVertexAttribArray(location);
    checkGlError("glEnableVertexAttribArray");
    glVertexAttribPointer(location, size, type, GL_FALSE, stride, 0);
    checkGlError("glVertexAttribPointer");
    
}
void Program::make(AShader vertexShader, AShader fragmentShader){
    LOGI("Making program with fragment and vertex shaders");
    _vertex = vertexShader;
    _fragment = fragmentShader;
    _log = 0;
    LOGI("Start linking program");
    _create();
}
char * Program::getInfo() {
    if (_log)
        delete[] _log;
    GLint bufLength = 0;
    _log = 0;
    glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &bufLength);
    LOGE("Log length %i", bufLength);
    if (bufLength) {
        _log = new char[bufLength];
        if (!_log)
            LOGE("Failed to allocate memory for program log");
    }
    return _log;
}

GLuint Program::_link() {
    glLinkProgram(_id);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_id, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        char * info = getInfo();
        if (info)
            LOGE("Could not link program:\n%s", info);
        glDeleteProgram(_id);
        _id = 0;
    } else {
        LOGI("Program linked succesfully");
    }
    return linkStatus;
}

GLuint Program::_create() {
    _id = glCreateProgram();
    if (_id) {
        LOGI("Attaching shader %d", _vertex->getName());
        glAttachShader(_id, _vertex->getName());
        checkGlError("glAttachShader");
        LOGI("Attaching shader %d", _fragment->getName());
        glAttachShader(_id, _fragment->getName());
        checkGlError("glAttachShader");
        GLuint status = _link();
        if (status != GL_TRUE)
            _id = 0;
    } else {
        LOGE("Failed to create program");
    }
    return _id;
}

Mesh::Mesh() {
    init();
}
Mesh::~Mesh(){
    glDeleteBuffers(BUF_COUNT, vboIds);
}
void Mesh::init() {
    glGenBuffers(BUF_COUNT, vboIds);
    LOGI("Buffers:");
    for(int i = 0; i < BUF_COUNT; i++)
        LOGI(" %d = %d", i, vboIds[i]);
    has_color = has_normal = has_texture = false;
}
void Mesh::setProgram(AProgram program) {
    this->program = program;
}
void Mesh::setVertices(GLfloat *buf, GLint num) {
    LOGI("Set position");
    _setBuffer(GL_ARRAY_BUFFER, buf, sizeof(GLfloat) * 3 * num, VERTEX_BUF);
}
void Mesh::setNormal(GLfloat *buf, GLint num) {
    has_normal = true;
    _setBuffer(GL_ARRAY_BUFFER, buf, sizeof(GLfloat) * 3 * num, NORMAL_BUF);
}
void Mesh::setTextureCoord(GLfloat *buf, GLint num) {
    has_texture = true;
    _setBuffer(GL_ARRAY_BUFFER, buf, sizeof(GLfloat) * 2 * num, TEXTURE_BUF);
}
void Mesh::setColor(GLfloat *buf, GLint num) {
    has_color = true;
    _setBuffer(GL_ARRAY_BUFFER, buf, sizeof(GLfloat) * 4 * num, COLOR_BUF);
}
void Mesh::setIndexes(GLushort *buf, GLint num) {
    numIndices = num;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * num, buf,
            GL_STATIC_DRAW);
}

void Mesh::draw() {
    assert(program->isValid(), "You need to set program for Mesh");
    program->activate();
    program->bindPosition(vboIds[VERTEX_BUF]);
    if(has_normal)
        program->bindNormal(vboIds[NORMAL_BUF]);
    if(has_texture)
        program->bindTexture(vboIds[TEXTURE_BUF]);
    if(has_color)
        program->bindColor(vboIds[COLOR_BUF]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
    checkGlError("glDrawElements");
}
void Mesh::_setBuffer(GLenum target, GLfloat *buf, GLuint size, GLuint sel) {
    LOGI("SetBuffer %d, %d, %d, %d", target, size, sel, (sel >= 0) && (sel < BUF_COUNT));
    assert((sel >= 0) && (sel < BUF_COUNT), "this buffer does not exist");
    //strides[sel] = stride;
    glBindBuffer(target, vboIds[sel]);
    glBufferData(target, size, buf, GL_STATIC_DRAW);
    checkGlError("glBufferData");
}

void Group::addObject(AMesh mesh){
    list<Mesh> lista;
    lista.push_front(Mesh());
    _objects.push_back(mesh);
}
void Group::draw(){
    list<AMesh>::iterator it;
    for(it = _objects.begin(); it != _objects.end(); it++)
        (*it)->draw();
}

GLfloat rec_vertices[4*3] = { 0.0f, 1.0f, 0.0f, // 0, Top Left
        0.0f, 0.0f, 0.0f, // 1, Bottom Left
        1.0f, 0.0f, 0.0f, // 2, Bottom Right
        1.0f, 1.0f, 0.0f, // 3, Top Right
    };
GLushort rec_indexes[2 * 3] = { 0, 1, 2, 0, 2, 3 };
Rectangle::Rectangle():Mesh(){
    setVertices(rec_vertices, 4);
    setIndexes(rec_indexes, 6);
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

unsigned char * load_bitmap(const char * source) {
    
    /*
     * Get java method id to load bitmap and call this method
     */
    jstring jstr = _env->NewStringUTF(source);
    jclass jclass = _env->FindClass("info/qrees/android/brains/GL2JNILib");
    jmethodID messageMe = _env->GetStaticMethodID(jclass, "loadBitmap", "(Ljava/lang/String;)[B");
    jbyteArray result = (jbyteArray)_env->CallStaticObjectMethod(jclass, messageMe, jstr);
    
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
