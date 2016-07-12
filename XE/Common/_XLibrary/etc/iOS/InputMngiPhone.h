#pragma once

#include "etc/InputMng.h"

class XInputMngiPhone : public XInputMng
{
public:
	static XInputMngiPhone *s_pInputMngiPhone;
protected:
	int m_nTouchCount;
	void _Init( void ) {
		m_nTouchCount = 0;
	}
	void _Destroy( void ) {}
public:
	XInputMngiPhone() {
        _Init();
    }
	virtual ~XInputMngiPhone() { _Destroy(); }
	
};

class XInputMngiPad : public XInputMngiPhone
{
public:
	XInputMngiPad() {}
	virtual ~XInputMngiPad() {}
	
};

#define INPUTMNG_IPHONE	XInputMngiPhone::s_pInputMngiPhone



