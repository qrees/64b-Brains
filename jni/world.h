/*
 * world.h
 *
 *  Created on: 08-07-2012
 *      Author: Krzysztof
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <Box2D/Dynamics/b2Body.h>

class BaseWorld {
    virtual ~BaseWorld();
    virtual void tick();
};

class BoxWorld: public BaseWorld {
private:
    b2World* mWorld;
    float32 mTimeStep;
    int32 mVelocityIterations;
    int32 mPositionIterations;
public:
    BoxWorld();
    ~BoxWorld();
    void init();
    void tick();
};

#endif /* WORLD_H_ */
