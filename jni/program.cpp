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