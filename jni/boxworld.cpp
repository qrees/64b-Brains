/*
 * boxworld.cpp
 *
 *  Created on: 23-08-2012
 *      Author: Krzysztof
 */

#include <stdlib.h>

#include "common.h"
#include "boxworld.h"
#include "boxscene.h"
#include "log.h"

BoxWorld::BoxWorld() {
    init();
}

void BoxWorld::init() {
    b2BodyDef mBodyDef;
    b2FixtureDef mFixtureDef;
    b2PolygonShape polygonShape;
    b2CircleShape circle;
    mTimeStep = 1.0f / 60.0f;
    mVelocityIterations = 2;
    mPositionIterations = 2;

    b2Vec2 gravity(0.0f, -10.0f);
    bool doSleep = false;
    mWorld = new b2World(gravity, doSleep);

    mBodyDef.type = b2_dynamicBody;
    mBodyDef.allowSleep = true;

    polygonShape.SetAsBox(0.4f, 0.4f);

    mFixtureDef.shape = &circle;
    mFixtureDef.density = 1.0f;
    mFixtureDef.friction = 0.3f;
    mFixtureDef.restitution = 0.f;

    //b2Body* body = mWorld->CreateBody(&mBodyDef);
    //body->CreateFixture(&mFixtureDef);
    for(int j = -9; j < 9; j ++){
        for(int i = -9; i < 9; i+=1){
            circle.m_radius = 0.1f +  (float)rand()/(float)RAND_MAX*0.2;
            mBodyDef.position.Set(i, j); //middle, bottom
            b2Body* body = mWorld->CreateBody(&mBodyDef);
            body->CreateFixture(&mFixtureDef);
        }
    }
    edge(-10, -10, 10, -10);
    edge(-10, 10, 10, 10);
    edge(-10, -10, -10, 10);
    edge(10, -10, 10, 10);
}

void BoxWorld::onSensor(float x, float y, float z){
    b2Vec2 vec;
    vec.x = -x;
    vec.y = -y;
    mWorld->SetGravity(vec);
}

void BoxWorld::edge(float sx, float sy, float ex, float ey){
    b2FixtureDef mFixtureDef;
    b2PolygonShape polygonShape;
    b2BodyDef mBodyDef;

    mFixtureDef.density = 1.0f;
    mFixtureDef.friction = 0.3f;
    mFixtureDef.restitution = 0.f;

    mFixtureDef.shape = &polygonShape;
    mBodyDef.type = b2_staticBody; //change body type
    mBodyDef.position.Set(sx, sy); //middle, bottom
    b2Body* staticBody = mWorld->CreateBody(&mBodyDef);
    polygonShape.SetAsEdge( b2Vec2(0, 0), b2Vec2(ex-sx, ey-sy) );
    staticBody->CreateFixture(&mFixtureDef);
}

AScene BoxWorld::initScene(int w, int h) {
    GLfloat *buf = new GLfloat[8*3];  // 8 - max number of vertexes

    b64assert(!mScene, "Scene for BoxWorld already initiated");
    mScene = new BoxScene(this, w, h);
    for (b2Body* body = mWorld->GetBodyList(); body; body = body->GetNext()){

        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            Mesh * mesh = 0;
            b2Shape* shape = f->GetShape();
            b2Shape::Type shape_type = shape->GetType();
            bool valid = true;
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
                        buf[vi*3+2] = 0;
                    }
                    mesh = mScene->createPolygon(buf, vert_count);
                    mBodyPolygonMap[mesh] = body;
                }
                break;
            case b2Shape::e_circle:{
                    b2CircleShape* polygon = dynamic_cast<b2CircleShape*>(shape);
                    if(!polygon){
                        LOGE("dynamic_cast failed");
                        continue;
                    }
                    float radius = polygon->m_radius;
                    mesh = mScene->createCircle(radius);
                    mBodyPolygonMap[mesh] = body;
                    LOGI("Circle created %p", mesh);
                }
                break;
            default:
                valid = false;
                break;
            }
            if(valid){
                /*
                 * set initial body mesh position from Box2d body
                 */
                ANode loc = mesh->getLocation();
                LOGI("GetPosition");
                b2Vec2 pos = body->GetPosition();
                LOGI("setLocation");
                loc->setLocation(pos.x, pos.y, 0);
                LOGI("location set");
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
        b2Body * body = (*it).second;
        b2Vec2 pos = body->GetPosition();
        float angle = body->GetAngle();
        //LOGI("Body location %f %f", pos.x, pos.y);
        ANode loc = (*it).first->getLocation();
        loc->setLocation(pos.x, pos.y, 0);
        loc->setEulerRotation(0, 0, toDegrees(-angle));
    }
}
