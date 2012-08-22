/*
 * program.cpp
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#include "engine.h"

Program::Program(AShader vertexShader, AShader fragmentShader) {
    _id = 0;
    make(vertexShader, fragmentShader);
}

Program::Program() {
    _id = 0;
}

Program::~Program() {
    LOGI("Deleting program %d", getName());
    glDeleteProgram(getName());
    checkGlError("glDeleteProgram");
}

void Program::activateAttributes() {
    initAttribute("a_color");
    initAttribute("a_position");
    initAttribute("a_texture");
    initUniform("s_texture");
    initUniform("u_use_color");
    initUniform("u_solid_color");
    _view_matrix = glGetUniformLocation(getName(), "u_view_matrix");
    _model_matrix = glGetUniformLocation(getName(), "u_model_matrix");
}

void Program::initUniform(const char* uniform_name){
    mUniformMap[uniform_name] = glGetUniformLocation(getName(), uniform_name);
}

void Program::initAttribute(const char *name) {
    mAttributeMap[name] = glGetAttribLocation(getName(), name);
}

void Program::activate() {
    glUseProgram(getName());
}

void Program::bindViewMatrix(GLMatrix &matrix) {
    glUniformMatrix4fv(_view_matrix, 1, GL_FALSE, matrix.data());
    checkGlError("glUniformMatrix4fv");
}

void Program::bindModelMatrix(GLMatrix &matrix) {
    glUniformMatrix4fv(_model_matrix, 1, GL_FALSE, matrix.data());
    checkGlError("glUniformMatrix4fv");
}

void Program::useColor(bool use_color){
    uniform1i("u_use_color", use_color);
}

void Program::uniform1i(const char* uniform_name, GLint value){
    glUniform1i(mUniformMap[uniform_name], value);
}

void Program::bindColor(const void *data) {
    bindAttribute(mAttributeMap["a_color"], 4, GL_FLOAT, 0, data);
}
void Program::bindColorRGB(const void *data) {
    bindAttribute(mAttributeMap["a_color"], 3, GL_FLOAT, 0, data);
}

void Program::bindPosition(const void *data) {
    bindAttribute(mAttributeMap["a_position"], 3, GL_FLOAT, 0, data);
}

void Program::bindPosition(GLuint buf_id) {
    bindBuffer(mAttributeMap["a_position"], buf_id, 3, GL_FLOAT, 0, 0);
}

void Program::bindNormal(GLuint buf_id) {
    bindBuffer(mAttributeMap["a_normal"], buf_id, 3, GL_FLOAT, 0, 0);
}

void Program::bindTexture(GLuint buf_id, GLuint tex_id) {
    bindBuffer(mAttributeMap["a_texture"], buf_id, 2, GL_FLOAT, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    checkGlError("glActiveTexture");
    b64assert(glIsTexture(tex_id) == GL_TRUE, "Trying to bind non existant texture!");
    glBindTexture(GL_TEXTURE_2D, tex_id);
    checkGlError("glBindTexture");
    glUniform1i(mUniformMap["s_texture"], 0);
    checkGlError("glUniform1i");
}

void Program::bindColor(GLuint buf_id) {
    bindBuffer(mAttributeMap["a_color"], buf_id, 4, GL_FLOAT, 0, 0);
}

void Program::bindSolidColor(GLfloat*color) {
    glUniform4fv(mUniformMap["u_solid_color"], 1, color);
}

void Program::bindAttribute(GLuint location, GLuint size, GLenum type,
        GLuint stride, const void *data) {
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, size, type, GL_FALSE, stride, data);
}

void Program::bindBuffer(GLuint location, GLuint buf_id, GLuint size,
        GLenum type, GLuint stride, const void * offset) {
    glBindBuffer(GL_ARRAY_BUFFER, buf_id);
    checkGlError("glBindBuffer");
    glEnableVertexAttribArray(location);
    checkGlError("glEnableVertexAttribArray");
    glVertexAttribPointer(location, size, type, GL_FALSE, stride, 0);
    checkGlError("glVertexAttribPointer");
}

void Program::make(AShader vertexShader, AShader fragmentShader) {
    _vertex = vertexShader;
    _fragment = fragmentShader;
    _log = 0;
    _create();
    activateAttributes();
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
        glGetProgramInfoLog(getName(), bufLength, 0, _log);
        if (!_log)
            LOGE("Failed to allocate memory for program log");
    }
    return _log;
}

GLuint Program::_link() {
    glLinkProgram(getName());
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(getName(), GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        char * info = getInfo();
        if (info)
            LOGE("Could not link program:\n%s", info);
    } else {
        LOGI("Program linked succesfully");
    }
    return linkStatus;
}

GLuint Program::_create() {
    _id = glCreateProgram();
    if (_id) {
        LOGI("Created program %d", getName());
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
