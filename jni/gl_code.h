/*
 * gl_code.h
 *
 *  Created on: 2011-05-12
 *      Author: qrees
 */

#ifndef GL_CODE_H_
#define GL_CODE_H_
#include <string>
#include <vector>
#include <list>


using namespace std;

#define  LOG_TAG    "Brains"
#define  LOGI(msg, ...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,msg"\n", ##__VA_ARGS__)
//#define  LOGI(msg)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,msg "\n")
#define  LOGE(msg, ...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,msg"\n", ##__VA_ARGS__)
//#define  LOGE(msg)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,msg "\n")
#define assert(cond, mess) if(!cond){LOGE("Failed: %s file %s[%d]: %s", #cond, __FILE__, __LINE__, mess);exit(0);}

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

    template<class OTHER>
    AutoPtr(const AutoPtr<OTHER>& other) {
        T* foo = other;
        m_ptr = (T*)(other.m_ptr);
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
/*
    T** operator &() {
        return &m_ptr;
    }
*/
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
    void bindPosition(GLuint buf_id);
    void bindNormal(GLuint buf_id);
    void bindTexture(GLuint buf_id);
    void bindColor(GLuint buf_id);
    void activateColor();
    void activatePosition();
    void activate();
private:
    GLuint _link();

    GLuint _create();
    AShader _vertex;
    AShader _fragment;
    GLuint _id;
    GLuint attribs[8];
    char * _log;
};
typedef AutoPtr<Program> AProgram;


#define BUF_COUNT   5
#define VERTEX_BUF  0
#define NORMAL_BUF  1
#define TEXTURE_BUF 2
#define INDEX_BUF   3
#define COLOR_BUF   4

class BaseMesh:public RefCntObject {
public:
    virtual ~BaseMesh(){};
    virtual void draw(){};
};
typedef AutoPtr<BaseMesh> AMesh;

class Mesh: public BaseMesh {
    GLuint vboIds[BUF_COUNT];
    //GLuint strides[4];
    //GLuint attrib[3];
    AProgram program;
    GLuint numIndices;
    bool has_color, has_normal, has_texture;
public:
    Mesh();
    ~Mesh();
    void init();
    void setProgram(AProgram program);
    void setVertices(GLfloat *buf, GLint num);
    void setNormal(GLfloat *buf, GLint num);
    void setTextureCoord(GLfloat *buf, GLint num);
    void setColor(GLfloat *buf, GLint num);
    void setIndexes(GLushort *buf, GLint num);
    virtual void draw();
private:
    void _setBuffer(GLenum target, GLfloat *buf, GLuint size, GLuint sel);
};

class Group: public BaseMesh {
private:
    list<AMesh> _objects;
public:
    void addObject(AMesh);
    void draw();
};

class Rectangle:public Mesh {
private:
    
public:
    Rectangle();
};
typedef AutoPtr<Rectangle> ARectangle;


class Scene: public RefCntObject {
    AMesh root;
    AProgram program;
};

#endif /* GL_CODE_H_ */
