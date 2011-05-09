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
#include <string>

using namespace std;

#ifdef __CDT_PARSER__
#define JNIEXPORT
#define JNICALL
#endif

#define  LOG_TAG    "Brains"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

char * load_asset(const char * source);
jint throwJNI(const char *message);

JNIEnv * _env;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled != GL_TRUE) {
            LOGE("Failed to compile shader: %s\n", pSource);
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            LOGE("Log length %i\n", infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                } else
                    LOGE("Failed to allocate memory for program log");
                glDeleteShader(shader);
                shader = 0;
            }
        } else {
            LOGI("Shader compilled succesfully\n");
        }
    } else {
        LOGE("Failed to create shader\n");
    }
    return shader;
}

GLuint linkProgram(GLuint program) {
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        LOGE("Log length %i\n", bufLength);
        if (bufLength) {
            char* buf = (char*) malloc(bufLength);
            if (buf) {
                glGetProgramInfoLog(program, bufLength, NULL, buf);
                LOGE("Could not link program:\n%s\n", buf);
                free(buf);
            } else
                LOGE("Failed to allocate memory for program log");
        }
        glDeleteProgram(program);
    } else {
        LOGI("Program linked succesfully\n");
    }
    return linkStatus;
}

GLuint createProgram(GLuint vertexShader, GLuint pixelShader) {
    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        GLuint status = linkProgram(program);
        if (status != GL_TRUE)
            program = 0;
    } else {
        LOGE("Failed to create program\n");
    }
    return program;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    LOGI("loading vertex shader");
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    LOGI("loading fragment shader");
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = createProgram(vertexShader, pixelShader);
    return program;
}

GLuint gProgram;
GLuint gvPositionHandle;

GLfloat vertexPos[3 * 3] = { 0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
        0.0f };
GLfloat colors[4 * 3] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f };

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);
    char * gVertexShader = load_asset("shaders/vertex_attrib.gls");
    char * gFragmentShader = load_asset("shaders/fragment_attrib.gls");
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }

    glViewport(0, 0, w, h);
    checkGlError("glViewport");

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, colors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, vertexPos);
    glBindAttribLocation(gProgram, 0, "a_color");
    glBindAttribLocation(gProgram, 1, "a_position");

    return true;
}

class Shader {
private:
    string _source;
    GLuint _id;
    GLenum _type;
    char * _log;
public:
    Shader() {
        throwJNI("Cannot initialize shader without source");
    }
    ;
    Shader(const Shader& copy) {
        _log = 0;
        _id = copy._id;
        _type = copy._type;
        _source = copy._source;
    }
    ;
    Shader(const char * source, GLenum shaderType) {
        _source = string(source);
        _type = shaderType;
        _id = 0;
        _log = 0;
        _compile();
    }
    ;

    char * getInfo() {
        if (_log)
            delete[] _log;
        _log = 0;
        GLint infoLen = 0;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &infoLen);
        LOGE("Log length %i\n", infoLen);
        if (infoLen) {
            _log = new char[infoLen];
            if (_log)
                return _log;
            else
                LOGE("Failed to allocate memory for program log");
        }
        return _log;
    }

    bool isValid() {
        return _id != 0;
    }

    GLuint _compile() {
        GLuint _id = glCreateShader(_type);
        if (_id) {
            const char * source = _source.c_str();
            glShaderSource(_id, 1, &source, NULL);
            glCompileShader(_id);
            GLint compiled = 0;
            glGetShaderiv(_id, GL_COMPILE_STATUS, &compiled);
            if (compiled != GL_TRUE) {
                LOGE("Failed to compile shader: %s\n", _source.c_str());
                char * info = getInfo();
                if (info)
                    LOGE("Could not compile shader %d:\n%s\n",
                            _type, info);
                glDeleteShader(_id);
                _id = 0;
            } else {
                LOGI("Shader compilled succesfully\n");
            }
        } else {
            LOGE("Failed to create shader\n");
        }
        return _id;
    }

    GLuint getName() {
        return _id;
    }
};

class Program {
    Shader _vertex;
    Shader _fragment;
    GLuint _id;
    char * _log;

    Program(Shader vertexShader, Shader fragmentShader) {
        _vertex = vertexShader;
        _fragment = fragmentShader;
        _log = 0;
    }

