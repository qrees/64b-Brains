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
    scene.click(_x, _y);
}
