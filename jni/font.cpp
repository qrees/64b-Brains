/*
 * font.cpp
 *
 *  Created on: 2011-06-24
 *      Author: qrees
 */


/*
#include <exception>

namespace boost
{
    void throw_exception(std::exception const&) {}
}
namespace std
{
    exception::exception() {}
    exception::~exception() {}
    const char* exception::what() const {return "";}
}
*/
#include <string.h>
#include <string>
#include <libgen.h>
#include <stdlib.h>

#include "my_jni.h"
#include "font.h"
#include "log.h"

Font::Font(const char * source){
    _data = NULL;
    load(source);
};

Font::~Font(){
    delete[] _data;
}

void Font::load(const char * source){
    u_char * current;
    size_t file_size;
    u_char * page_start;
    char * font_name;
    char * page_name;
    size_t char_count;
    ATexture page_texture;
    FontHeader *header;
    string path;
    char * base = basename(source);
    char * dir = dirname(source);
    
    LOGI("Loading font from %s, basename %s, dirname %s", source, base, dir);
    if(_data)
        delete[] _data;
    const u_char * data = load_raw(source, &file_size);
    _data = data;
    current = (u_char*)data;
    header = (FontHeader*)data;
    current+=sizeof(FontHeader);
    LOGI("Read font file version %i", header->version);
    
    BlockHeader *block_header;
    while(((size_t)(current-data)) < file_size){
        block_header = (BlockHeader*)current;
        current+=sizeof(BlockHeader);
        switch(block_header->block_type){
            case 1: // FontInfo block
                FontInfo *info;
                info = (FontInfo*)current;
                LOGI("Font Info block, font size: %hu", info->font_size);
                font_name = &(info->font_name);
                LOGI("Font name: %s", (current+14));
                break;
            case 2: // FontCommon block, common values for characters
                FontCommon *common;
                common = (FontCommon*)current;
                LOGI("Font Common block, line height %i, pages %i, scale h %i, scale w %i", common->line_height, common->pages, common->scale_h, common->scale_w);

                _line_height = common->line_height;
                _base = common->base;
                _scale_w = common->scale_w;
                _scale_h = common->scale_h;
                _pages_count = common->pages;
                _bit_field = common->bit_field;
                _alpha_chnl = common->alpha_chnl;
                _red_chnl = common->red_chnl;
                _green_chnl = common->green_chnl;
                _blue_chnl = common->blue_chnl;
                break;
            case 3:
                page_start = current;
                for(size_t i=0; i<_pages_count; i++){
                    page_name = (char*)page_start;
                    page_start += strlen(page_name)+1;
                    path = string(dir) + "/" + page_name;
                    page_texture = loadBitmap(path.c_str());
                    _pages.push_back(page_texture);
                    LOGI("Font page name: %s", path.c_str());
                };
                break;
            case 4:
                FFontChar* fcharacter;
                AFontChar character;
                u_char* char_start;
                char_start = current;
                char_count = block_header->block_size/sizeof(FFontChar);
                for(size_t i=0; i<char_count; i++){
                    fcharacter = (FFontChar*)char_start;
                    char_start += sizeof(FFontChar);
                    LOGI("Font character: %c", fcharacter->id);
                    character = _char_from_fchar(fcharacter);
                    _characters[character->id] = character;
                }
                break;
        }
        current+=block_header->block_size;
    }
}

AFontChar Font::getCharData(uint32_t id){
    if(_characters.count(id))
        return _characters[id];
    else{
        LOGE("Character %u not found", id);
        return _characters[0];
    }
}
ATexture Font::getTexture(uint32_t id){
    if(_characters.count(id))
        return _pages[_characters[id]->page];
    else{
        LOGE("Character %u not found", id);
        return _pages[0];
    }
}

uint16_t Font::getPageWidth(){
    return _scale_w;
}

uint16_t Font::getPageHeight(){
    return _scale_h;
}

AFontChar Font::_char_from_fchar(FFontChar* fcharacter){
    FontChar *character = new FontChar;
    AFontChar a_character = character;
    character->channel = fcharacter->channel;
    character->height = fcharacter->height;
    character->id = fcharacter->id;
    character->page = fcharacter->page;
    character->width = fcharacter->width;
    character->x = fcharacter->x;
    character->xadvance = fcharacter->xadvance;
    character->xoffset = fcharacter->xoffset;
    character->y = fcharacter->y;
    character->yoffset = fcharacter->yoffset;
    return a_character;
}
