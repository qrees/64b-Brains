/*
 * scene.cpp
 *
 *  Created on: 2011-05-22
 *      Author: qrees
 */

#include "engine.h"

Scene::Scene(GLuint w, GLuint h){
    _w = w;
    _h = h;
}

Scene::~Scene(){
    
}

void Scene::setProgram(AProgram program){
    
}

ATexture Scene::loadBitmap(GLuint width, GLuint height, const char * source){
    u_char * texture = load_bitmap(source);
    ATexture tex(new Texture());
    tex->load(width, height, texture);
    delete[] texture;
    return tex;
}
ATexture Scene::loadTexture(const char * source){
    const u_char * texture = load_raw(source);
    ATexture tex(new Texture());
    tex->load_pkm(texture);
    delete[] texture;
    return tex;
}

AShader Scene::loadShader(const char * source, GLuint type){
    const char * shader_text = load_asset(source);
    AShader  shader = AShader(new Shader());
    shader->load(shader_text, type);
    delete shader_text;
    return shader;
}

void Scene::renderFrame(){
    LOGI("Scene: called stub renderFrame method.");
}

MainScene::MainScene(GLuint w, GLuint h):Scene(w, h){
    _vertex_shader = loadShader("shaders/vertex_attrib.gls", GL_VERTEX_SHADER);
    _fragment_shader = loadShader("shaders/fragment_attrib.gls", GL_FRAGMENT_SHADER);
    _program = new Program();
    _program->make(_vertex_shader, _fragment_shader);
    
    Group * group = new Group();
    _root = group;
    AMesh a_mesh = new Rectangle();
    AMesh b_mesh = new Rectangle();
    b_mesh->setProgram(_program);
    a_mesh->setProgram(_program);
    //group->addObject(a_mesh);
    group->addObject(b_mesh);
    
    ATexture tex = loadTexture("images/background.pkm");
    a_mesh->setTexture(tex);
    ATexture tex_buttons = loadBitmap(1024, 666, "images/buttons.png");
    _texture_map["buttons"] = tex_buttons;
    b_mesh->setTexture(tex_buttons);
    b_mesh->setHitable(true);
    
    _frabuffer = new Framebuffer();
    _frabuffer->setFormat(_w, _h);
    
    _view_matrix = GLMatrix().ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f);
}

void MainScene::renderFrame(){
    static float grey;
    grey = 0.5f;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
    
    //_view_matrix.rotate(1, 0, 0, 1);
    _program->activate();
    _program->bindViewMatrix(_view_matrix);
    _root->draw();
}

AEntity Scene::_hit_check(){
    if(not _hit_program){
        AShader _vertex_shader = loadShader("shaders/vertex_attrib.gls", GL_VERTEX_SHADER);
        AShader _fragment_shader = loadShader("shaders/fragment_hit.gls", GL_FRAGMENT_SHADER);
        _hit_program = new Program();
        _hit_program->make(_vertex_shader, _fragment_shader);
        _hit_program->activateSolidColor();
    }

    static float grey;
    grey = 0.5f;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
    
    AHitVisitor visitor = new HitVisitor();
    visitor->setHitProgram(_hit_program);
    _hit_program->activate();
    _hit_program->bindViewMatrix(_view_matrix);
    _root->_draw_hit_check(visitor);
    return _root;
}
