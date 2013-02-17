/*
 * common.h
 *
 *  Created on: 2011-06-06
 *      Author: qrees
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "sys/types.h"
#include <math.h>
#include <string.h>

using namespace std;

struct cmp_str
{
   bool operator()(char const *a, char const *b) const {
      return strcmp(a, b) < 0;
   }
};

#define toDegrees(angle) (angle * 180.0f / M_PI)
#define toRadians(angle) (angle * M_PI / 180.0f)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define null 0

#define ACTION_DOWN 0
#define ACTION_MOVE 2
#define ACTION_UP   1

#define ALPHA_8   0
#define RGB_565   1 
#define ARGB_4444 2
#define ARGB_8888 3

#define TEXTURE_HEADER 12


#endif /* COMMON_H_ */
