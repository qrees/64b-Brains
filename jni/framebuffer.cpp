/*
 * framebuffer.cpp
 *
 *  Created on: 2011-05-29
 *      Author: qrees
 */

//#include "engine.h"


#include "log.h"
#include "framebuffer.h"
#include <GLES2/gl2ext.h>

Framebuffer::Framebuffer(){
    _stencil_id = 0;
    _color_id = 0;
    _depth_id = 0;
    glGenFramebuffers(1, &_id);
    checkGlError("glGenFramebuffers");
}

Framebuffer::~Framebuffer(){
    glDeleteRenderbuffers(1, &_color_id);
    glDeleteRenderbuffers(1, &_depth_id);
    glDeleteRenderbuffers(1, &_stencil_id);
    glDeleteFramebuffers(1, &_id);
    checkGlError("glDeleteFramebuffers");
}

void Framebuffer::setDepthStencilBuffer(GLuint width, GLuint height){
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    if(_stencil_id)
        glDeleteRenderbuffers(1, &_stencil_id);
    if(_depth_id)
        glDeleteRenderbuffers(1, &_depth_id);
    
    glGenRenderbuffers(1, &_stencil_id);
    glBindRenderbuffer(GL_RENDERBUFFER, _stencil_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
    checkGlError("glRenderbufferStorage stencil");

    glGenRenderbuffers(1, &_depth_id);
    glBindRenderbuffer(GL_RENDERBUFFER, _depth_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    checkGlError("glRenderbufferStorage stencil");

    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, width, height);


    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, _depth_id);
    checkGlError("glFramebufferRenderbuffer");
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
    //                          GL_RENDERBUFFER, _stencil_id);
    checkGlError("glFramebufferRenderbuffer");
}

void Framebuffer::setColorBuffer(GLuint width, GLuint height){
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    if(_color_id)
        glDeleteRenderbuffers(1, &_color_id);
    
    glGenRenderbuffers(1, &_color_id);
    glBindRenderbuffer(GL_RENDERBUFFER, _color_id);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, width, height);
    checkGlError("glRenderbufferStorage color");

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, _color_id);
    checkGlError("glFramebufferRenderbuffer");
    LOGI("Created color buffer");
}

void Framebuffer::setColorTextureBuffer(ATexture texture){
    if(_color_id)
        glDeleteRenderbuffers(1, &_color_id);

    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getName(), 0);
    checkGlError("glFramebufferTexture2D");
}

void Framebuffer::activate(){
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

bool Framebuffer::isValid(){
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    checkGlError("glCheckFramebufferStatus");
    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            //LOGI("Framebuffer status is ready");
            return true;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOGI("Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGI("Framebuffer status is GL_FRAMEBUFFER_MISSING_ATTACHMENT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            LOGI("Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOGI("Framebuffer status is GL_FRAMEBUFFER_UNSUPPORTED");
            break;
        default:
            LOGE("Unknown Framebuffer status %d", status);
            break;
    }
    return false;
}

void Framebuffer::setFormat(GLuint width, GLuint height){
    _width = width;
    _height = height;
    setDepthStencilBuffer(width, height);
    setColorBuffer(width, height);

}


