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

#include "world.h"
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
    ATexture _circle_texture;
    ATexture _screen_texture;
    EventListener* _listener;
    GLubyte *_screen_data;
    AFramebuffer _framebuffer;
    GLMatrix _framebuffer_view_matrix;
    ARectangle background, _metal_background;
    ARectangle fade_out_background;
    Mesh* _createPolygon(GLfloat* vert, int vert_count);
    Mesh* _applyTexture(Mesh*, GLfloat* vert, int vert_count, ATexture texture);
public:
    BoxScene(EventListener* world, GLuint w=0, GLuint h=0);
    ~BoxScene();
    void clearScene();
    void renderFrame();
    Mesh* createPolygon(GLfloat* vert, int vert_count);
    Mesh* createCircle(float radius);
    void sensor(float x, float y, float z);
};
typedef AutoPtr<BoxScene> ABoxScene;


#endif /* BOXSCENE_H_ */

