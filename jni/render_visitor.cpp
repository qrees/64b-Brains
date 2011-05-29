/*
 * render_visitor.cpp
 *
 *  Created on: 2011-05-29
 *      Author: qrees
 */

#include "engine.h"

void RenderVisitor::visit(AEntity){
    
};

void RenderVisitor::visit(AMesh){
    
};

AProgram HitVisitor::getHitProgram(){
    return _hit_program;
}

void HitVisitor::setHitProgram(AProgram program){
    _hit_program = program;
}
