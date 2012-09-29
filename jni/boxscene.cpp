/*
 * BoxScene.cpp
 *
 *  Created on: 15-07-2012
 *      Author: Krzysztof
 */
#include <stdint.h>
#include <GLES2/gl2.h>
#include "boxscene.h"
#include "world.h"

BoxScene::BoxScene(EventListener* world, GLuint w, GLuint h) :
        Scene(w, h) {
    float ratio = (float) h / (float) w;
    _listener = world;

    Group * group = new Group();
    _root = group;

    // Location nodes, entities are attached to these nodes.
    mRootLocation = new Node("root");
    mRootLocation->setLocation(0.5f, 0.5f, 0.0f);
    float margin = (ratio - 1);
    _view_matrix = GLMatrix().ortho(-10.0f, 10.0f, (-1-margin)*10.0f, (1+margin)*10.0f, 1.0f, -1.0f);
    _framebuffer_view_matrix = GLMatrix().ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, -1.0f);
    _body_texture = loadBitmap("images/WoodFine_256.png");
    _circle_texture = loadBitmap("images/circle.png");
    ATexture metal = loadBitmap("images/metal.png");

    _screen_texture = new Texture();
    _screen_texture->empty(512, 512);

    fade_out_background = new Rectangle();
    fade_out_background->setTexture(_screen_texture);
    ANode loc = fade_out_background->getLocation();
    loc->setScale(20, -20, 1);
    loc->setLocation(-10, 10, 0);

    _screen_data = new GLubyte[w*h*4];

    _framebuffer = new Framebuffer();
    _framebuffer->setFormat(512, 512);
    //_framebuffer->setDepthStencilBuffer(512, 512);
    //_framebuffer->setColorTextureBuffer(_screen_texture);


    background = new Rectangle();
    background->setTexture(_screen_texture);
    loc = background->getLocation();
    loc->setScale(20, -20, 1);
    loc->setLocation(-10, 10, 1);

    _metal_background = new Rectangle();
    _metal_background->setTexture(metal);
    loc = _metal_background->getLocation();
    loc->setScale(20, -20, 1);
    loc->setLocation(-10, 10, 1);

}

BoxScene::~BoxScene(){
    delete[] _screen_data;
}

void BoxScene::sensor(float x, float y, float z){
    _listener->onSensor(x, y, z);
}

Mesh* BoxScene::_createPolygon(GLfloat* vert, int vert_count) {
    ANode root_location = new Node("root");
    root_location->setLocation(0.5f, 0.5f, 0.0f);
    uint16_t * indexes = new uint16_t[vert_count];
    for (int i = 0; i < vert_count; i++){
        indexes[i] = i;
    }
    Mesh *mesh = new Mesh();
    mesh->setVertices(vert, vert_count);
    mesh->setIndexes(indexes, vert_count);
    mesh->setLocation(root_location);
    delete[] indexes;
    _root->addObject(mesh);
    return mesh;
}

Mesh * BoxScene::_applyTexture(Mesh* mesh, GLfloat* vert, int vert_count, ATexture texture) {
    GLfloat min_x = FLT_MAX;
    GLfloat max_x = -FLT_MAX;
    GLfloat min_y = FLT_MAX;
    GLfloat max_y = -FLT_MAX;
    GLfloat scale_x, scale_y;
    GLfloat* tex_coord = new GLfloat[vert_count*2];
    for (int i = 0; i < vert_count; i++){
        min_x = min(vert[i*3], min_x);
        max_x = max(vert[i*3], max_x);
        min_y = min(vert[i*3+1], min_y);
        max_y = max(vert[i*3+1], max_y);
    }
    scale_x = max_x - min_x;
    scale_y = max_y - min_y;
    for (int i = 0; i < vert_count; i++){
        if(scale_x == 0){
            tex_coord[i*2] = 0;
        }else{
            tex_coord[i*2] = (vert[i*3]-min_x)/scale_x;
        }
        if(scale_y == 0){
            tex_coord[i*2+1] = 0;
        }else{
            tex_coord[i*2+1] = (vert[i*3+1]-min_y)/scale_y;

        }
    }
    mesh->setType(GL_TRIANGLE_FAN);
    mesh->setTexture(texture);
    mesh->setTextureCoord(tex_coord, vert_count);
    delete[] tex_coord;
    return mesh;
}

Mesh* BoxScene::createCircle(float radius) {
    int vert_count = 4;
    float max_coord = -radius;
    float min_coord = -max_coord;
    float *vert = new float[4*3];
    vert[0*3+0] = min_coord;
    vert[0*3+1] = min_coord;
    vert[1*3+0] = max_coord;
    vert[1*3+1] = min_coord;
    vert[2*3+0] = max_coord;
    vert[2*3+1] = max_coord;
    vert[3*3+0] = min_coord;
    vert[3*3+1] = max_coord;
    for(int i = 1; i < 4; i++)vert[i*3+2] = 0.f;  // "z" coordinate
    Mesh *mesh = this->_createPolygon(vert, vert_count);
    this->_applyTexture(mesh, vert, vert_count, _circle_texture);
    _root->addObject(mesh);
    delete[] vert;
    return mesh;
}

Mesh* BoxScene::createPolygon(GLfloat* vert, int vert_count) {
    Mesh *mesh = this->_createPolygon(vert, vert_count);
    if(vert_count > 2){
        mesh = this->_applyTexture(mesh, vert, vert_count, _body_texture);
    }else{
        mesh->setType(GL_LINES);
        mesh->setColorRGB(1.0f, 1.0f, 1.0f);
    }
    _root->addObject(mesh);
    return mesh;
}

void BoxScene::clearScene(){
    _framebuffer->activate();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    _screen_buffer->activate();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
}

void BoxScene::renderFrame(){
    b64assert(_framebuffer->isValid(), "Framebuffe is not valid");
    glViewport(0, 0, 512, 512);
    _framebuffer->activate();

    ARenderVisitor visitor = new RenderVisitor();
    visitor->setProgram(_program);
    _program->activate();
    _program->bindViewMatrix(_framebuffer_view_matrix);
    fade_out_background->draw(visitor);
    _root->draw(visitor);

    glReadPixels(0, 0, 512, 512,  GL_RGBA, GL_UNSIGNED_BYTE, _screen_data);
    checkGlError("glReadPixels");
    _screen_texture->load(512, 512, ARGB_8888, _screen_data);

    _screen_texture->generateMipmap();
    glViewport(0, 0, _w, _h);
    _screen_buffer->activate();
    _program->bindViewMatrix(_view_matrix);
    _metal_background->draw(visitor);
    background->draw(visitor);

}

