/*
 * world.h
 *
 *  Created on: 15-07-2012
 *      Author: Krzysztof
 */

#ifndef BOXSCENE_H_
#define BOXSCENE_H_

#include <hash_map>
#include <queue>

#include "scene.h"

using namespace std;

class BoxScene: public Scene{
private:
    AShader _vertex_shader;
    AShader _fragment_shader;
    ANode mRootLocation;
    hash_map<string, ATexture> _texture_map;
    hash_map<string, AEntity> _object_map;
    ATexture _body_texture;
public:
    BoxScene(GLuint w=0, GLuint h=0);
    void renderFrame();
    //void prepareScene();
    //void handle_tick();
    Mesh* createPolygon(GLfloat* vert, int vert_count);
};
typedef AutoPtr<BoxScene> ABoxScene;


#endif /* BOXSCENE_H_ */

