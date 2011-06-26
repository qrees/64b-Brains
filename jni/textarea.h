/*
 * textarea.h
 *
 *  Created on: 2011-06-25
 *      Author: qrees
 */

#ifndef TEXTAREA_H_
#define TEXTAREA_H_

#include "mesh.h"
#include "font.h"

class TextArea: public Mesh{
public:
    TextArea();
    void setFont(AFont);
    void setText(const char*);
    void setText(string);
    void setSize(GLfloat);
    void drawPrepare(ARenderVisitor);
private:
    void _update();
    AFont _font;
    string _text;
    GLfloat _size;
    bool _dirty;
};

typedef AutoPtr<TextArea> ATextArea;

#endif /* TEXTAREA_H_ */
