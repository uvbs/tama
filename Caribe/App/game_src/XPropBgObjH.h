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
	_tstring m_strSnd;		// 사운드 파일명
	ID getid() {
		return m_idObj;
	}
	bool IsInArea( const XE::VEC2& vwLT, const XE::VEC2& vwSize ) const {
		return XE::IsArea( vwLT, vwSize, m_vwPos ) != FALSE;
	}
#ifdef _xIN_TOOL
	std::string m_strType;	// m_Type의 스트링을 저장.
	void SetNew( XGAME::xtBgObj type, const _tstring& strSpr, ID idAct, const XE::VEC2& vwPos );
#endif // _xIN_TOOL
};
const char* GetstrEnum( XGAME::xtBgObj type );
//
XE_NAMESPACE_END; // xnBgObj

