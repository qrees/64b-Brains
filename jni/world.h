/*
 * world.h
 *
 *  Created on: 08-07-2012
 *      Author: Krzysztof
 */

#ifndef WORLD_H_
#define WORLD_H_

#include "Box2D/Dynamics/b2Body.h"
#include "boxscene.h"

class BaseWorld {
    virtual ~BaseWorld();
    virtual void tick();
    virtual AScene initScene();
    virtual void destroyScene();
};

class BoxWorld: public BaseWorld {
private:
    b2World* mWorld;
    float32 mTimeStep;
    int32 mVelocityIterations;
    int32 mPositionIterations;
    ABoxScene mScene;

    b2BodyDef mBodyDef;
    b2PolygonShape mBoxShape;
    b2FixtureDef mFixtureDef;
    b2Body* mBody;
public:
    BoxWorld();
    ~BoxWorld();
    void init();
    AScene initScene();
    void destroyScene();
    void tick();
};

#endif /* WORLD_H_ */
