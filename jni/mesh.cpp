/*
 * mesh.cpp
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#include "engine.h"

Mesh::Mesh() {
    init();
}
Mesh::~Mesh(){
    LOGI("Deleting buffers:");
    for(int i = 0; i < BUF_COUNT; i++)
        LOGI(" %d = %d", i, vboIds[i]);
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
void Mesh::setTexture(ATexture tex){
    _texture = tex;
}

void Mesh::draw() {
    assert(program->isValid(), "You need to set program for Mesh");
    program->activate();
    program->bindPosition(vboIds[VERTEX_BUF]);
    if(has_normal)
        program->bindNormal(vboIds[NORMAL_BUF]);
    if(has_texture && bool(_texture))
        program->bindTexture(vboIds[TEXTURE_BUF], this->_texture->getName());
    if(has_color)
        program->bindColor(vboIds[COLOR_BUF]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
    checkGlError("glDrawElements");
}
void Mesh::_setBuffer(GLenum target, GLfloat *buf, GLuint size, GLuint sel) {
    LOGI("SetBuffer %d, %d", size, vboIds[sel]);
    assert((sel >= 0) && (sel < BUF_COUNT), "this buffer does not exist");
    //strides[sel] = stride;
    glBindBuffer(target, vboIds[sel]);
    glBufferData(target, size, buf, GL_STATIC_DRAW);
    checkGlError("glBufferData");
}

void Group::addObject(AMesh mesh){
    _objects.push_back(mesh);
}
void Group::draw(){
    list<AMesh>::iterator it;
    for(it = _objects.begin(); it != _objects.end(); it++)
        (*it)->draw();
}

GLfloat rec_vertices[4*3] = { 0.0f, 1.0f, 1.0f, // 0, Top Left
        0.0f, 0.0f, 1.0f, // 1, Bottom Left
        1.0f, 0.0f, 1.0f, // 2, Bottom Right
        1.0f, 1.0f, 1.0f, // 3, Top Right
    };
GLfloat rec_tex_coord[4*2] = { 0.0f, 0.0f, // 0, Top Left
        0.0f, 1.0f,  // 1, Bottom Left
        1.0f, 1.0f,  // 2, Bottom Right
        1.0f, 0.0f,  // 3, Top Right
    };
GLushort rec_indexes[2 * 3] = { 0, 1, 2, 0, 2, 3 };
Rectangle::Rectangle():Mesh(){
    setVertices(rec_vertices, 4);
    setTextureCoord(rec_tex_coord, 4);
    setIndexes(rec_indexes, 6);
}
