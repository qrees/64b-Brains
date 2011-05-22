
#include "engine.h"

Shader::Shader() {
    valid = false;
    _id = 0;
    _source = string("");
}
Shader::~Shader(){
    LOGI("Deleting shader %d", _id);
    if(_id)
        glDeleteShader(_id);
}
Shader::Shader(const char * source, GLenum shaderType) {
    _source = string(source);
    _type = shaderType;
    _id = 0;
    load(source, shaderType);
}
void Shader::load(const char*source, GLenum shaderType){
    _source = string(source);
    _type = shaderType;
    if(_id)
        glDeleteShader(_id);
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
        glGetShaderInfoLog(getName(), infoLen, 0, _log);
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
