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

#ifdef __CDT_PARSER__
#define JNIEXPORT
#define JNICALL
#endif

#define  LOG_TAG    "Brains"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

char * load_asset(const char * source);

JNIEnv * _env;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
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
                }else
    				LOGE("Failed to allocate memory for program log");
                glDeleteShader(shader);
                shader = 0;
            }
        }else{
        	LOGI("Shader compilled succesfully\n");
        }
    }else{
    	LOGE("Failed to create shader\n");
    }
    return shader;
}

GLuint linkProgram(GLuint program){
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
			}else
				LOGE("Failed to allocate memory for program log");
		}
        glDeleteProgram(program);
	}else{
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
        if(status != GL_TRUE)
        	program = 0;
    }else{
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

GLfloat vertexPos[3 * 3] = {
	 0.0f,  0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f
};
GLfloat colors[4 * 3] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f };

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

class Mesh {
public:
	Mesh(){};

private:
	GLfloat *_vertices;
	GLfloat *_indexes;
	
};

void renderFrame() {
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
}

char * load_asset(const char * source){
	jstring jstr = _env->NewStringUTF(source);
	jclass jclass = _env->FindClass( "info/qrees/android/brains/GL2JNILib");
	jmethodID messageMe = _env->GetStaticMethodID(jclass, "loadAsset", "(Ljava/lang/String;)Ljava/lang/String;");
	jobject result = _env->CallStaticObjectMethod(jclass, messageMe, jstr);
	
	const char* str = _env->GetStringUTFChars((jstring) result, NULL);
	LOGI("Loaded asset %s:\n%s", source, str);
	int len = strlen(str);
	char * ret_str = new char[len+1];
	strcpy(ret_str, str);
	ret_str[len] = 0;
	_env->ReleaseStringUTFChars((jstring)result, str);
	return ret_str;
}

extern "C" {
    JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_step(JNIEnv * env, jobject obj);
    JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_loadShader(JNIEnv * env, jobject obj, jstring javaString, jint type);
    JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_createProgram(JNIEnv * env, jobject obj, jint vertexShader, jint pixelShader);
};

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
	_env = env;
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_info_qrees_android_brains_GL2JNILib_step(JNIEnv * env, jobject obj)
{
	_env = env;
    renderFrame();
}

JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_loadShader(JNIEnv * env, jobject obj, jstring javaString, jint shaderType)
{
	_env = env;
	const char *nativeString = env->GetStringUTFChars(javaString, 0);
	
	LOGI("Loading shader: %s\n", nativeString);
	GLuint shader = loadShader(shaderType, nativeString);
	
	env->ReleaseStringUTFChars(javaString, nativeString);
	return shader;
}

JNIEXPORT jint JNICALL Java_info_qrees_android_brains_GL2JNILib_createProgram(JNIEnv * env, jobject obj, jint vertexShader, jint pixelShader)
{
	_env = env;
	gProgram = createProgram(vertexShader, pixelShader);
	return gProgram;
}
