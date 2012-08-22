/*
 * BoxScene.cpp
 *
 *  Created on: 15-07-2012
 *      Author: Krzysztof
 */
#include <stdint.h>
#include <GLES2/gl2.h>
#include "boxscene.h"

BoxScene::BoxScene(GLuint w, GLuint h) :
        Scene(w, h) {
    float ratio = (float) h / (float) w;
    // Fonts
    AFont font = new Font("fonts/small-outline.fnt");

    // Textures
    ATexture tex_buttons = loadBitmap("images/buttons.png");
    ATexture tex = loadBitmap("images/background.png");
    //ATexture tex = loadTexture("images/background.pkm");

    Group * group = new Group();
    _root = group;

    // Location nodes, entities are attached to these nodes.
    mRootLocation = new Node("root");
    mRootLocation->setLocation(0.5f, 0.5f, 0.0f);
    float half = (ratio - 1)/2;
    _view_matrix = GLMatrix().ortho(-10.0f, 10.0f, (-1-half)*10.0f, (1+half)*10.0f, 1.0f, -1.0f);
    _body_texture = loadBitmap("images/background.png");
}

Mesh* BoxScene::createPolygon(GLfloat* vert, int vert_count) {
    ANode root_location = new Node("root");
    root_location->setLocation(0.5f, 0.5f, 0.0f);
    GLfloat min_x = FLT_MAX;
    GLfloat max_x = -FLT_MAX;
    GLfloat min_y = FLT_MAX;
    GLfloat max_y = -FLT_MAX;
    GLfloat scale_x, scale_y;
    uint16_t * indexes = new uint16_t[vert_count];
    GLfloat* tex_coord = new GLfloat[vert_count*2];
    for (int i = 0; i < vert_count; i++){
        indexes[i] = i;
        min_x = min(vert[i*3], min_x);
        max_x = max(vert[i*3], max_x);
        min_y = min(vert[i*3+1], min_y);
        max_y = max(vert[i*3+1], max_y);
    }
    scale_x = max_x - min_x;
    scale_y = max_y - min_y;
    for (int i = 0; i < vert_count; i++){
        if(scale_x == 0){
            tex_coord[i*2] = 0;
        }else{
            tex_coord[i*2] = (vert[i*3]-min_x)/scale_x;
        }
        if(scale_y == 0){
            tex_coord[i*2+1] = 0;
        }else{
            tex_coord[i*2+1] = (vert[i*3+1]-min_y)/scale_y;

        }
    }
    Mesh *mesh = new Mesh();
    mesh->setVertices(vert, vert_count);
    //setTextureRect(0.f, 1.f, 1.f, 0.f);
    mesh->setIndexes(indexes, vert_count);
    if(vert_count > 2){
        mesh->setType(GL_TRIANGLE_FAN);
        mesh->setTexture(_body_texture);
    }else{
        mesh->setType(GL_LINES);
        mesh->setColor(1.0f, 1.0f, 1.0f);
    }
    mesh->setTextureCoord(tex_coord, vert_count);
    mesh->setLocation(root_location);
    delete[] indexes;
    delete[] tex_coord;
    _root->addObject(mesh);
    return mesh;
}

void BoxScene::renderFrame(){
    _screen_buffer->activate();

    ARenderVisitor visitor = new RenderVisitor();
    visitor->setProgram(_program);
    _program->activate();
    _program->bindViewMatrix(_view_matrix);
    _root->draw(visitor);
}

