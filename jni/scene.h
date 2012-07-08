/*
 * scene.h
 *
 *  Created on: 2011-05-22
 *      Author: qrees
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <hash_map>
#include <queue>
#include "render_visitor.h"
#include "textarea.h"
#include "event.h"

//class Event;
//typedef AutoPtr<Event> AEvent;


/** Scene class
 * 
 * Class that hold information about current scene state, consisting of:
 * renderable objects (#_root), shader programs (#_program), camera position (#_view_matrix).
 * 
 * This class also handles processing of various events (like clicks), and redering.
 */
class Scene: public RefCntObject {
	friend void *gameThread(void*);
private:
	bool _was_hit;
	bool _valid_scene;
	int _hit_x, _hit_y;
	sem_t _queue_read;
	sem_t _queue_write;
	pthread_mutex_t _queue_mutex;
	pthread_mutex_t _animation_mutex;
protected:
    ATexture _pixels;
    AProgram _hit_program;
    AFramebuffer _framebuffer;
    GLuint _w;
    GLuint _h;
    AEntity _root;
    AEntity _clicked;
    AProgram _program;
    GLMatrix _view_matrix;
    AFramebuffer _screen_buffer;
    queue<AEvent> _events;
    list<AAnimation> _animations;
    void _prepareForHit();
    void _processHit();
    void _draw_hit_check();
    void _process_events();
public:
    Scene(GLuint w=0, GLuint h=0);
    virtual ~Scene();
    void setRoot(AEntity root);
    void setProgram(AProgram program);
    void add_animation(AAnimation anim);
    virtual void renderFrame();
    virtual void prepareViewMatrix(){};
    virtual void clearScene();
    virtual void prepareScene();
    void tick();
    void handleAnimations();
    virtual void handle_tick();
    AEntity hitCheck(int x, int y);
    
    void _renderFrame();
    void hit(int x, int y);
    float x_pos(int x);
    float y_pos(int y);
    /**
     * Add #event to an event queue to be processed when time comes.
     */
    void addEvent(AEvent event);
    /**
     * Click event on the scene at screen coordinates #x and #y.
     */
    virtual void down(AEntity, int x, int y);
    virtual void move(int x, int y);
    virtual void up(int x, int y);
    virtual void invalidate();
};

typedef AutoPtr<Scene> AScene;



class MainScene: public Scene{
private:
    AShader _vertex_shader;
    AShader _fragment_shader;
    hash_map<string, ATexture> _texture_map;
    hash_map<string, AEntity> _object_map;
    ANode c_location;
    ANode b_location;
    ANode a_location;
    ANode root_location;
    AMesh b_mesh;
    ATextArea text_mesh;
public:
    MainScene(GLuint w=0, GLuint h=0);
    void renderFrame();
    void prepareScene();
    void handle_tick();
};



/*
 * Scene events
 */



/** ClickEvent class
 *
 * Represents single click event.
 */
class ClickEvent : public Event {
private:
    int _x, _y;
    AEntity _mesh;
public:
    /** Constructor for ClickEvent.
     *
     * @param x Horizontal coordinate of click event
     * @param y Vertical coordinate of click event
     */
    ClickEvent(AEntity, int x, int y);

    /**
     * Should be called by Scene class when event should be processed.
     *
     * @param scene Scene that is going to be affected by this event.
     */
    void process(RefCntObject &scene);
};

/**
 * Represents move of the pointing device on the surface (preceded by ClickEvent)
 */
class MoveEvent : public Event {
private:
    int _x, _y;
public:
    /** Constructor for MoveEvent.
     *
     * @param x Horizontal coordinate of click event
     * @param y Vertical coordinate of click event
     */
    MoveEvent(int x, int y);

    /**
     * Should be called by Scene class when event should be processed.
     *
     * @param scene Scene that is going to be affected by this event.
     */
    void process(RefCntObject &scene);
};

/**
 * Represents lifting of the pointing device.
 */
class UpEvent : public Event {
private:
    int _x, _y;
public:
    /** Constructor for MoveEvent.
     *
     * @param x Horizontal coordinate of click event
     * @param y Vertical coordinate of click event
     */
    UpEvent(int x, int y);

    /**
     * Should be called by Scene class when event should be processed.
     *
     * @param scene Scene that is going to be affected by this event.
     */
    void process(RefCntObject &scene);
};

/**
 * Stops processing of event queue for given scene.
 */
class InvalidateScene : public Event {
public:
    /** Constructor for InvalidateScene.
     */
    InvalidateScene();

    /**
     * Should be called by Scene class when event should be processed.
     *
     * @param scene Scene that is going to be affected by this event.
     */
    void process(RefCntObject &scene);
};

#endif /* SCENE_H_ */
