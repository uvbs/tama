#pragma once

XE_NAMESPACE_START( xnBgObj )
//

struct xProp {
	ID m_idObj = 0;		// 각 오브젝트의 고유아이디
	XGAME::xtBgObj m_Type = XGAME::xBOT_NONE;
	ID m_typeSub = 0;			// 배경오브젝트 타입내에서 다시 형태에따라 서브아이디를 붙인다.
	_tstring m_strSpr;
	ID m_idAct = 0;
	XE::VEC2 m_vwPos;		// 월드상에서의 좌표
	ID getid() {
		return m_idObj;
	}
};
//
XE_NAMESPACE_END; // xnBgObj

