/*
 * program.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

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
    void bindAttribute(GLuint location, GLuint size, GLenum type, GLuint stride, const void *data);
    void bindBuffer(GLuint location, GLuint buf_id, GLuint size, GLenum type, GLuint stride, const void * offset);
    void bindColor(const void *data);
    void bindColorRGB(const void *data);
    void bindPosition(const void *data);
    void activateAttribute(const char *name, GLuint location);
    void activateUniform(const char *name, GLuint location);
    void bindPosition(GLuint buf_id);
    void bindNormal(GLuint buf_id);
    void bindTexture(GLuint buf_id, GLuint tex_id);
    void bindColor(GLuint buf_id);
    void activateAttributes();
    void activateColor();
    void activatePosition();
    void activateTexture();
    void activateTextureSampler();
    void activate();
private:
    GLuint _link();

    GLuint _create();
    AShader _vertex;
    AShader _fragment;
    GLuint _id;
    GLuint attribs[8];
    GLuint uniforms[8];
    char * _log;
};
typedef AutoPtr<Program> AProgram;

#endif /* PROGRAM_H_ */
