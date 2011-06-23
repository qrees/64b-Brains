/*
 * texture.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#define ETC_PKM_HEADER_SIZE 16
#define ETC1_PKM_WIDTH_OFFSET 12
#define ETC1_PKM_HEIGHT_OFFSET 14

typedef unsigned char etc1_byte;

class Texture:public RefCntObject {
private:
    void _init();
    GLuint _id;
    u_int _width, _height;
    u_char * _data;
public:
    Texture();
    ~Texture();
    GLuint getName();
    void empty(GLuint width, GLuint height);
    void load(GLuint width, GLuint height, int format, u_char * data);
    void load_pkm(const u_char * data);
    void load_compressed(GLuint width, GLuint height, const u_char * data);
    GLuint getWidth();
    GLuint getHeight();
    virtual void loaded();
};
typedef AutoPtr<Texture> ATexture; 

u_int etc1_pkm_get_width(const u_char* pHeader);
u_int etc1_pkm_get_height(const u_char* pHeader);
u_int etc1_get_encoded_data_size(u_int width, u_int height);

#endif /* TEXTURE_H_ */
