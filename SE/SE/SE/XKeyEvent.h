#pragma once
#include "XKeyBase.h"

#define MAX_STR_PARAM		4

//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
class XKeyEvent : public XBaseKey
{
	friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
public:
	// 이 키의 타입들
	static xSpr::xtKey sGetType() {
		return xSpr::xKT_EVENT;
	}
	static xSpr::xtKeySub sGetChannelType() {
		return xSpr::xKTS_MAIN;
	}
	//static xSpr::xtLayer sGetLayerType() {
	//	return xSpr::xLT_EVENT;
	//}
	static xSpr::xtLayerSub sGetLayerChnnelType() {
		return xSpr::xLTS_MAIN;
	}
	// 외부호출 방지용
	XKeyEvent( const XKeyEvent& src ) {
		*this = src;
	}
private:
	xSpr::xtEventKey m_Event;		// 
	float m_lx, m_ly;				// 이벤트 발생 좌표. 센터를 기준으로한 로컬좌표
	xSpr::KEY_PARAM	m_Param[16];		// 4 * 16 byte만큼을 파라메터로 쓸수 있다
	std::string m_strSpr;
	std::vector<std::string> m_aryStrParam;
	void _Init() {
		SetSubType( xSpr::xKTS_MAIN );
		m_lx = m_ly = 0;
		m_Event = xSpr::xEKT_NONE;
		memset( m_Param, 0, sizeof(m_Param) );
		SettypeLoop( xRPT_1PLAY );
		SetidAct(1);
		SetScale( 1.f );
// 		m_aryStrParam[0] = std::string( "hello" );
// 		m_aryStrParam[1] = std::string( "hi" );
// 		m_aryStrParam[2] = std::string( "bye!" );
	}
	void _Destroy() {}
protected:
	XKeyEvent() : XBaseKey(), m_aryStrParam(MAX_STR_PARAM) { _Init(); }
public:
	XKeyEvent( XSPAction spAction, ID idLayer, ID idLocalInLayer ) 
		: XBaseKey( spAction, idLayer, idLocalInLayer )
		, m_aryStrParam(MAX_STR_PARAM){ 
		_Init(); 
		SetType( xSpr::xKT_EVENT ); 
	}
	XKeyEvent( XSPAction spAction, float fFrame, XSPLayerEvent spLayer, xSpr::xtEventKey Event, float x, float y );
	XKeyEvent( XSPAction spAction, float fFrame, XSPLayerEvent spLayer, DWORD id, float x, float y );
	virtual ~XKeyEvent() { _Destroy(); }

// 	XKeyEvent( const XKeyEvent& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer ) 
// 		: XBaseKey( src, spActNew, idLayer, idLocalInLayer ) {
// //		*this = src;
// 		// this만의 깊은복사 처리
// 	}
	XBaseKey* CreateCopy() override;
	GET_ACCESSOR( float, lx );
	GET_ACCESSOR( float, ly );
	GET_ACCESSOR( xSpr::xtEventKey, Event );
//	GET_ACCESSOR( xSpr::KEY_PARAM*, Param );
	//GET_ACCESSOR( xSPLayerEvent, spLayer );
	void SetPos( float lx, float ly ) { m_lx = lx; m_ly = ly; }
	void SetAngle( float angle ) { 
		SeteventdAng( angle );
	}
	float GetAngle() { 
		return GeteventdAng();
//		return m_Param[1].fParam; 
	}
	void SetSprFile( const char* cSpr ) {
		m_strSpr = cSpr;
	}
	const char* GetSprFile() {
		return m_strSpr.c_str();
	}

	GET_SET_KEY_PARAM_DWORD( eventidObj, 0 );
	GET_SET_KEY_PARAM_FLOAT( eventdAng, 1 );
	GET_SET_KEY_PARAM_DWORD( idAct, 2 );
	GET_SET_KEY_PARAM_DWORD( typeLoop, 3 );
	GET_SET_KEY_PARAM_FLOAT( secLifeTime, 4 );
	GET_SET_KEY_PARAM_BOOL( bTraceParent, 5, 0 );	// b[0]사용
	GET_SET_KEY_PARAM_FLOAT( Scale, 6 );
	void CopyParam( XKeyEvent *pSrc ) { 
		memcpy( m_Param, pSrc->m_Param, sizeof(m_Param) ); 
	}

// 	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec=0 ) override{ 
// 		XBaseKey::Execute( pSprObj, fOverSec ); 
// 	}
	void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
	void ValueAssign( XBaseKey *pSrc ) override {		
		*this = *(dynamic_cast<XKeyEvent *>(pSrc));
	}
	BOOL EditDialog() override;
	void GetToolTipSize( float *w, float *h ) override;
	CString GetToolTipString( XSPBaseLayer spLayer ) override;
	void InitRandom() override {}
private:
}; // class XKeyEvent


XE_NAMESPACE_START( xSpr )

// KeyEvent 델리게이트시 넘겨주는 파라메터
struct xEvent {
	_tstring m_strSpr;
	ID m_idAct = 0;
	xRPT_TYPE m_PlayMode = xRPT_LOOP;
	XSprObj* m_pSprObj = nullptr;
	XKeyEvent* m_pKey = nullptr;
	ID m_idEvent = 0;
	XE::VEC2 m_vLocal;
	float m_dAng = 0;
	float m_fOverSec = 0;
	float m_Scale = 1.f;
	bool m_bTraceParent = false;
	float m_secLifeTime = 0;
};
//
XE_NAMESPACE_END; // xSpr

