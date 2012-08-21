/*
 * world.cpp
 *
 *  Created on: 08-07-2012
 *      Author: Krzysztof
 */

#include "common.h"
#include "world.h"
#include "boxscene.h"
#include "log.h"

BoxWorld::BoxWorld() {
    init();
}

void BoxWorld::init() {
    b2BodyDef mBodyDef;
    b2FixtureDef mFixtureDef;
    b2PolygonShape polygonShape;
    mTimeStep = 1.0f / 60.0f;
    mVelocityIterations = 6;
    mPositionIterations = 2;

    b2Vec2 gravity(0.0f, -10.0f);
    bool doSleep = true;
    mWorld = new b2World(gravity, doSleep);

    mBodyDef.type = b2_dynamicBody;

    polygonShape.SetAsBox(0.1f, 0.1f);

    mFixtureDef.shape = &polygonShape;
    mFixtureDef.density = 1.0f;
    mFixtureDef.friction = 0.3f;

    b2Body* body = mWorld->CreateBody(&mBodyDef);
    body->CreateFixture(&mFixtureDef);


    b2BodyDef mBodyDef2;
    b2FixtureDef mFixtureDef2;
    b2PolygonShape polygonShape2;
    mFixtureDef2.shape = &polygonShape2;
    mBodyDef2.type = b2_staticBody; //change body type
    mBodyDef2.position.Set(0,-1); //middle, bottom
    b2Body* staticBody = mWorld->CreateBody(&mBodyDef2);
    polygonShape2.SetAsEdge( b2Vec2(-15,0), b2Vec2(15,0) );
    mFixtureDef2.density = 1.0f;
    mFixtureDef2.friction = 0.3f;
    staticBody->CreateFixture(&mFixtureDef2);

}

AScene BoxWorld::initScene(int w, int h) {
    GLfloat *buf = new GLfloat[8*3];  // 8 - max number of vertexes

    b64assert(!mScene, "Scene for BoxWorld already initiated");
    mScene = new BoxScene(w, h);
    for (b2Body* body = mWorld->GetBodyList(); body; body = body->GetNext()){
        LOGI("Adding body to scene from BoxWorld");

        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            b2Shape* shape = f->GetShape();
            b2Shape::Type shape_type = shape->GetType();
            LOGI("Adding shape to scene from BoxWorld");
            switch(shape_type){
            case b2Shape::e_polygon:{
                    b2PolygonShape* polygon = dynamic_cast<b2PolygonShape*>(shape);
                    int vert_count = polygon->GetVertexCount();

                    LOGI("Polygon shape with %i vertices", vert_count);
                    for (int vi = 0; vi < vert_count; vi++){
                        b2Vec2 vec = polygon->GetVertex(vi);
                        buf[vi*3] = vec.x;
                        buf[vi*3+1] = vec.y;
                        LOGI("Vertex %f %f", vec.x, vec.y);
                        buf[vi*3+2] = 0;
                    }
                    Mesh * mesh = mScene->createPolygon(buf, vert_count);
                    mBodyPolygonMap[mesh] = body;
                }
                break;
            default:
                //b64assert(false, "Unknown fixture type");
                break;
            }

        }
    }
    delete[] buf;
    return mScene;
}

void BoxWorld::detachScene(){
    mBodyPolygonMap.clear();
    mScene = null;
}

BoxWorld::~BoxWorld() {
    delete mWorld;
}

void BoxWorld::tick() {
    mWorld->Step(mTimeStep, mVelocityIterations, mPositionIterations);
    map<Mesh*, b2Body*>::iterator it;
    for(it = mBodyPolygonMap.begin(); it != mBodyPolygonMap.end(); it++){
        b2Vec2 pos = (*it).second->GetPosition();
        LOGI("Body location %f %f", pos.x, pos.y);
    }
}
