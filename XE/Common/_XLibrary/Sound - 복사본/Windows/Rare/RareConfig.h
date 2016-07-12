#ifndef	RARECONFIG_H
#define	RARECONFIG_H

#if	defined(_DEBUG)
#	ifndef	DEBUG
#		define	DEBUG	_DEBUG
#	endif
#elif defined(DEBUG)
#	define	_DEBUG		DEBUG
#endif

#if defined(_UNICODE)
#	ifndef	UNICODE
#		define	UNICODE	_UNICODE
#	endif
#elif defined(UNICODE)
#	define	_UNICODE	UNICODE
#endif

#ifdef  WIN32
#define	WIN32_LEAN_AND_MEAN
#endif	//WIN32

#define RARELOG


#ifdef	_MSC_VER
#define	DLL_IMPORT	__declspec(dllimport)
#define	DLL_EXPORT	__declspec(dllexport)
#else
#define	DLL_IMPORT
#define	DLL_EXPORT
#endif

#ifdef	_MSC_VER
#	ifndef	_CPPUNWIND
#		error	C++ Exception Handling Required
#	endif
#	ifndef	_DLL
#		ifndef	DEBUG
#			error	DLL Version CRT Required (Use /MD option)
#		else
#			error	DLL Version CRT Required (Use /MDd option)
#		endif
#	endif	//_DLL
#endif

#ifdef	_MSC_VER
#pragma warning( disable: 4251 )	//non-exported class derived from DLL class
#pragma warning( disable: 4284 )	//error related unused template fnction
#pragma	warning( disable: 4786 )	//long symbol cannot debugged
#endif

// Rare_Open Type 
enum Rare_Open {
	RARE_OPENSTREAM = 0,
	RARE_OPENEFFECT
};

enum Rare_Notify {
	//RARE_END   = 0,
	RARE_STOP  = 0,
	RARE_PLAY  = 1,
	RARE_PAUSE = 2,
};

typedef class RContext* RARECONTEXT;
typedef class RSound* RARESOUND;

#ifndef RNOTIFY
#define RNOTIFY

typedef class RNotify
{
public:
	virtual void RareNotify(Rare_Notify code, RARESOUND sound){}
} RARENOTIFY;

#endif//RNOTIFY


#endif//RARECONFIG_H
