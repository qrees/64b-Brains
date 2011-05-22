/*
 * scene.h
 *
 *  Created on: 2011-05-22
 *      Author: qrees
 */

#ifndef SCENE_H_
#define SCENE_H_

class Scene: public RefCntObject {
protected:
    ABaseMesh _root;
    AProgram _program;
public:
    Scene();
    virtual ~Scene();
    void setRoot(ABaseMesh root);
    void setProgram(AProgram program);
    ATexture loadTexture(const char * source);
    AShader loadShader(const char * source, GLuint type);
    void renderFrame();
};
typedef AutoPtr<Scene> AScene;

class MainScene: public Scene{
private:
    AShader _vertex_shader;
    AShader _fragment_shader;
public:
    MainScene();
};


#endif /* SCENE_H_ */
