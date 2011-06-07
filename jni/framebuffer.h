/*
 * framebuffer.h
 *
 *  Created on: 2011-05-29
 *      Author: qrees
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_


class Framebuffer: public RefCntObject{
public:
    Framebuffer();
    ~Framebuffer();
    void setFormat(GLuint width, GLuint height);
    void setColorBuffer(GLuint with, GLuint height);
    void setDepthStencilBuffer(GLuint with, GLuint height);
    void setColorTextureBuffer(ATexture texture);
    virtual void activate();
    bool isValid();
private:
    GLuint _color_id;
    GLuint _depth_id;
    GLuint _stencil_id;
    GLuint _id;
};

class ScreenBuffer : public Framebuffer{
public:
    void activate(){glBindFramebuffer(GL_FRAMEBUFFER, 0);};
    
};

typedef AutoPtr<Framebuffer> AFramebuffer;

#endif /* FRAMEBUFFER_H_ */
