/*
 * program.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <map>

#include "smart_ptr.h"
#include "matrix.h"
#include "shader.h"

#define LOC_TEXTURE_SAMPLER 0
#define LOC_POSITION 0
#define LOC_COLOR 1
#define LOC_NORMAL 3
#define LOC_TEXTURE 4
#define POSITION_ATTRIB attribs[LOC_POSITION]
#define COLOR_ATTRIB    attribs[LOC_COLOR]
#define NORMAL_ATTRIB   attribs[LOC_NORMAL]
#define TEXTURE_ATTRIB  attribs[LOC_TEXTURE]
class Program: public RefCntObject {
public:
    Program(AShader vertexShader, AShader fragmentShader);
    Program();
    ~Program();
    void make(AShader vertexShader, AShader fragmentShader);
    char * getInfo();
    bool isValid(){return _id != 0;};
    GLuint getName(){return _id;};
    /*
     * Bind OpenGL atribute with actual data
     */
    void bindAttribute(GLuint location, GLuint size, GLenum type, GLuint stride, const void *data);
    void bindBuffer(GLuint location, GLuint buf_id, GLuint size, GLenum type, GLuint stride, const void * offset);
    void bindColor(const void *data);
    void bindSolidColor(GLint*color);
    void bindSolidColor(GLfloat*color);
    void bindColorRGB(const void *data);
    void bindPosition(const void *data);
    void bindPosition(GLuint buf_id);
    void bindNormal(GLuint buf_id);
    void bindTexture(GLuint buf_id, GLuint tex_id);
    void bindColor(GLuint buf_id);
    void bindViewMatrix(GLMatrix &matrix);
    void bindModelMatrix(GLMatrix &matrix);
    void useColor(bool use_color);
    void uniform1i(const char* uniform_name, GLint value);
    /*
     * Atributes activation, link string attributes names with OpenGL names (GLuint).
     */
    void initUniform(const char* uniform_name);
    void initAttribute(const char *name);
    void activateAttributes();
    void activate();
private:
    map<const char*, GLint, cmp_str> mUniformMap;
    map<const char*, GLint, cmp_str> mAttributeMap;
    GLuint _link();

    GLuint _create();
    AShader _vertex;
    AShader _fragment;
    GLuint _id;
    //GLuint attribs[8];
    //GLuint uniforms[8];
    GLint _view_matrix;
    GLint _model_matrix;
    //GLint _u_solid_color;
    char * _log;
};
typedef AutoPtr<Program> AProgram;

#endif /* PROGRAM_H_ */
