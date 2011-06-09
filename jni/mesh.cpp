/*
 * mesh.cpp
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#include "engine.h"

/*
 * Node class implementation
 */
Node::Node(){
    _name = "";
    init();
}

Node::Node(string name){
    _name = name;
    init();
}

void Node::init(){
    _matrix.identity();
    _local_matrix.identity();
    _parent = 0;
    _valid = true;
    _rotation = GLQuaternion::idt();
}

void Node::setParent(AutoPtr<Node> parent){
    _parent = parent;
    _valid = false;
}

void Node::setOrientation(GLMatrix matrix){
    _local_matrix = matrix;
    _valid = false;
}

GLMatrix& Node::getLocalMatrix(){
    return _local_matrix;
}


void Node::update(){
    /*
    _rotation.toMatrix(_local_matrix);
    GLMatrix loc_matrix;
    loc_matrix.position(_x, _y, _z);
    _local_matrix = loc_matrix * _local_matrix;
    */
    GLMatrix loc_matrix;
    _rotation.toMatrix(loc_matrix);
    _local_matrix.position(_x, _y, _z);
    _local_matrix._multiply(loc_matrix);
    
    if(_parent){
        GLMatrix parent_matrix = _parent->getMatrix();
        _matrix = parent_matrix * _local_matrix;
    }else{
        _matrix = _local_matrix;
    }
    _valid = true;
}

GLMatrix& Node::getMatrix(){
    //if(not _valid) # TODO : changing location of parent should mark descendants as invalid
        update();
    return _matrix;
}

void Node::setLocation(GLfloat x, GLfloat y, GLfloat z){
    _x = x;
    _y = y;
    _z = z;
    _valid = false;
}

void Node::setRotation(GLfloat x, GLfloat y, GLfloat z, GLfloat angle){
    _rotation.setFromAxis(x, y, z, angle);
    _valid = false;
}


/*
 * Entity class implementation
 */
Entity::Entity(){
    _location = new Node();
}

void Entity::setLocation(ANode location){
    _location = location;
}


/*
 * Mesh class implementation
 */
Mesh::Mesh() {
    init();
}

Mesh::~Mesh(){
    glDeleteBuffers(BUF_COUNT, vboIds);
}

void Mesh::init() {
    glGenBuffers(BUF_COUNT, vboIds);
    has_color = has_normal = has_texture = false;
    _solid_color = 0;
    _hit_color = 0;
    _hitable = false;
}

void Mesh::setProgram(AProgram program) {
    this->_program = program;
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

GLint Mesh::_hit_color_seq = 200; // 0 is color for empty space
void Mesh::setHitable(bool hitable){
    _hitable = hitable;
    if(_hitable){
        if(_hit_color == 0){
            _hit_color = new GLfloat[4];
            _hit_color[0] = (float)(_hit_color_seq & ALPHA_BYTE) / 255.0f;
            _hit_color[1] = (float)((_hit_color_seq & BLUE_BYTE) >> 8) / 255.0f;
            _hit_color[2] = (float)((_hit_color_seq & GREEN_BYTE) >> 16) / 255.0f;
            _hit_color[3] = 1.0f;
            LOGI("_hit_color = %f %f %f %f", _hit_color[0], _hit_color[1], _hit_color[2], _hit_color[3]);
            _hit_color_seq++;
        }
    }
}

void Mesh::draw() {
    assert(_program->isValid(), "You need to set program for Mesh");
    _program->activate();
    _program->bindPosition(vboIds[VERTEX_BUF]);
    if(has_normal)
        _program->bindNormal(vboIds[NORMAL_BUF]);
    if(has_texture && bool(_texture))
        _program->bindTexture(vboIds[TEXTURE_BUF], this->_texture->getName());
    if(has_color)
        _program->bindColor(vboIds[COLOR_BUF]);
    
    _program->bindModelMatrix(_location->getMatrix());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
    checkGlError("glDrawElements");
}

void Mesh::_draw_hit_check(ARenderVisitor visitor){
    AHitVisitor hit_visitor = static_cast<HitVisitor*>(visitor.m_ptr);
    AProgram program = hit_visitor->getHitProgram();
    program->bindPosition(vboIds[VERTEX_BUF]);
    program->bindSolidColor(_hit_color);
    program->bindModelMatrix(_location->getMatrix());
    
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

/*
 * Group class implementation
 */
void Group::addObject(AMesh mesh){
    _objects.push_back(mesh);
}

void Group::draw(){
    list<AMesh>::iterator it;
    for(it = _objects.begin(); it != _objects.end(); it++)
        (*it)->draw();
}

void Group::_draw_hit_check(ARenderVisitor visitor){
    list<AMesh>::iterator it;
    for(it = _objects.begin(); it != _objects.end(); it++)
        (*it)->_draw_hit_check(visitor);
}


/*
 * Rectangle class implementation
 */
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
