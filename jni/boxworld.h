/*
 * boxworld.h
 *
 *  Created on: 23-08-2012
 *      Author: Krzysztof
 */

#ifndef BOXWORLD_H_
#define BOXWORLD_H_

#include "world.h"
#include "boxscene.h"

class BoxWorld: public BaseWorld {
private:
    b2World* mWorld;
    float32 mTimeStep;
    int32 mVelocityIterations;
    int32 mPositionIterations;
    ABoxScene mScene;
    map<Mesh*, b2Body*> mBodyPolygonMap;
public:
    BoxWorld();
    ~BoxWorld();
    void init();
    void edge(float sx, float sy, float ex, float ey);
    AScene initScene(int w, int h);
    void detachScene();
    void tick();
    void onSensor(float x, float y, float z);
};

#endif /* BOXWORLD_H_ */
