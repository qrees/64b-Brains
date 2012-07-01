/*
 * animation.cpp
 *
 *  Created on: 23-06-2012
 *      Author: Krzysztof
 */

#include "engine.h"

Animation::Animation(){

}

Animation::~Animation(){

}
Animation::Animation(unsigned int length){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    _start = tv.tv_sec*1000000 + tv.tv_usec;
    _deadline = _start + length;
    _delta = length;
}

bool Animation::finished(unsigned int time){
    return time > _deadline;
}

EntityAnimation::EntityAnimation(ANode node, ANode start_location, ANode end_location, unsigned int length):Animation(length){
    _node = node;
    _start_value = start_location;
    _end_value = end_location;
}

EntityAnimation::EntityAnimation(ANode node, ANode end_location, unsigned int length):Animation(length){
    _node = node;
    _start_value = new Node("animation tmp");
    _start_value->setFrom(node);
    _end_value = end_location;
}

bool EntityAnimation::process(unsigned int time){
    float alpha;
    _node->setFrom(_start_value);

    alpha = float(time - _start) / _delta;
    alpha = min(1.0, alpha);
    _node->setEulerRotation(0, 0, float(360) * alpha);
    //_node->blend(_end_value, alpha);
    return finished(time);
}

