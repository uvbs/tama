#pragma once
#include "client/XApp.h"

////////////////////////////////////////////////////////////////
class XAppDelegate : public XApp
{
	void Init() {}
	void Destroy() {}
public:
	XAppDelegate();
	virtual ~XAppDelegate() { Destroy(); }
	//
	virtual XClientMain* OnCreateAppMain( XE::xtDevice device, float widthPhy, float heightPhy );
};
