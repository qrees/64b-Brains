/**
 * mesh.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef MESH_H_
#define MESH_H_

#include <string>
#include <list>
#include <GLES2/gl2.h>

#include "log.h"
#include "smart_ptr.h"
#include "matrix.h"
#include "quaternion.h"
#include "program.h"
#include "texture.h"

class RenderVisitor;
typedef AutoPtr<RenderVisitor> ARenderVisitor;

class Node:public RefCntObject {
private:
    AutoPtr<Node> _parent;
    GLMatrix _matrix;           // Absolute transform matrix, valid if 
    string _name;
    GLMatrix _local_matrix;     // Relative transform matrix
    GLfloat _x, _y, _z;         // Relative transposition
    GLQuaternion _rotation;     // Relative rotation
    GLfloat _sx, _sy, _sz;      // Relative scale.
    bool _valid;
public:
    enum TransformSpace {
        /// Transform is relative to the space of the parent node
        TS_PARENT,
        /// Transform is relative to world space
        TS_WORLD
    };
    Node();
    Node(string name);
    void init();
    ~Node();
    void addChild(AutoPtr<Node> node);
    void setParent(AutoPtr<Node>);
    
    /**
     * Return parent of this node as set by setParent.
     */
    AutoPtr<Node> getParent();
    
    /**
     * Return set of the node as set during creation.
     */
    string getName();
    /**
     * Updates absolute orientation from relative orienation
     * and parent nodes. If there is no parent node,
     * absolute and relative orientation are the same.
     */
    void update();
    /**
     * Sets relative orientation to given matrix.
     */
    void setOrientation(GLMatrix matrix);
    void setLocation(GLfloat x, GLfloat y, GLfloat z);
    void setRotation(GLfloat x, GLfloat y, GLfloat z, GLfloat angle);
    void setEulerRotation(GLfloat yaw, GLfloat pitch, GLfloat roll);
    void setScale(GLfloat x, GLfloat y, GLfloat z);
    GLMatrix& getMatrix();
    GLMatrix& getLocalMatrix();
};

typedef AutoPtr<Node> ANode;

/**
 * Entity
 * 
 * Base class for all objects in the scene. 
 */
class Entity:public RefCntObject {
protected:
    ANode _location;
public:
    Entity();
    virtual ~Entity(){};
    virtual void draw(ARenderVisitor){};
    virtual void _draw_hit_check(ARenderVisitor){};
    virtual void setLocation(ANode location);
    virtual ANode getLocation();
    virtual Entity* getEntityForColor(GLubyte*);
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


/**
 * Mesh
 * 
 * Entity that has vertices, may have colors, texture etc.
 * Requires attaching shader program. 
 */
class Mesh: public Entity {
protected:
    GLuint vboIds[BUF_COUNT];
    AProgram _program;
    ATexture _texture;
    GLuint numIndices;
    GLfloat * _solid_color;
    GLfloat * _hit_color;
    GLint _hit_color_int;
    GLenum _type;
    static GLint _hit_color_seq;
    bool has_color, has_normal, has_texture, _hitable;
public:
    Mesh();
    ~Mesh();
    void init();
    
    /*
     * Shader program to be used by Mesh in "draw" method.
     */
    void setProgram(AProgram program);
    
    /**
     * Sets Mesh Vertices from C array. Data in organized in triples:
     *    (x, y, z)...
     */
    void setVertices(GLfloat *buf, GLint num);
    void setNormal(GLfloat *buf, GLint num);
    void setTextureCoord(GLfloat *buf, GLint num);
    void setTexture(ATexture tex);
    void setColor(GLfloat *buf, GLint num);
    void setIndexes(GLushort *buf, GLint num);
    
    /**
     * Set Mesh type to one of GL_POINTS, GL_LINES, GL_LINE_LOOP, GL_LINE_STRIP,
     * GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN.
     */
    void setType(GLenum);
    GLenum getType();
    /**
     * setHitable(true) will allow the mesh to receive touch events.
     */
    void setHitable(bool );
    
    /**
     * Draw the mesh on current framebuffer.
     * Requires setting at least vertices with setVertices earlier.
     */
    void draw(ARenderVisitor);
    
    /**
     * Called before actual mesh draw.
     */
    virtual void drawPrepare(ARenderVisitor);
    void _draw_hit_check(ARenderVisitor);
    Entity* getEntityForColor(GLubyte*);
    
    /**
     * Down event method. Called when entity is pressed down
     * on touchscreen.
     */
    virtual void down(GLfloat x, GLfloat y){LOGE("Called generic down method");};
    /**
     * Move event method. Called when finger is moved on 
     * touchscreen with this mesh pressed.
     */
    virtual void move(GLfloat x, GLfloat y){};
    /**
     * Up event method. Called when finger is lifted.
     */
    virtual void up(GLfloat x, GLfloat y){LOGE("Called generic up method");};
private:
    void _setBuffer(GLenum target, GLfloat *buf, GLuint size, GLuint sel);
};
typedef AutoPtr<Mesh> AMesh;


/**
 * Group
 * 
 * Groups multiple Entities into one object. Can be used
 * for example as a root object in the Scene.
 */
class Group: public Entity {
protected:
    list<AMesh> _objects;
public:
    void addObject(AMesh);
    void draw(ARenderVisitor);
    void _draw_hit_check(ARenderVisitor);
    Entity* getEntityForColor(GLubyte*);
};

/**
 * Rectangle:
 * Simple 3D Quad
 */
class Rectangle:public Mesh {
private:
    
public:
    //Rectangle();
    Rectangle(float aspect = 1.f);
    void setTextureRect(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
};
typedef AutoPtr<Rectangle> ARectangle;


class Button:public Rectangle {
protected:
    enum ButtonState {
        NORMAL=0,
        PRESSED=1,
        ACTIVE=2,
        DISABLED=3
    };
    int _state_count;
    bool _dirty;
    ButtonState _state;
    GLfloat _sx, _sy;
    GLfloat*_state_textures;
public:
    Button(GLfloat sx, GLfloat sy);
    Button();
    ~Button();
    void drawPrepare(ARenderVisitor);
    void setState(ButtonState);
    void setStateCount(int);
    void setStateTexture(int state, GLfloat x, GLfloat y);
    void setTextureSize(GLfloat sx, GLfloat xy);
    
    void down(GLfloat x, GLfloat y);
    void up(GLfloat x, GLfloat y);
};

#endif /* MESH_H_ */
