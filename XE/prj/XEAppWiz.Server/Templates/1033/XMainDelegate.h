#pragma once

#include "server/XEServerDelegate.h"

////////////////////////////////////////////////////////////////
class XMainDelegate : public XEServerDelegate
{
	void Init() {}
	void Destroy();
public:
	XMainDelegate();
	virtual ~XMainDelegate() { Destroy(); }
	//
	virtual XEServerMain* OnCreateMain( _tstring& strINI );
};

