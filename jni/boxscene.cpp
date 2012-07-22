/*
 * BoxScene.cpp
 *
 *  Created on: 15-07-2012
 *      Author: Krzysztof
 */
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

    _view_matrix = GLMatrix().ortho(0.0f, 1.0f, 1 - ratio, 1.0f, 1.0f, -1.0f);
}

AMesh BoxScene::createPolygon(GLfloat* vert, int vert_count) {
    unsigned short int * indexes = new int[vert_count];
    for (int i = 0; i < vert_count; i++)
        indexes[i] = i;
    AMesh mesh = new Mesh();
    mesh->setVertices(vert, vert_count);
    //setTextureRect(0.f, 1.f, 1.f, 0.f);
    mesh->setIndexes(indexes, vert_count);
    mesh->setType(GL_LINE_LOOP);
    delete[] indexes;
    return mesh;
}
