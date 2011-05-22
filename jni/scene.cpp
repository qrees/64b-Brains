/*
 * scene.cpp
 *
 *  Created on: 2011-05-22
 *      Author: qrees
 */

#include "engine.h"

Scene::Scene(){
    
}

Scene::~Scene(){
    
}

void Scene::setProgram(AProgram program){
    
}

ATexture Scene::loadTexture(const char * source){
    const u_char * texture = load_raw("images/background.pkm");
    ATexture tex(new Texture());
    tex->load_pkm(texture);
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

MainScene::MainScene():Scene(){
    _vertex_shader = loadShader("shaders/vertex_attrib.gls", GL_VERTEX_SHADER);
    _fragment_shader = loadShader("shaders/fragment_attrib.gls", GL_FRAGMENT_SHADER);
    _program = new Program();
    _program->make(_vertex_shader, _fragment_shader);
    
    Group * group = new Group();
    _root = group;
    AMesh a_mesh = new Rectangle();
    a_mesh->setProgram(_program);
    group->addObject(a_mesh);
    
    ATexture tex = loadTexture("images/background.pkm");
    a_mesh->setTexture(tex);
    
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
    LOGI("Rendering frame");
    _program->activate();
    _program->bindViewMatrix(_view_matrix);
    _root->draw();
}
