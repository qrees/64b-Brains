/*
 * shader.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef SHADER_H_
#define SHADER_H_

#include <string>
#include <GLES2/gl2.h>

#include "smart_ptr.h"
using namespace std;

class Shader:public RefCntObject {
private:
    string _source;
    GLuint _id;
    GLenum _type;
    char * _log;
    bool valid;
public:
    Shader();
    ~Shader();
    //Shader(const Shader& copy);
    Shader(const char * source, GLenum shaderType);
    char * getInfo();
    bool isValid();
    GLuint getName();
    void load(const char*source, GLenum shaderType);
private:
    GLuint _compile();
};
typedef AutoPtr<Shader> AShader;

AShader loadShader(const char * source, GLuint type);

#endif /* SHADER_H_ */
