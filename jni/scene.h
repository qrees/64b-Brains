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

class Event;
typedef AutoPtr<Event> AEvent;


/** Scene class
 * 
 * Class that hold information about current scene state, consisting of:
 * renderable objects (#_root), shader programs (#_program), camera position (#_view_matrix).
 * 
 * This class also handles processing of various events (like clicks), and redering.
 */
class Scene: public RefCntObject {
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
    bool _was_touched;
    int _touch_x, _touch_y;
    void _prepareForHit();
    void _draw_hit_check();
    void _process_touch(int x, int y);
public:
    Scene(GLuint w=0, GLuint h=0);
    virtual ~Scene();
    void setRoot(AEntity root);
    void setProgram(AProgram program);
    virtual void renderFrame();
    virtual void prepareViewMatrix(){};
    virtual void clearScene();
    virtual void prepareScene();
    AEntity hitCheck(int x, int y);
    
    /**
     * Indicates that point x/y has been touched. Scene object will
     * remember this event and process it in render thread to find which object
     * has been touched. This can be done only in render thread, becase we need
     * to render the scene.
     */
    void touched(int x, int y);
    
    /**
     * Add #event to an event queue to be processed when time comes.
     */
    void addEvent(AEvent event);
    
    /**
     * Processes one event from scene event queue. This method should be called
     * by event loop thread. 
     */
    void processEvent();
    
    /**
     * Click event on the scene at screen coordinates #x and #y.
     */
    virtual void down(int x, int y);
    virtual void move(int x, int y);
    virtual void up(int x, int y);
};

typedef AutoPtr<Scene> AScene;


class MainScene: public Scene{
private:
    AShader _vertex_shader;
    AShader _fragment_shader;
    hash_map<string, ATexture> _texture_map;
    hash_map<string, AEntity> _object_map;
    ANode b_location;
    ANode a_location;
    ANode root_location;
    AMesh b_mesh;
    ATextArea text_mesh;
public:
    MainScene(GLuint w=0, GLuint h=0);
    void renderFrame();
    void prepareScene();
};


class GameScene: public Scene{
private:
    double start_time;
    double end_time;
    double stage_start_time;
    
public:
    GameScene(GLuint w=0, GLuint h=0);
    void click(AMesh);
    //void renderFrame();
    //void prepareScene();
};

typedef AutoPtr<GameScene> AGameScene;

#endif /* SCENE_H_ */
