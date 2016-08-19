#pragma once
#include "Sprdef.h"
class XDlgBaseKey 
{
public:
	XDlgBaseKey(void) {
		m_Interpolation = xSpr::xLINEAR;;
	}
	virtual ~XDlgBaseKey(void) {}

	CString m_strLua;		// 루아코드
	xSpr::xtInterpolation m_Interpolation;	// 보간종류
//	void DoOk( int nSelectFunc ) { m_Interpolation = (xSpr::xtInterpolation)nSelectFunc; }
};
