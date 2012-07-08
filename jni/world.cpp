/*
 * world.cpp
 *
 *  Created on: 08-07-2012
 *      Author: Krzysztof
 */

#include "world.h"

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

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 4.0f);
    b2Body* body = mWorld->CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    body->CreateFixture(&fixtureDef);
}

BoxWorld::~BoxWorld() {
    delete mWorld;
}

void BoxWorld::tick() {
    mWorld->Step(mTimeStep, mVelocityIterations, mPositionIterations);
}
