/*
 * shader.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef SHADER_H_
#define SHADER_H_

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


#endif /* SHADER_H_ */
