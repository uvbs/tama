#pragma once

#ifdef _XLIB_BUILD
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void _NSLog( const char *str, ... );
#define	XLOGXN			_NSLog

#endif // xlib_build


#ifdef ANDROID
#include <android/log.h>
#ifndef XLOGXN
#define  XLOGXN(...)  __android_log_print(ANDROID_LOG_DEBUG,"xuzhu",__VA_ARGS__)
#endif

#endif // android

#ifndef XBREAK
#define XBREAK(EXP)	(EXP)? (XLOGXN(#EXP),1) : 0
#endif
