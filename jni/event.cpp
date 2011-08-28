/*
 * event.cpp
 *
 *  Created on: 2011-06-10
 *      Author: qrees
 */

#include "engine.h"

/** Event class. 
 * 
 * Holds data about events like clicks, drags etc. Events are
 * oranised into queue in the Scene class.
 */
Event::Event(){
    
}

Event::~Event(){
    
}

ClickEvent::ClickEvent(int x, int y){
    _x = x;
    _y = y;
}

void ClickEvent::process(Scene &scene){
    scene.down(_x, _y);
}

UpEvent::UpEvent(int x, int y){
    _x = x;
    _y = y;
}

void UpEvent::process(Scene &scene){
    scene.up(_x, _y);
}

MoveEvent::MoveEvent(int x, int y){
    _x = x;
    _y = y;
}

void MoveEvent::process(Scene &scene){
    scene.move(_x, _y);
}

bool running;
FinishEvent::FinishEvent(){
    
}

void FinishEvent::process(Scene &scene){
    running = false;
}

