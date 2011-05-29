/*
 * my_jni.h
 *
 *  Created on: 2011-05-22
 *      Author: qrees
 */

#ifndef MY_JNI_H_
#define MY_JNI_H_

#include <jni.h>

jint throwJNI(const char *message);
char * load_asset(const char * source);
u_char * load_raw(const char * source);
u_char * load_bitmap(const char * source);

#endif /* MY_JNI_H_ */
