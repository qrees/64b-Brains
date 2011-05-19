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

#include "engine.h"

jint throwJNI(const char *message);
char * load_asset(const char * source);
unsigned char * load_raw(const char * source);

class Scene: public RefCntObject {
    AMesh root;
    AProgram program;
};

#endif /* GL_CODE_H_ */
