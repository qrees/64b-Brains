/*
 * world.h
 *
 *  Created on: 08-07-2012
 *      Author: Krzysztof
 */

#ifndef WORLD_H_
#define WORLD_H_

#include "Box2D/Box2D.h"
#include "boxscene.h"

class BaseWorld:public RefCntObject {
public:
    virtual ~BaseWorld() { };
    virtual void tick() = 0;
    virtual AScene initScene(int w, int h) = 0;
    virtual void detachScene() = 0;
};
typedef AutoPtr<BaseWorld> ABaseWorld;

class BoxWorld: public BaseWorld {
private:
    b2World* mWorld;
    float32 mTimeStep;
    int32 mVelocityIterations;
    int32 mPositionIterations;
    ABoxScene mScene;

    //b2BodyDef mBodyDef;
    //b2PolygonShape mBoxShape;
    //b2FixtureDef mFixtureDef;
    map<Mesh*, b2Body*> mBodyPolygonMap;
public:
    BoxWorld();
    ~BoxWorld();
    void init();
    void edge(float sx, float sy, float ex, float ey);
    AScene initScene(int w, int h);
    void detachScene();
    void tick();
};

#endif /* WORLD_H_ */
