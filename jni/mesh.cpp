/*
 * mesh.cpp
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

//#include "engine.h"

#include "mesh.h"
#include "render_visitor.h"

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

Node::~Node(){

}

void Node::init(){
    _matrix.identity();
    _local_matrix.identity();
    _parent = 0;
    _valid = true;
    _rotation = GLQuaternion::idt();
    _sx = _sy = _sz = 1.0f;
    _x = _y = _z = 0.f;
}

std::string f_to_s(float val){
    std::ostringstream sstr;
    sstr << val;
    return sstr.str();
}

void Node::setFrom(ANode other){
    this->_x = other->_x;
    this->_y = other->_y;
    this->_z = other->_z;
    this->_sx = other->_sx;
    this->_sy = other->_sy;
    this->_sz = other->_sz;
    this->_rotation = other->_rotation;
    invalidate();
}

void Node::blend(ANode other, float alpha){
    _x = _x + ((other->_x - _x) * alpha);
    _y = _y + ((other->_y - _y) * alpha);
    _z = _z + ((other->_z - _z) * alpha);
    _rotation.lerp(other->_rotation, alpha);
    _sx = _sx + ((other->_sx - _sx) * alpha);
    _sy = _sy + ((other->_sy - _sy) * alpha);
    _sz = _sz + ((other->_sz - _sz) * alpha);
    invalidate();
}

string Node::getName(){
    if(_parent)
        return _name + " -> " + _parent->getName();
    else
        return _name;
}

void Node::invalidate(){
	set<Node*>::iterator iter;
	if(not _valid)
		return;
	for (iter = _children.begin(); iter != _children.end(); iter++){
		(*iter)->invalidate();
	}
	_valid = false;
}

void Node::setParent(ANode parent){
	if(_parent){
		_parent->removeChild(this);
	}
    _parent = parent;
    _parent->addChild(this);
    invalidate();
}

ANode Node::getParent(){
    return _parent;
}

void Node::removeChild(Node* node){
	b64assert(_children.count(node) == 1, "Cannot remove this node as it's not one of my children.");
	_children.erase(node);
}

void Node::addChild(Node* node){
	b64assert(_children.count(node) == 0, "Cannot add this node again as a child.");
	_children.insert(node);
}

void Node::setOrientation(GLMatrix matrix){
    _local_matrix = matrix;
    _valid = false;
}

GLMatrix& Node::getLocalMatrix(){
    return _local_matrix;
}


void Node::update(){
    GLMatrix loc_matrix;
    _rotation.toMatrix(loc_matrix);
    _local_matrix.position(_x, _y, _z);
    _local_matrix.scale(_sx, _sy, _sz);
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
    if(not _valid)
        update();
    return _matrix;
}

void Node::updateLocation(GLfloat x, GLfloat y, GLfloat z){
    _x = _x + x;
    _y = _y + y;
    _z = _z + z;
    invalidate();
}

void Node::setLocation(GLfloat x, GLfloat y, GLfloat z){
    _x = x;
    _y = y;
    _z = z;
    invalidate();
}

void Node::setRotation(GLfloat x, GLfloat y, GLfloat z, GLfloat angle){
    _rotation.setFromAxis(x, y, z, angle);
    invalidate();
}

void Node::setEulerRotation(GLfloat yaw, GLfloat pitch, GLfloat roll){
    _rotation.setEulerAngles(yaw, pitch, roll);
    invalidate();
}

void Node::setScale(GLfloat x, GLfloat y, GLfloat z){
    _sx = x;
    _sy = y;
    _sz = z;
    invalidate();
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

ANode Entity::getLocation(){
    return this->_location;
}

Entity* Entity::getEntityForColor(GLubyte*){
    return this;
}

/*
 * Mesh class implementation
 */
Mesh::Mesh() {
    init();
}

Mesh::~Mesh(){
    glDeleteBuffers(BUF_COUNT, vboIds);
    checkGlError("glDeleteBuffers");
    delete[] _solid_color;
    delete[] _hit_color;
}

void Mesh::init() {
    glGenBuffers(BUF_COUNT, vboIds);
    checkGlError("glGenBuffers");
    has_color = has_normal = has_texture = false;
    _solid_color = 0;
    _hit_color = 0;
    _hitable = false;
    _type = GL_TRIANGLES;

    _tex_mult_r = _tex_mult_g = _tex_mult_b = _tex_mult_a = 1.f;
    _tex_fade_r = _tex_fade_g = _tex_fade_b = _tex_fade_a = 0.f;
}

void Mesh::setProgram(AProgram program) {
    this->_program = program;
}

