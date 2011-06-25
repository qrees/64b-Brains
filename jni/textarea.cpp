/*
 * textarea.cpp
 *
 *  Created on: 2011-06-25
 *      Author: qrees
 */

#include "textarea.h"

TextArea::TextArea(){
    _dirty = false;
}

void TextArea::_update(){
    if(not _font){
        LOGE("TextArea: No font assigned to TextArea");
        return;
    }
    AFontChar character;
    GLuint tw = _font->getPageWidth();
    GLuint th = _font->getPageHeight();
    GLfloat* vertices = new GLfloat[_text.length()*3  * 6]; // 3 dimensions * 6 vertices per character
    GLfloat* tex_coord = new GLfloat[_text.length()*2 * 6]; // 2 dimensions * 6 vertices per character
    GLfloat* cur_vert = vertices;
    GLfloat* cur_tex = tex_coord;
    GLfloat cur_x=0;
    for(size_t i=0; i<_text.length(); i++){
        //ATexture texture = _font->getTexture(_text.at(i)); // XXX : to make it simple, text should not refer to multiple font textures
        character = _font->getCharData(_text.at(i));
        
        // First triangle
        *cur_vert++ = cur_x+character->xoffset;
        *cur_vert++ = -character->yoffset;
        *cur_vert++ = 0;
        *cur_tex++ = character->x/tw;
        *cur_tex++ = character->y/th;

        *cur_vert++ = cur_x+character->xoffset;
        *cur_vert++ = -character->yoffset - character->height;
        *cur_vert++ = 0;
        *cur_tex++ = character->x/tw;
        *cur_tex++ = (character->y+character->height)/th;
        
        *cur_vert++ = cur_x+character->xoffset + character->width;
        *cur_vert++ = -character->yoffset;
        *cur_vert++ = 0;
        *cur_tex++ = (character->x+character->width)/tw;
        *cur_tex++ = character->y/th;
        
        // Second triangle
        *cur_vert++ = cur_x+character->xoffset + character->width;
        *cur_vert++ = -character->yoffset;
        *cur_vert++ = 0;
        *cur_tex++ = (character->x+character->width)/tw;
        *cur_tex++ = character->y/th;

        *cur_vert++ = cur_x+character->xoffset;
        *cur_vert++ = -character->yoffset - character->height;
        *cur_vert++ = 0;
        *cur_tex++ = character->x/tw;
        *cur_tex++ = (character->y+character->height)/th;
        
        *cur_vert++ = cur_x+character->xoffset + character->width;
        *cur_vert++ = -character->yoffset - character->height;
        *cur_vert++ = 0;
        *cur_tex++ = (character->x+character->width)/tw;
        *cur_tex++ = (character->y+character->height)/th;
        cur_x+=character->xadvance;
    }
    delete[] tex_coord;
    delete[] vertices;
    _dirty = false;
}

void TextArea::setFont(AFont font){
    _font = font;
    _dirty = true;
}

void TextArea::setText(const char* text){
    setText(string(text));
}

void TextArea::setText(string text){
    _text = text;
    _dirty = true;
}


void TextArea::drawPrepare(ARenderVisitor visitor){
    if(_dirty){
        _update();
    }
}
