#pragma once
#ifndef _SERVER
class XPThread //: public XBaseThread
{
	void Init() {}
	void Destroy() {}
public:
	XPThread() { Init(); }
	virtual ~XPThread() { Destroy(); }
};

#endif // not SERVER