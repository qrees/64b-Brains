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
    AProgram _program;
    GLMatrix _view_matrix;
    AFramebuffer _screen_buffer;
    queue<AEvent> _events;
    void _prepareForHit();
    void _draw_hit_check();
    void _process_events();
public:
    Scene(GLuint w=0, GLuint h=0);
    virtual ~Scene();
    void setRoot(AEntity root);
    void setProgram(AProgram program);
    ATexture loadBitmap(GLuint width, GLuint height, const char * source);
    ATexture loadTexture(const char * source);
    AShader loadShader(const char * source, GLuint type);
    virtual void renderFrame();
    virtual void prepareViewMatrix(){};
    virtual void prepareScene(){};
    AEntity hitCheck(int x, int y);
    
    /**
     * Add #event to an event queue to be processed when time comes.
     */
    void addEvent(AEvent event);
    /**
     * Click event on the scene at screen coordinates #x and #y.
     */
    void click(int x, int y);
};

typedef AutoPtr<Scene> AScene;



class MainScene: public Scene{
private:
    AShader _vertex_shader;
    AShader _fragment_shader;
    hash_map<string, ATexture> _texture_map;
    hash_map<string, AEntity> _object_map;
    ANode b_location;
    ANode root_location;
    AMesh b_mesh;
public:
    MainScene(GLuint w=0, GLuint h=0);
    void renderFrame();
    void prepareScene();
};


#endif /* SCENE_H_ */
