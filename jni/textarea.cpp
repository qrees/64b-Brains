/*
 * textarea.cpp
 *
 *  Created on: 2011-06-25
 *      Author: qrees
 */

#include "textarea.h"

TextArea::TextArea(){
    _dirty = true;
    _size = 1.0f;
}

void TextArea::_update(){
    if(not _font){
        LOGE("TextArea: No font assigned to TextArea");
        return;
    }
    GLfloat yoffset;
    AFontChar character;
    GLfloat tw = _font->getPageWidth();
    GLuint th = _font->getPageHeight();
    GLfloat* vertices  = new GLfloat [_text.length()*3  * 6]; // 3 dimensions * 6 vertices per character
    GLfloat* tex_coord = new GLfloat [_text.length()*2  * 6]; // 2 dimensions * 6 vertices per character
    GLushort*indexes   = new GLushort[_text.length()    * 6];
    GLushort*cur_index = indexes;
    uint index = 0;
    GLfloat* cur_vert = vertices;
    GLfloat* cur_tex = tex_coord;
    GLfloat cur_x=0;
    GLfloat cur_y=0;
    if(_text.length() > 0)
        setTexture(_font->getTexture(_text.at(0)));
    else
        LOGE("Not setting texture for text, because text is empty");
    for(size_t i=0; i<_text.length(); i++){
        //ATexture texture = _font->getTexture(_text.at(i)); // XXX : to make it simple, text should not refer to multiple font textures
        if(_text.at(i) == 10){
            cur_x = 0;
            cur_y -= _font->getLineHeight();
            continue;
        }
        character = _font->getCharData(_text.at(i));
        /*LOGI("Drawing character %c %i\toffset:%i %i\tpos:%i %i\tsize: %i %i",
                character->id, character->id,
                character->xoffset, character->yoffset,
                character->x, character->y,
                character->width, character->height);*/
        // First triangle
        yoffset = _font->getLineHeight() - character->yoffset - character->height;
        *cur_index++ = index++; // =0
        *cur_vert++ = cur_x+character->xoffset;
        *cur_vert++ = cur_y + yoffset;
        *cur_vert++ = 0;
        *cur_tex++ = float(character->x)/tw;
        *cur_tex++ = float(character->y+character->height)/th;

        *cur_index++ = index++; // =1
        *cur_vert++ = cur_x + character->xoffset;
        *cur_vert++ = cur_y + yoffset + character->height ;
        *cur_vert++ = 0;
        *cur_tex++ = float(character->x)/tw;
        *cur_tex++ = float(character->y)/th;

        *cur_index++ = index; // =2
        *cur_vert++ = cur_x+character->xoffset + character->width;
        *cur_vert++ = cur_y + yoffset;
        *cur_vert++ = 0;
        *cur_tex++ = float(character->x+character->width)/tw;
        *cur_tex++ = float(character->y+character->height)/th;
        
        // Second triangle
        *cur_index++ = index; // =2

        *cur_index++ = index-1; // =1

        *cur_index++ = ++index; // =3
        *cur_vert++ = cur_x+character->xoffset + character->width;
        *cur_vert++ = cur_y + yoffset + character->height;
        *cur_vert++ = 0;
        *cur_tex++ = float(character->x+character->width)/tw;
        *cur_tex++ = float(character->y)/th;
        cur_x+=character->xadvance;
        index++;
    }
    setVertices(vertices, (index));
    setTextureCoord(tex_coord, (index));
    setIndexes(indexes, (index)/4 * 6);
    setType(GL_TRIANGLES);
    delete[] tex_coord;
    delete[] vertices;
    delete[] indexes;
    _dirty = false;
}

void TextArea::setFont(AFont font){
    _font = font;
    _dirty = true;
}
/*
void TextArea::setText(const char* text){
    setText(string(text));
}
*/
void TextArea::setText(const char* format, ... ){
    char *buffer;
    char dummy;
    va_list args;
    va_start (args, format);
    int count = vsnprintf(&dummy, 0, format, args);
    buffer = new char[count];
    vsnprintf(buffer, count, format, args);
    setText(string(buffer));
    delete buffer;
    va_end (args);
}

void TextArea::setText(string text){
    _text = text;
    _dirty = true;
}

void TextArea::setSize(GLfloat size){
    _size = size;
}

void TextArea::drawPrepare(ARenderVisitor visitor){
    if(_dirty){
        _update();
        _location->setScale(1.f/float(_font->getLineHeight())*_size, 1.f/float(_font->getLineHeight())*_size, 1.0f);
    }else{
    }
}
