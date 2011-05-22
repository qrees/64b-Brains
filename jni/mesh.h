/*
 * mesh.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef MESH_H_
#define MESH_H_

/*
 * BaseMesh
 * 
 * Base class for all objects in the scene. 
 */
class BaseMesh:public RefCntObject {
public:
    virtual ~BaseMesh(){};
    virtual void draw(){};
};
typedef AutoPtr<BaseMesh> ABaseMesh;

/*
 * Mesh
 * 
 * Base class meshes that have vertices, textures, colors etc. 
 */
#define BUF_COUNT   5
#define VERTEX_BUF  0
#define NORMAL_BUF  1
#define TEXTURE_BUF 2
#define INDEX_BUF   3
#define COLOR_BUF   4

class Mesh: public BaseMesh {
    GLuint vboIds[BUF_COUNT];
    AProgram program;
    ATexture _texture;
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
    void setTexture(ATexture tex);
    void setColor(GLfloat *buf, GLint num);
    void setIndexes(GLushort *buf, GLint num);
    virtual void draw();
private:
    void _setBuffer(GLenum target, GLfloat *buf, GLuint size, GLuint sel);
};
typedef AutoPtr<Mesh> AMesh;

class Group: public BaseMesh {
private:
    list<AMesh> _objects;
public:
    void addObject(AMesh);
    void draw();
};


/*
 * Rectangle:
 * Simple 3D Quad
 */
class Rectangle:public Mesh {
private:
    
public:
    Rectangle();
};
typedef AutoPtr<Rectangle> ARectangle;


#endif /* MESH_H_ */
