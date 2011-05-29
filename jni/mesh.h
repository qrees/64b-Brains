/*
 * mesh.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef MESH_H_
#define MESH_H_


class Node;
typedef AutoPtr<Node> ANode;
class RenderVisitor;
typedef AutoPtr<RenderVisitor> ARenderVisitor;

class Node:public RefCntObject {
private:
    ANode _parent;
    GLMatrix _matrix;
    string _name;
    GLMatrix _local_matrix;
public:
    enum TransformSpace {
        /// Transform is relative to the space of the parent node
        TS_PARENT,
        /// Transform is relative to world space
        TS_WORLD
    };
    Node();
    ~Node();
    void addChild(ANode node);
    void setOrientation(GLMatrix matrix);
    void setParent(ANode);
    void getMatrix();
};

/*!
 * Entity
 * 
 * Base class for all objects in the scene. 
 */
class Entity:public RefCntObject {
public:
    virtual ~Entity(){};
    virtual void draw(){};
    virtual void _draw_hit_check(ARenderVisitor){};
};
typedef AutoPtr<Entity> AEntity;

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

#define RED_BYTE   0xFF000000
#define GREEN_BYTE 0x00FF0000
#define BLUE_BYTE  0x0000FF00
#define ALPHA_BYTE 0x000000FF


/*!
 * Mesh
 * 
 * Entity that has vertices, may have colors, texture etc.
 * Requires attaching shader program. 
 */
class Mesh: public Entity {
    GLuint vboIds[BUF_COUNT];
    AProgram _program;
    ATexture _texture;
    GLuint numIndices;
    GLfloat * _solid_color;
    GLfloat * _hit_color;
    static GLint _hit_color_seq;
    bool has_color, has_normal, has_texture, _hitable;
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
    void setHitable(bool );
    void draw();
    void _draw_hit_check(ARenderVisitor);
private:
    void _setBuffer(GLenum target, GLfloat *buf, GLuint size, GLuint sel);
};
typedef AutoPtr<Mesh> AMesh;


/*!
 * Group
 * 
 * Groups multiple Entities into one object. Can be used
 * for example as a root object in the Scene.
 */
class Group: public Entity {
private:
    list<AMesh> _objects;
public:
    void addObject(AMesh);
    void draw();
    void _draw_hit_check(ARenderVisitor);
};

/*!
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
