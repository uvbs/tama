#ifndef	RARE_H
#define	RARE_H

#include "RareConfig.h"

#ifndef RARE_API
#    ifndef RARE_STATIC
#        define RARE_API DLL_EXPORT
#    else 
#        define RARE_API
#    endif


#ifdef	UNICODE
//#error	Unicode not supported now
#endif
/*
#ifdef	_MSC_VER
//#	ifndef UNICODE
#		ifndef DEBUG
#			pragma comment(lib, "Rare.lib")
#		else
#			pragma comment(lib, "RareD.lib")
#		endif
//#	else
//#		error	Sorry.. No Unicode Support Now
//#	endif
#endif
*/
/*
#ifdef	WIN32
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <DSound.h>
#else
#	error	...
#endif
//*/

#endif//RARE_API

//----------------------------------------------------------------------------------------------------------------
// Rare Sound
//----------------------------------------------------------------------------------------------------------------

RARECONTEXT RARE_API Rare_CreateContext();
void RARE_API Rare_DestroyContext(RARECONTEXT context);
void RARE_API Rare_SetSoundNotify(RARECONTEXT context, RARENOTIFY* notify);

RARESOUND RARE_API Rare_OpenSound(RARECONTEXT context, char *filename, Rare_Open type = RARE_OPENSTREAM);
void RARE_API Rare_CloseSound(RARESOUND sound);

void RARE_API Rare_PlaySound(RARESOUND sound);
void RARE_API Rare_StopSound(RARESOUND sound);
void RARE_API Rare_ResetSound(RARESOUND sound);
void RARE_API Rare_PauseSound(RARESOUND sound);
BOOL RARE_API Rare_IsPlaySound(RARESOUND sound);

void RARE_API Rare_SetFrequency(RARESOUND sound, LONG frequency);
LONG RARE_API Rare_GetFrequency(RARESOUND sound);

void RARE_API Rare_SetPan(RARESOUND sound, LONG pan);
LONG RARE_API Rare_GetPan(RARESOUND sound);

void RARE_API Rare_SetVolume(RARESOUND sound, LONG volume);
LONG RARE_API Rare_GetVolume(RARESOUND sound);

void RARE_API Rare_SetRepeat(RARESOUND sound, BOOL repeat);
BOOL RARE_API Rare_IsRepeat(RARESOUND sound);

//----------------------------------------------------------------------------------------------------------------

#endif//RARE_H
