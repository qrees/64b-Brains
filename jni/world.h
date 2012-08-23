/*
 * world.h
 *
 *  Created on: 08-07-2012
 *      Author: Krzysztof
 */

#ifndef WORLD_H_
#define WORLD_H_

#include "Box2D/Box2D.h"
#include "smart_ptr.h"
#include "scene.h"

class EventListener {
public:
    virtual ~EventListener() { };
    virtual void onSensor(float x, float y, float z) = 0;
};


class BaseWorld:public RefCntObject, public EventListener {
public:
    virtual ~BaseWorld() { };
    virtual void tick() = 0;
    virtual AScene initScene(int w, int h) = 0;
    virtual void detachScene() = 0;
};
typedef AutoPtr<BaseWorld> ABaseWorld;


#endif /* WORLD_H_ */
