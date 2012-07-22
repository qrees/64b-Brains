/*
 * framebuffer.h
 *
 *  Created on: 2011-05-29
 *      Author: qrees
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "common.h"
#include "smart_ptr.h"
#include "texture.h"

class Framebuffer: public RefCntObject{
public:
    Framebuffer();
    ~Framebuffer();
    void setFormat(GLuint width, GLuint height);
    void setColorBuffer(GLuint with, GLuint height);
    void setDepthStencilBuffer(GLuint with, GLuint height);
    void setColorTextureBuffer(ATexture texture);
    virtual void activate();
    virtual bool isValid();
private:
    GLuint _color_id;
    GLuint _depth_id;
    GLuint _stencil_id;
    GLuint _id;
    GLuint _width;
    GLuint _height;
};

class ScreenBuffer : public Framebuffer{
public:
    void activate(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    };
    bool isValid(){return true;}; ///< Screen buffer is always vliad.
};

typedef AutoPtr<Framebuffer> AFramebuffer;

#endif /* FRAMEBUFFER_H_ */
