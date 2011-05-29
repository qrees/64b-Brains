/*
 * framebuffer.cpp
 *
 *  Created on: 2011-05-29
 *      Author: qrees
 */

#include "engine.h"

Framebuffer::Framebuffer(){
    GLuint ids[3];
    glGenRenderbuffers(3, ids);
    _color_id = ids[0];
    _depth_id = ids[1];
    _stencil_id = ids[2];
    glGenFramebuffers(1, &_id);
}

Framebuffer::~Framebuffer(){
    glDeleteRenderbuffers(1, &_color_id);
    glDeleteRenderbuffers(1, &_depth_id);
    glDeleteRenderbuffers(1, &_stencil_id);
    glDeleteFramebuffers(1, &_id);
}

void Framebuffer::setFormat(GLuint width, GLuint height){
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glEnable(GL_DEPTH_TEST);

    glBindRenderbuffer(GL_RENDERBUFFER, _color_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, width, height);
    checkGlError("glRenderbufferStorage color");
    
    //glBindRenderbuffer(GL_RENDERBUFFER, _depth_id);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    //checkGlError("glRenderbufferStorage depth");
    
    glBindRenderbuffer(GL_RENDERBUFFER, _stencil_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, width, height);
    checkGlError("glRenderbufferStorage stencil");
    

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, _color_id);
    checkGlError("glFramebufferRenderbuffer");
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, _stencil_id);
    checkGlError("glFramebufferRenderbuffer");
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, _stencil_id);
    checkGlError("glFramebufferRenderbuffer");
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    checkGlError("glCheckFramebufferStatus");
    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            LOGI("Framebuffer status is ready");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOGI("Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
            return;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGI("Framebuffer status is GL_FRAMEBUFFER_MISSING_ATTACHMENT");
            return;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            LOGI("Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
            return;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOGI("Framebuffer status is GL_FRAMEBUFFER_UNSUPPORTED");
            return;
            break;
        default:
            LOGE("Unknown Framebuffer status %d", status);
            return;
            break;
    }
}
