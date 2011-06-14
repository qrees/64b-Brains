/*
 * texture.cpp
 *
 *  Created on: 2011-05-17
 *      Author: qrees
 */

#include "engine.h"

void Texture::_init(){
    glGenTextures(1, &_id);
    checkGlError("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Texture::Texture(){
    _data = 0;
    _init();
}
Texture::~Texture(){
    delete[] _data;
    glDeleteTextures(1, &_id);
}

GLuint Texture::getName(){
    return _id;
}

void Texture::load(GLuint width, GLuint height, u_char * data){
    if(_data)
        delete[] _data;
    _data = new u_char[width*height*4];
    memcpy(_data, data, width*height*4);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data);
    checkGlError("glTexImage2D");
    loaded();
}

void Texture::empty(GLuint width, GLuint height){
    if(_data)
        delete[] _data;
    _data = new u_char[width*height*4];
    memset(_data, 0, width*height*4);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    checkGlError("glTexImage2D");
}

void Texture::load_pkm(const u_char * data){
    u_int width = 0;
    u_int height = 0;
    width = etc1_pkm_get_width(data);
    height = etc1_pkm_get_height(data);
    LOGI("Loading PKM image: %dx%d", width, height);
    load_compressed(width, height, data + ETC_PKM_HEADER_SIZE);
}

void Texture::load_compressed(GLuint width, GLuint height, const u_char * data){
    u_int encodedSize = 0;
    glBindTexture(GL_TEXTURE_2D, _id);
    _width = width;
    _height = height;
    encodedSize = etc1_get_encoded_data_size(width, height);
    if(_data)
        delete[] _data;
    _data = new u_char[encodedSize];
    memcpy(_data, data, encodedSize);
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, width, height, 0, encodedSize, _data);
    checkGlError("glCompressedTexImage2D");
}

void Texture::loaded(){
    //glGenerateMipmap(GL_TEXTURE_2D);
    checkGlError("glGenerateMipmap");
}

static u_int readBEUint16(const u_char* pIn) {
    return (pIn[0] << 8) | pIn[1];
}

// Read the image width from a PKM header

u_int etc1_pkm_get_width(const u_char* pHeader) {
    return readBEUint16(pHeader + ETC1_PKM_WIDTH_OFFSET);
}

// Read the image height from a PKM header

u_int etc1_pkm_get_height(const u_char* pHeader){
    return readBEUint16(pHeader + ETC1_PKM_HEIGHT_OFFSET);
}

// Return the size of the encoded image data (does not include size of PKM header).

u_int etc1_get_encoded_data_size(u_int width, u_int height) {
    return (((width + 3) & ~3) * ((height + 3) & ~3)) >> 1;
}
