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
    AEntity _clicked;
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
    
    /**
     * XXX: load* methods should probably be moved to separate loader class
     * 
     * loadBitmap loads image from *source file in assets directory.
     * It actually makes call to static Java method that returns parsed image data
     * with additional header. Data is in form:
     *     
     *     4 bytes  width
     *     4 bytes  height
     *     4 bytes  format (565, 4444, 8888 etc.)
     *     width*height*bpp  pixels, bpp depends on format
     * This is done to overcome need to create separe class in Java and link it with
     * C++ code.
     */
    ATexture loadBitmap(const char * source);
    
    /**
     * loads compressed texture in pkm format. Data from this texture can be directly
     * feed to OpenGL if GL_ETC1_RGB8_OES extension is supported.
     */
    ATexture loadTexture(const char * source);
    
    /**
     * Loads OpenGL shader in text format from assets directory. Two shaders
     * (vertex and fragment) are required to create OpenGL Program. Program is
     * required to draw anything on screen.
     */
    AShader loadShader(const char * source, GLuint type);
    virtual void renderFrame();
    virtual void prepareViewMatrix(){};
    virtual void clearScene();
    virtual void prepareScene();
    AEntity hitCheck(int x, int y);
    
    /**
     * Add #event to an event queue to be processed when time comes.
     */
    void addEvent(AEvent event);
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
public:
    MainScene(GLuint w=0, GLuint h=0);
    void renderFrame();
    void prepareScene();
};


#endif /* SCENE_H_ */
