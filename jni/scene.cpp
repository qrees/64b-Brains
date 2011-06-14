/*
 * scene.cpp
 *
 *  Created on: 2011-05-22
 *      Author: qrees
 */

#include "engine.h"

/*
 * Scene implementation
 */
Scene::Scene(GLuint w, GLuint h){
    _w = w;
    _h = h;
    _screen_buffer = new ScreenBuffer;
    _prepareForHit();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    AShader _vertex_shader = loadShader("shaders/vertex_attrib.gls", GL_VERTEX_SHADER);
    AShader _fragment_shader = loadShader("shaders/fragment_attrib.gls", GL_FRAGMENT_SHADER);
    _program = new Program();
    _program->make(_vertex_shader, _fragment_shader);
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

void Scene::_prepareForHit(){
    if(_hit_program)
        return;
    AShader _vertex_shader = loadShader("shaders/vertex_attrib.gls", GL_VERTEX_SHADER);
    AShader _fragment_shader = loadShader("shaders/fragment_hit.gls", GL_FRAGMENT_SHADER);
    _hit_program = new Program();
    _hit_program->make(_vertex_shader, _fragment_shader);
    _hit_program->activateSolidColor();
    
    _pixels = new Texture();
    _pixels->empty(_w, _h);
    
    _framebuffer = new Framebuffer();
    _framebuffer->setColorBuffer(_w, _h);
    _framebuffer->setDepthStencilBuffer(_w, _h);
    assert(_framebuffer->isValid(), "Framebuffer is not valid");
}

void Scene::clearScene(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
}

void Scene::prepareScene(){
    clearScene();
    prepareViewMatrix();
}

void Scene::_draw_hit_check(){
    _framebuffer->activate();
    
    prepareScene();
    
    AHitVisitor visitor = new HitVisitor();
    visitor->setHitProgram(_hit_program);
    _hit_program->activate();
    _hit_program->bindViewMatrix(_view_matrix);
    _root->_draw_hit_check(visitor);
}

AEntity Scene::hitCheck(int x, int y){
    GLubyte data[4];
    
    _prepareForHit();

    //assert(_framebuffer->isValid(), "Framebuffer is not valid");
    _draw_hit_check();
    //glFinish();
    glReadPixels(x, _h-y, 1, 1,  GL_RGBA, GL_UNSIGNED_BYTE, data);
    checkGlError("glReadPixels");
    Entity* entity = _root->getEntityForColor(data);
    return entity;
}

void Scene::addEvent(AEvent event){
    _events.push(event);
}

void Scene::down(int x, int y){
    _clicked = hitCheck(x, y);
    if(_clicked)
        ((Mesh*)(_clicked.m_ptr))->down(x, y);
}

void Scene::move(int x, int y){
    if(_clicked)
        ((Mesh*)(_clicked.m_ptr))->move(x, y);
}

void Scene::up(){
    if(_clicked)
        ((Mesh*)(_clicked.m_ptr))->up();
}

void Scene::_process_events(){
    AEvent event;
    while(not _events.empty()){
        event = _events.front();
        _events.pop();
        event->process(*this);
    }
}

/*
 * MainScene implementation
 * 
 * This is example Scene class to test various features.
 */

MainScene::MainScene(GLuint w, GLuint h):Scene(w, h){
    float ratio = (float)h/(float)w;

    Group * group = new Group();
    _root = group;
        
    root_location = new Node();
    root_location->setLocation(0.5f, 0.5f, 0.0f);
    ANode a_location = new Node();
    a_location->setParent(root_location);
    a_location->setLocation(-0.5f, 0.5f-ratio, 0.0f);
    a_location->setScale(1, ratio, 1);
    b_location = new Node();
    b_location->setParent(root_location);
    b_location->setLocation(-0.5f, -0.f, 0.0f);
    b_location->setEulerRotation(180, 0, 180);

    AMesh a_mesh = new Rectangle();
    group->addObject(a_mesh);
    ATexture tex = loadTexture("images/background.pkm");
    a_mesh->setTexture(tex);
    a_mesh->setLocation(a_location);
    
    Rectangle * rec = new Rectangle(0.3f);
    b_mesh = rec;
    rec->setLocation(b_location);
    group->addObject(b_mesh);
    ATexture tex_buttons = loadBitmap(1024, 666, "images/buttons.png");
    _texture_map["buttons"] = tex_buttons;
    rec->setTexture(tex_buttons);
    rec->setTextureRect(0.f, 100.f/666.f, 320.f/1024.f, 200.f/666.f);
    //rec->setTextureCoord(b_texture_coord, 4);
    rec->setHitable(true);

    _view_matrix = GLMatrix().ortho(0.0f, 1.0f, 1-ratio, 1.0f, 1.0f, -1.0f);
    //_view_matrix = GLMatrix().ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);
}

void MainScene::prepareScene(){
    Scene::prepareScene();
    timeval curr_time;
    gettimeofday(&curr_time, NULL);
    //double t = (double)(curr_time.tv_sec % 360) + (double)(curr_time.tv_usec)/1000000.0f;    
}

void MainScene::renderFrame(){
    _process_events();
    
    _screen_buffer->activate();
    
    prepareScene();

    ARenderVisitor visitor = new RenderVisitor();
    visitor->setProgram(_program);
    _program->activate();
    _program->bindViewMatrix(_view_matrix);
    _root->draw(visitor);
}

