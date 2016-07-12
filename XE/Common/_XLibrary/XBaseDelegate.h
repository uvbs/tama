#pragma once

class XBaseDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XBaseDelegate() { Init(); }
	virtual ~XBaseDelegate() { Destroy(); }
};
