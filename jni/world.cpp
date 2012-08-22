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
    mBodyDef.position.Set(0, 8); //middle, bottom

    polygonShape.SetAsBox(0.5f, 0.5f);

    mFixtureDef.shape = &polygonShape;
    mFixtureDef.density = 1.0f;
    mFixtureDef.friction = 0.3f;
    mFixtureDef.restitution = 0.4f;

    b2Body* body = mWorld->CreateBody(&mBodyDef);
    body->CreateFixture(&mFixtureDef);

    edge(-10, -10, 10, -10);
    edge(-10, 10, 10, 10);
    edge(-10, -10, -10, 10);
    edge(10, -10, 10, 10);
}

void BoxWorld::edge(float sx, float sy, float ex, float ey){
    b2FixtureDef mFixtureDef;
    b2PolygonShape polygonShape;
    b2BodyDef mBodyDef;

    mFixtureDef.density = 1.0f;
    mFixtureDef.friction = 0.3f;
    mFixtureDef.restitution = 0.4f;

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

                    /*
                     * set initial body mesh position from Box2d body
                     */
                    ANode loc = mesh->getLocation();
                    b2Vec2 pos = body->GetPosition();
                    loc->setLocation(pos.x, pos.y, 0);
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
        //LOGI("Body location %f %f", pos.x, pos.y);
        ANode loc = (*it).first->getLocation();
        loc->setLocation(pos.x, pos.y, 0);
    }
}
