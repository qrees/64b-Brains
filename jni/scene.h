/*
 * scene.h
 *
 *  Created on: 2011-05-22
 *      Author: qrees
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <hash_map>
#include "render_visitor.h"

class Scene: public RefCntObject {
private:
    AProgram _hit_program;
protected:
    AEntity _root;
    AProgram _program;
    GLMatrix _view_matrix;
public:
    Scene();
    virtual ~Scene();
    void setRoot(AEntity root);
    void setProgram(AProgram program);
    ATexture loadBitmap(GLuint width, GLuint height, const char * source);
    ATexture loadTexture(const char * source);
    AShader loadShader(const char * source, GLuint type);
    virtual void renderFrame();
    AEntity _hit_check();
};
typedef AutoPtr<Scene> AScene;

class MainScene: public Scene{
private:
    AShader _vertex_shader;
    AShader _fragment_shader;
    hash_map<string, ATexture> _texture_map;
    hash_map<string, AEntity> _object_map;
    
public:
    MainScene();
    void renderFrame();
};


#endif /* SCENE_H_ */
