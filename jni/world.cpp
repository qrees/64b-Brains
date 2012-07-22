/*
 * world.cpp
 *
 *  Created on: 08-07-2012
 *      Author: Krzysztof
 */

#include "world.h"
#include "boxscene.h"
#include "log.h"

BoxWorld::BoxWorld() {
    init();
}

void BoxWorld::init() {
    mTimeStep = 1.0f / 60.0f;
    mVelocityIterations = 6;
    mPositionIterations = 2;

    b2Vec2 gravity(0.0f, -10.0f);
    bool doSleep = true;
    mWorld = new b2World(gravity, doSleep);

    mBodyDef.type = b2_dynamicBody;

    mBoxShape.SetAsBox(1.0f, 1.0f);

    mFixtureDef.shape = &mBoxShape;
    mFixtureDef.density = 1.0f;
    mFixtureDef.friction = 0.3f;

    b2Body* body = mWorld->CreateBody(&mBodyDef);
    body->CreateFixture(&mFixtureDef);
}

AScene BoxWorld::initScene() {
    GLfloat *buf = new GLfloat[8*3];  // 8 - max number of vertexes

    mScene = new BoxScene();
    int count = mWorld->GetBodyCount();
    b2Body* bodies = mWorld->GetBodyList();
    for (int i = 0; i < count; i++){
        b2Body* body = &(bodies[i]);

        for (b2Fixture* f = body->GetFixtureList(); f; f = f->m_next) {
            int32 proxyCount = f->m_proxyCount;
            b2Shape* shape = f->GetShape();
            b2Shape::Type shape_type = shape->GetType();
            switch(shape_type){
            case b2Shape::e_polygon:
                b2PolygonShape* polygon = shape;
                int vert_count = polygon->GetVertexCount();

                for (int vi = 0; vi < vert_count; vi++){
                    b2Vec2 vec = polygon->GetVertex(vi);
                    buf[vi*3] = vec.x;
                    buf[vi*3+1] = vec.y;
                    buf[vi*3+2] = 0;
                }
                mScene->createPolygon(buf, vert_count);
                break;
            default:
                b64assert(false, "Unknown fixture type");
                break;
            }

        }
    }
    delete[] buf;

    return mScene;
}

void BoxWorld::destroyScene(){
    mScene = NULL;
}

BoxWorld::~BoxWorld() {
    delete mWorld;
}

void BoxWorld::tick() {
    mWorld->Step(mTimeStep, mVelocityIterations, mPositionIterations);
}
