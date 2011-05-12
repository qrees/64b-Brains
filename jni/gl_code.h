/*
 * gl_code.h
 *
 *  Created on: 2011-05-12
 *      Author: qrees
 */

#ifndef GL_CODE_H_
#define GL_CODE_H_
#include <string>
using namespace std;

class RefCntObject {
public:
    virtual ~RefCntObject() {
    }

    RefCntObject() :
        m_refcnt(0) {
    }

    void addRef() {
        m_refcnt++;
    }

    bool subRef() {
        return (--m_refcnt <= 0);
    }

private:
    int m_refcnt;
};

template<class T>
class AutoPtr {
public:
    AutoPtr(T* ptr = 0) :
        m_ptr(ptr) {
        addRef();
    }

    AutoPtr(const AutoPtr& p) :
        m_ptr(p.m_ptr) {
        addRef();
    }

    ~AutoPtr() {
        subRef();
    }

    AutoPtr& operator=(const AutoPtr& p) {
        return *this = p.m_ptr;
    }

    AutoPtr& operator=(T* ptr) {
        if (m_ptr != ptr) {
            subRef();
            m_ptr = ptr;
            addRef();
        }
        return *this;
    }

    T& operator *() const {
        return *m_ptr;
    }

    T* operator ->() const {
        return m_ptr;
    }

    operator T*() const {
        return m_ptr;
    }
    operator const T*() const {
        return m_ptr;
    }

    operator bool() const {
        return m_ptr != 0;
    }

    T** operator &() {
        return &m_ptr;
    }

private:
    void addRef() {
        if (m_ptr)
            m_ptr->addRef();
    }

    void subRef() {
        if (m_ptr) {
            if (m_ptr->subRef()) {
                delete m_ptr;
                m_ptr = 0;
            }
        }
    }

    T* m_ptr;
};

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

class Program: public RefCntObject {
public:
    Program(AShader vertexShader, AShader fragmentShader);
    Program();
    ~Program();
    void make(AShader vertexShader, AShader fragmentShader);
    char * getInfo();
    bool isValid(){return _id != 0;};
    GLuint getName(){return _id;};
private:
    GLuint _link();

    GLuint _create();
    AShader _vertex;
    AShader _fragment;
    GLuint _id;
    char * _log;
};

#endif /* GL_CODE_H_ */
