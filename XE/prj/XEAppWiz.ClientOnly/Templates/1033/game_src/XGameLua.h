#pragma once
#include "XLua.h"

class XGameLua : public XLua
{
	void Init() {}
	void Destroy() {}
public:
	XGameLua();
	XGameLua( const char *cLua );
	virtual ~XGameLua() { Destroy(); }
	//
	virtual void RegisterGlobal( void );
};

