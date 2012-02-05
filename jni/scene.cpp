/*
 * scene.cpp
 *
 *  Created on: 2011-05-22
 *      Author: qrees
 */

#include "engine.h"

class ClickEvent;
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
    checkGlError("glBlendFunc");
    AShader _vertex_shader = loadShader("shaders/vertex_attrib.gls", GL_VERTEX_SHADER);
    AShader _fragment_shader = loadShader("shaders/fragment_attrib.gls", GL_FRAGMENT_SHADER);
    _program = new Program();
    _program->make(_vertex_shader, _fragment_shader);
    sem_init(&_queue_read, 0, 0);
    sem_init(&_queue_write, 0, 10);
    pthread_mutex_init(&_queue_mutex, NULL);
}

Scene::~Scene(){
    
}

void Scene::setProgram(AProgram program){
    
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
    b64assert(_framebuffer->isValid(), "Framebuffer is not valid");
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

    b64assert(_framebuffer->isValid(), "Framebuffer is not valid");
    _draw_hit_check();
    glFinish();
    glReadPixels(x, _h-y, 1, 1,  GL_RGBA, GL_UNSIGNED_BYTE, data);
    checkGlError("glReadPixels");
    LOGI("Read pixels %i %i %i %i", data[0], data[1], data[2], data[3]);
    Entity* entity = _root->getEntityForColor(data);
    return entity;
}

void Scene::addEvent(AEvent event){
	sem_wait(&_queue_write); // We can write
	pthread_mutex_lock(&_queue_mutex);
    _events.push(event);
    pthread_mutex_unlock(&_queue_mutex);
	sem_post(&_queue_read);  // Allow reader
}

void Scene::hit(int x, int y){
	_was_hit = true;
	_hit_x = x;
	_hit_y = y;
}

void Scene::down(AEntity clicked, int x, int y){
    LOGI("Scene 'down' event");
    _clicked = clicked;
	((Mesh*)(clicked.m_ptr))->down(x_pos(x), y_pos(y));
}

void Scene::move(int x, int y){
	b64assert(_clicked, "_clicked attribute is not set.");
	((Mesh*)(_clicked.m_ptr))->move(x_pos(x), y_pos(y));
}

void Scene::up(int x, int y){
	b64assert(_clicked, "_clicked attribute is not set.");
    ((Mesh*)(_clicked.m_ptr))->up(x_pos(x), y_pos(y));
	_clicked = NULL;
}

float Scene::x_pos(int x){
	return float(_w)/float(x);
}

float Scene::y_pos(int y){
	return float(_h)/float(y);
}

void Scene::_process_events(){
    AEvent event;
    while(true){
    	sem_wait(&_queue_read);  // We can read
    	pthread_mutex_lock(&_queue_mutex);
        event = _events.front();
        _events.pop();
        event->process(*this);
        pthread_mutex_unlock(&_queue_mutex);
    	sem_post(&_queue_write); // Others can write
    }
}

void Scene::_processHit(){
	if(!_was_hit)
		return;
	_was_hit = false;
    _clicked = hitCheck(_hit_x, _hit_y);
    if(_clicked){
        AEvent event = new ClickEvent(_clicked, _hit_x, _hit_y);
        addEvent(event);
    }
}

void Scene::_renderFrame(){
    _process_events();
    prepareScene();
    _processHit();
    renderFrame();
}

/*
 * MainScene implementation
 * 
 * This is example Scene class to test various features.
 */

MainScene::MainScene(GLuint w, GLuint h):Scene(w, h){
    float ratio = (float)h/(float)w;
    // Fonts
    AFont font = new Font("fonts/small-outline.fnt");
    
    // Textures
    ATexture tex_buttons = loadBitmap("images/buttons.png");
    ATexture tex = loadBitmap("images/background.png");
    //ATexture tex = loadTexture("images/background.pkm");
    
    Group * group = new Group();
    _root = group;
        
    // Location nodes, entities are attached to these nodes.
    root_location = new Node("root");
    root_location->setLocation(0.5f, 0.5f, 0.0f);
    a_location = new Node("a");
    a_location->setParent(root_location);
    a_location->setLocation(-0.5f, 0.5f-ratio, 0.0f);
    a_location->setScale(1, ratio, 1);
    b_location = new Node("b");
    b_location->setParent(root_location);
    b_location->setLocation(-0.5f, -0.f, 0.0f);
    //b_location->setEulerRotation(0, 180, 0);
    
    ANode text_location = new Node("txt");
    text_location->setParent(b_location);

    AMesh a_mesh = new Rectangle();
    group->addObject(a_mesh);
    a_mesh->setTexture(tex);
    a_mesh->setLocation(a_location);

    AMesh button_mesh;
    Button*butt = new Button(287, 59);
    button_mesh = butt;
    group->addObject(button_mesh);
    butt->setLocation(b_location);
    butt->setTexture(tex_buttons);
    butt->setStateTexture(0, 12, 125);
    butt->setStateTexture(1, 12, 203);
    butt->setStateTexture(2, 12, 281);
    butt->setStateTexture(3, 12, 359);

    text_mesh = new TextArea();
    group->addObject(text_mesh);
    text_mesh->setFont(font);
    text_mesh->setLocation(text_location);
    text_mesh->setSize(0.1f);
    
    _view_matrix = GLMatrix().ortho(0.0f, 1.0f, 1-ratio, 1.0f, 1.0f, -1.0f);
    //_view_matrix = GLMatrix().ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, -1.0f);
    
    // perspective view:
    //_view_matrix = GLMatrix().perspective(10.f, (float)w/(float)h, 1.f, 1000.f);
    //_view_matrix.translate(-0.5f, 0.5f, -10.f);
}

void MainScene::prepareScene(){
    Scene::prepareScene();
    timeval curr_time;
    gettimeofday(&curr_time, NULL);    
    double t = (double)(curr_time.tv_sec%1000000) + (double)(curr_time.tv_usec)/1000000.0f;
    text_mesh->setText("time: %Lf", t);
    t = t*20;
    t = abs((fmod(t, 360.))-180.f);

    //_view_matrix = GLMatrix().ortho(-t, t, -t, t, 1.0f, -1.0f);
    //a_location->setEulerRotation(0, t, 0);
}

void MainScene::renderFrame(){
    _screen_buffer->activate();
    
    ARenderVisitor visitor = new RenderVisitor();
    visitor->setProgram(_program);
    _program->activate();
    _program->bindViewMatrix(_view_matrix);
    _root->draw(visitor);
}

