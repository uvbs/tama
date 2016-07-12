#pragma once

#ifdef WIN32
	#define DIR_ROOT		_T("")
	#define DIR_ICON		_T("icon/")
	#define DIR_UI			_T("ui/")
	#define DIR_FONT		_T("fonts/")
	#define DIR_PROP		_T("prop/")
	#define DIR_SCRIPT	_T("prop/")
	#define DIR_SCRIPTA	"prop/"
	#define DIR_SCRIPTW	_T("prop/")
	#define DIR_LAYOUT	_T("prop/layout/")
	#define DIR_IMG		_T("img/")
	  #ifdef _XPATCH
			#define DIR_BGM		"../../win32/ogg/"		
			#define DIR_SND		"../../win32/ogg/"		
	  #else
			#define DIR_BGM		"../win32/ogg/"		
			#define DIR_SND		"../win32/ogg/"		
	  #endif
	#define DIR_BGM_MP3		"snd/mp3/"		
	#define DIR_PARTICLE	_T("particle/")
#else // win32
	//#define DIR_SPR		"spr/"
	#define DIR_ROOT		""
	#define DIR_ICON    "icon/"
	#define DIR_BG		"bg/"
	#define DIR_SND		"snd/"
	#define DIR_BGM		"snd/mp3/"
	#define DIR_PROP	"prop/"
	#define DIR_SCRIPT	"prop/"
	#define DIR_SCRIPTA	"prop/"
	#define DIR_SCRIPTW	"prop/"
	#define DIR_LAYOUT	_T("prop/layout/")
	#define DIR_FONT	"fonts/"
	#define DIR_VE		"ve/"
	#define DIR_IMG     "img/"
	#define DIR_UI      "ui/"
	#define DIR_PARTICLE "particle/"
#endif // not win32

#define PATH_UI(FILENAME)	XE::MakePath(DIR_UI,_T(FILENAME))
#define PATH_IMG(FILENAME)	XE::MakePath(DIR_IMG,_T(FILENAME))
#define PATH_ICON(FILENAME)	XE::MakePath(DIR_ICON,_T(FILENAME))
#define PATH_PROP(FILENAME)	XE::MakePath(DIR_PROP,_T(FILENAME))
