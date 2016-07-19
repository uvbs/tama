#pragma once

class Empty
{
	void Init() {}
	void Destroy() {}
public:
	Empty() { Init(); }
	virtual ~Empty() { Destroy(); }
	//
};