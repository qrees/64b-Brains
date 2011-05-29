/*
 * log.h
 * 
 * Logging and debugging functions.
 * 
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef LOG_H_
#define LOG_H_

#include <android/log.h>

#define assert(cond, mess) if(!(cond)){LOGE("Failed: %s file %s[%d]: %s", #cond, __FILE__, __LINE__, mess);exit(0);}

#define  LOG_TAG    "Brains"
#define  LOGI(msg, ...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,msg"\n", ##__VA_ARGS__)
#define  LOGE(msg, ...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,msg"\n", ##__VA_ARGS__)

extern void printGLString(const char *name, GLenum s);
extern void checkGlError(const char* op);

#endif /* LOG_H_ */
