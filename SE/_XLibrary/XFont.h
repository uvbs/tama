#pragma once

	#include "XBaseFontDat.h"
	#include "XBaseFontObj.h"
	#ifdef WIN32
	#include "XFontObjDX.h"
	#include "XFontDatDX.h"
	#else
	#include "XFontObjFTGL.h"
	#include "XFontDatFTGL.h"
	#endif

