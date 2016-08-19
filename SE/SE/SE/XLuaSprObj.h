#pragma once
#include "xLua.h"

class XLuaSprObj : public XLua
{
	void Init() {
	}
	void Destroy() {}
public:
	XLuaSprObj() { 
		Init(); 
		RegisterLua();
	}
	virtual ~XLuaSprObj() { Destroy(); }

	virtual void RegisterLua();

};
