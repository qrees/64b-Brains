/*
 * font.h
 *
 *  Created on: 2011-06-24
 *      Author: qrees
 */

#ifndef FONT_H_
#define FONT_H_
/*
 * Fixes for Boost library
 * 
namespace std {
    struct bad_alloc : public exception { bad_alloc operator()(){}};
    struct bad_cast : public exception {bad_cast operator()(){}};
}

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
 */

#include "sys/types.h"
#include "smart_ptr.h"
#include "texture.h"
#include <vector>
#include <map>

using namespace std;

/**
 * File data structures need to be packed, so attributes are properly matched.
 */
#define FILE_DATA  __attribute__ ((__packed__)) 

struct FILE_DATA FontHeader {
    char ident1; // B
    char ident2; // M 
    char ident3; // F
    uint8_t version;
};

struct FILE_DATA BlockHeader{
    uint8_t block_type;
    uint32_t block_size;
};

struct FILE_DATA FontInfo {
    uint16_t font_size;
    uint8_t  bit_field;
    uint8_t  char_set;
    uint16_t strech_h;
    uint8_t  aa;
    uint8_t  padding_up;
    uint8_t  padding_right;
    uint8_t  padding_down;
    uint8_t  padding_left;
    uint8_t  spacing_horiz;
    uint8_t  spacing_vert;
    uint8_t  outline;
    char     font_name;
};

struct FILE_DATA FontCommon {
    uint16_t line_height;
    uint16_t base;
    uint16_t scale_w;
    uint16_t scale_h;
    uint16_t pages;
    uint8_t  bit_field;
    uint8_t  alpha_chnl;
    uint8_t  red_chnl;
    uint8_t  green_chnl;
    uint8_t  blue_chnl;
};

#define _FONTCHAR \
    uint32_t id; \
    uint16_t x; \
    uint16_t y; \
    uint16_t width; \
    uint16_t height; \
    uint16_t xoffset; \
    uint16_t yoffset; \
    uint16_t xadvance; \
    uint8_t  page; \
    uint8_t  channel;

struct FILE_DATA FFontChar { _FONTCHAR };
class FontChar:public RefCntObject {
public:
    _FONTCHAR
};
typedef AutoPtr<FontChar> AFontChar;

/**
 * Font class
 * 
 * Describes bitmap font in AngelCode format. Can be used
 * by TextArea class to draw text on screen. Font pixels
 * are stored in OpenGL texture.
 */
class Font: public RefCntObject{
public:
    Font(const char * source);
    ~Font();
    
    /**
     * Load font data from binary file *source in angelcode Bitmap
     * Font format. Images for the font are searched relatively 
     * to *source location.
     */
    void load(const char * source);
    ATexture getTexture(uint32_t);
    uint16_t getPageWidth();
    uint16_t getPageHeight();
    AFontChar getCharData(uint32_t);
private:
    AFontChar _char_from_fchar(FFontChar*);
    const u_char * _data;
    uint _pages_count;
    vector<ATexture> _pages;
    map<uint32_t, AFontChar> _characters;
    
    uint16_t _line_height;
    uint16_t _base;
    uint16_t _scale_w;
    uint16_t _scale_h;
    uint8_t  _bit_field;
    uint8_t  _alpha_chnl;
    uint8_t  _red_chnl;
    uint8_t  _green_chnl;
    uint8_t  _blue_chnl;
};

typedef AutoPtr<Font> AFont;

#endif /* FONT_H_ */