void Mesh::setVertices(GLfloat *buf, GLint num) {
    _setBuffer(GL_ARRAY_BUFFER, buf, sizeof(GLfloat) * 3 * num, VERTEX_BUF);
}
/*
void Mesh::setNormal(GLfloat *buf, GLint num) {
    has_normal = true;
    _setBuffer(GL_ARRAY_BUFFER, buf, sizeof(GLfloat) * 3 * num, NORMAL_BUF);
}*/

void Mesh::setTextureCoord(GLfloat *buf, GLint num) {
    has_texture = true;
    _setBuffer(GL_ARRAY_BUFFER, buf, sizeof(GLfloat) * 2 * num, TEXTURE_BUF);
}

void Mesh::setTextureMultipler(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
    _tex_mult_r = r;
    _tex_mult_g = g;
    _tex_mult_b = b;
    _tex_mult_a = a;
}

void Mesh::setTextureFade(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
    _tex_fade_r = r;
    _tex_fade_g = g;
    _tex_fade_b = b;
    _tex_fade_a = a;
}

void Mesh::setColorRGBA(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
    has_color = true;
    if(_solid_color == 0){
        _solid_color = new GLfloat[4];
    }
    _solid_color[0] = r; _solid_color[1] = g; _solid_color[2] = b; _solid_color[3] = a;
}

void Mesh::setColorRGB(GLfloat r, GLfloat g, GLfloat b){
    //has_solid_color = true;
    has_color = true;
    if(_solid_color == 0){
        _solid_color = new GLfloat[4];
    }
    _solid_color[0] = r; _solid_color[1] = g; _solid_color[2] = b; _solid_color[3] = 1.0f;
}

/*
void Mesh::setColor(GLfloat *buf, GLint num) {
    has_color = true;
    _setBuffer(GL_ARRAY_BUFFER, buf, sizeof(GLfloat) * 4 * num, COLOR_BUF);
}*/

void Mesh::setIndexes(GLushort *buf, GLint num) {
    numIndices = num;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * num, buf,
            GL_STATIC_DRAW);
}

void Mesh::setTexture(ATexture tex){
    has_color = false;
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
            _hit_color_int = _hit_color_seq;
            LOGI("_hit_color = %f %f %f %f", _hit_color[0], _hit_color[1], _hit_color[2], _hit_color[3]);
            _hit_color_seq++;
        }
    }
}

void Mesh::drawPrepare(ARenderVisitor visitor){
    
}

void Mesh::draw(ARenderVisitor visitor) {
    drawPrepare(visitor);
    
    AProgram program;
    if(_program){
        program = _program;
    }else{
        program = visitor->getProgram();
    }
    b64assert(program, "You need to set program for Mesh");
    b64assert(program->isValid(), "Mesh program is not valid");
    
    program->activate();
    program->bindPosition(vboIds[VERTEX_BUF]);
    //if(has_normal)
    //    program->bindNormal(vboIds[NORMAL_BUF]);
    if(has_texture && bool(_texture)) {
        program->bindTexture(vboIds[TEXTURE_BUF], this->_texture->getName());
    }
    program->uniform4f("u_texture_multipler", _tex_mult_r, _tex_mult_g, _tex_mult_b, _tex_mult_a);
    program->uniform4f("u_texture_fade", _tex_fade_r, _tex_fade_g, _tex_fade_b, _tex_fade_a);
    program->useColor(has_color);
    if(has_color)
        program->bindSolidColor(_solid_color);

    program->bindModelMatrix(_location->getMatrix());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
    glDrawElements(getType(), numIndices, GL_UNSIGNED_SHORT, 0);
    checkGlError("glDrawElements");
}

void Mesh::_draw_hit_check(ARenderVisitor visitor){
    AHitVisitor hit_visitor = static_cast<HitVisitor*>(visitor.m_ptr);
    if(not _hitable)
        return;
    AProgram program = hit_visitor->getHitProgram();
    program->bindPosition(vboIds[VERTEX_BUF]);
    program->bindSolidColor(_hit_color);
    program->bindModelMatrix(_location->getMatrix());
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[INDEX_BUF]);
    glDrawElements(getType(), numIndices, GL_UNSIGNED_SHORT, 0);
    checkGlError("glDrawElements");
}

void Mesh::_setBuffer(GLenum target, GLfloat *buf, GLuint size, GLuint sel) {
	b64assert((sel >= 0) && (sel < BUF_COUNT), "this buffer does not exist");
    glBindBuffer(target, vboIds[sel]);
    glBufferData(target, size, buf, GL_STATIC_DRAW);
    checkGlError("glBufferData");
}

Entity* Mesh::getEntityForColor(GLubyte* color){
    if(not _hitable)
        return NULL;
    if (_hit_color_int == ((color[0]) |(color[1] << 8) |(color[2] << 16)) ){
        LOGI("Found mesh hit");
        return this;
    }else{
        return NULL;
    }
}

