/*
 * render_visitor.h
 *
 *  Created on: 2011-05-29
 *      Author: qrees
 */

#ifndef RENDER_VISITOR_H_
#define RENDER_VISITOR_H_

class RenderVisitor : public RefCntObject{
public:
    void visit(AEntity);
    void visit(AMesh);
    AProgram getProgram();
    void setProgram(AProgram);
private:
    AProgram _program;
};
typedef AutoPtr<RenderVisitor> ARenderVisitor;

class HitVisitor : public RenderVisitor{
public:
    AProgram getHitProgram();
    void setHitProgram(AProgram);
private:
    AProgram _hit_program;
};

typedef AutoPtr<HitVisitor> AHitVisitor;

#endif /* RENDER_VISITOR_H_ */