    char * getInfo() {
        if (_log)
            delete[] _log;
        GLint bufLength = 0;
        _log = 0;
        glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &bufLength);
        LOGE("Log length %i\n", bufLength);
        if (bufLength) {
            _log = new char[bufLength];
            if (!_log)
                LOGE("Failed to allocate memory for program log");
        }
        return _log;
    }

    GLuint _link() {
        glLinkProgram(_id);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(_id, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            char * info = getInfo();
            if (info)
                LOGE("Could not link program:\n%s\n", info);
            glDeleteProgram(_id);
            _id = 0;
        } else {
            LOGI("Program linked succesfully\n");
        }
        return linkStatus;
    }

    GLuint _create(GLuint vertexShader, GLuint pixelShader) {
        GLuint _id = glCreateProgram();
        if (_id) {
            glAttachShader(_id, _vertex.getName());
            checkGlError("glAttachShader");
            glAttachShader(_id, _fragment.getName());
            checkGlError("glAttachShader");
            GLuint status = _link();
            if (status != GL_TRUE)
                _id = 0;
        } else {
            LOGE("Failed to create program\n");
        }
        return _id;
    }
};

class Scene {

};

#define assert(cond, mess) if(!cond){LOGE("Failed: %s at %d in file %s: %s", #cond, __LINE__, __FILE__, mess);exit(0);}
#define VERTEX_BUF  0
#define NORMAL_BUF  1
#define TEXTURE_BUF 2
#define INDEX_BUF   3

class Mesh {
    GLuint vboIds[4];
    GLuint strides[4];
    GLuint attrib[3];
    const char * names[3];
    unsigned char sizes[4];
    GLuint program;
public:
    Mesh() {
        glGenBuffers(4, vboIds);
        init();
    }
    ;
    void init() {
        attrib[VERTEX_BUF] = 0;
        attrib[NORMAL_BUF] = 1;
        attrib[TEXTURE_BUF] = 2;
        names[VERTEX_BUF] = "v_position";
        names[NORMAL_BUF] = "v_normal";
        names[TEXTURE_BUF] = "v_texcoord";

        sizes[VERTEX_BUF] = 3;
        sizes[NORMAL_BUF] = 3;
        sizes[TEXTURE_BUF] = 2;
        program = 0;
    }
    void setProgram(GLuint program) {
        this->program = program;
    }
    void setVertices(GLfloat *buf, GLint num, GLuint stride) {
        _setBuffer(buf, num, stride, VERTEX_BUF);
    }
    ;
    void setNormal(GLfloat *buf, GLint num, GLuint stride) {
        _setBuffer(buf, num, stride, NORMAL_BUF);
    }
    ;
    void setTextureCoord(GLfloat *buf, GLint num, GLuint stride) {
        _setBuffer(buf, num, stride, TEXTURE_BUF);
    }
    ;
    void setIndexes(GLushort *buf, GLint num, GLuint stride) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * num, buf,
                GL_STATIC_DRAW);
    }
    ;
    GLuint attribute(int indx) {
        return attrib[indx];
    }
    ;
    void draw() {
        assert(program != 0, "You need to set program for Mesh");
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[VERTEX_BUF]);
        glEnableVertexAttribArray(attribute(VERTEX_BUF));
        glVertexAttribPointer(attribute(VERTEX_BUF), sizes[VERTEX_BUF],
                GL_FLOAT, GL_FALSE, strides[VERTEX_BUF], 0);

        glBindBuffer(GL_ARRAY_BUFFER, vboIds[NORMAL_BUF]);
        glEnableVertexAttribArray(attribute(NORMAL_BUF));
        glVertexAttribPointer(attribute(NORMAL_BUF), sizes[NORMAL_BUF],
                GL_FLOAT, GL_FALSE, strides[NORMAL_BUF], 0);

        glBindBuffer(GL_ARRAY_BUFFER, vboIds[TEXTURE_BUF]);
        glEnableVertexAttribArray(attribute(TEXTURE_BUF));
        glVertexAttribPointer(attribute(TEXTURE_BUF), sizes[TEXTURE_BUF],
                GL_FLOAT, GL_FALSE, strides[TEXTURE_BUF], 0);

        glBindAttribLocation(program, attribute(VERTEX_BUF), names[VERTEX_BUF]);
        glBindAttribLocation(program, attribute(NORMAL_BUF), names[NORMAL_BUF]);
        glBindAttribLocation(program, attribute(TEXTURE_BUF),
                names[TEXTURE_BUF]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
    }
private:
    GLfloat *_vertices;
    GLfloat *_indexes;
    void _setBuffer(GLfloat *buf, GLint num, GLuint stride, GLuint sel) {
        assert(sel >= 0 & sel < 4, "this buffer does not exist");
        strides[sel] = stride;
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[sel]);
        glBufferData(GL_ARRAY_BUFFER, stride * num, buf, GL_STATIC_DRAW);
    }
};

void renderFrame() {
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
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

    LOGI("Loading shader: %s\n", nativeString);
    GLuint shader = loadShader(shaderType, nativeString);

    env->ReleaseStringUTFChars(javaString, nativeString);
    return shader;
}

JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_createProgram(
        JNIEnv * env, jobject obj, jint vertexShader, jint pixelShader) {
    _env = env;
    gProgram = createProgram(vertexShader, pixelShader);
    return gProgram;
}