void Mesh::setType(GLenum type){
    switch (type) {
        case GL_POINTS:
        case GL_LINES:
        case GL_LINE_LOOP:
        case GL_LINE_STRIP:
        case GL_TRIANGLES:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
            _type = type;
            break;
        default:
            LOGE("Incorrect type for mesh given.");
            break;
    }
}

GLenum Mesh::getType(){
    return _type;
}
/*
 * Group class implementation
 */
void Group::addObject(AMesh mesh){
    _objects.push_back(mesh);
}

void Group::draw(ARenderVisitor visitor){
    list<AMesh>::iterator it;
    for(it = _objects.begin(); it != _objects.end(); it++)
        (*it)->draw(visitor);
}

void Group::_draw_hit_check(ARenderVisitor visitor){
    list<AMesh>::iterator it;
    for(it = _objects.begin(); it != _objects.end(); it++)
        (*it)->_draw_hit_check(visitor);
}

Entity* Group::getEntityForColor(GLubyte* color){
    list<AMesh>::iterator it;
    Entity* entity;
    for(it = _objects.begin(); it != _objects.end(); it++){
        entity = (*it)->getEntityForColor(color);
        if(entity)
            return entity;
    }
    return NULL;
}


/*
 * Rectangle class implementation
 */

GLushort rec_indexes[4] = { 0, 1, 2, 3 };

Rectangle::Rectangle(float aspect):Mesh(){
    GLfloat vert[3 * 4];
    vert[2] = vert[5] = vert[8] = vert[11] = 0.f;
    vert[0 * 3 + 0] = 0.f;    // top left
    vert[0 * 3 + 1] = aspect;
    vert[1 * 3 + 0] = 0.f;    // bottom left
    vert[1 * 3 + 1] = 0.f; 
    vert[2 * 3 + 0] = 1.f;    // top right
    vert[2 * 3 + 1] = aspect;
    vert[3 * 3 + 0] = 1.f;    // bottom right
    vert[3 * 3 + 1] = 0.f;
    setVertices(vert, 4);
    setTextureRect(0.f, 1.f, 1.f, 0.f);
    setIndexes(rec_indexes, 4);
    setType(GL_TRIANGLE_STRIP);
}

void Rectangle::setTextureRect(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2){
    GLfloat coords[8];
    coords[0] = x1;
    coords[1] = y2;
    coords[2] = x1;
    coords[3] = y1;
    coords[4] = x2;
    coords[5] = y2;
    coords[6] = x2;
    coords[7] = y1;
    setTextureCoord(coords, 4);
}

/*
 * Button methods implementation
 */
Button::Button(GLfloat sx, GLfloat sy):Rectangle(1){
    _state_textures = 0;
    _state = NORMAL;
    _dirty = true;
    setTextureSize(sx, sy);
    setStateCount(4); // button has 4 states: normal, pressed, active, disabled
    setHitable(true); // Button can be pressed by default
}

Button::~Button(){
    delete[] _state_textures;
}

void Button::setStateCount(int count){
	b64assert(count > 0, "Button need more than 0 states");
    delete[] _state_textures;
    _state_textures = new GLfloat[count*2];
    _state_count = count;
}

void Button::setState(ButtonState state){
	b64assert((state < _state_count) && (state >= 0), "Button: Invalid state");
    if (state != _state){
        _state = state;
        _dirty = true;
    }
}

void Button::setTextureSize(GLfloat sx, GLfloat sy){
    _sx = sx;
    _sy = sy;
}

void Button::setStateTexture(int state, GLfloat x, GLfloat y){
	b64assert(_state_textures, "Button: No states available.");
    _state_textures[state*2 + 0] = x;
    _state_textures[state*2 + 1] = y;
}

void Button::down(float x, float y){
    setState(PRESSED);
}

void Button::up(float x, float y){
    setState(NORMAL);
    click();
}

void Button::click(){

}

void Button::drawPrepare(ARenderVisitor visitor){
	b64assert(_state_textures, "Button: No states available.");
    if(_dirty){
        GLfloat tw = (float)(_texture->getWidth());
        GLfloat th = (float)(_texture->getHeight());
        GLfloat x1 = (float)(_state_textures[_state*2+0])/tw;
        GLfloat y2 = (float)(_state_textures[_state*2+1])/th;
        GLfloat x2 = x1 + ((float)_sx/tw);
        GLfloat y1 = y2 + ((float)_sy/th);
        LOGI("Button is dirty, updating texture: %f, %f, %f, %f", x1, y1, x2, y2);
        
        setTextureRect(x1, y1, x2, y2);
        _dirty = false;
    }
}

