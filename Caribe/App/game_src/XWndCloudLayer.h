#pragma once
//#include "XWindow.h"
#include "_Wnd2/XWnd.h"
#include "XPropCloud.h"
#include "XFramework/Game/XEComponents.h"
//#include "Sprite/SprDat.h"

class XSprDat;
class XSprite;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/07/05 20:02
*****************************************************************/
class XCloudMove
{
public:
	XCloudMove() { Init(); }
	virtual ~XCloudMove() { Destroy(); }
	// get/setter
	// public member
	void SetState1() {
		m_State = 0;
		m_alpha = 0.f;
		m_vOffset.Set( c_speed * ( 60 * -2 ) );
	}
	void SetState2() {	
		m_State = 1;
		m_timer.Set( 4.f );
		m_timer.SetPassSec( 2.f );
		m_vOffset.Set( c_speed * (60 * 2) );
		m_alpha = 1.f;
	}
	void Process( float dt );
	GET_ACCESSOR( XE::VEC2&, vOffset );
	GET_ACCESSOR( float, alpha );
// private member
private:
	XE::VEC2 m_vOffset;
	float m_alpha = 1.f;
	CTimer m_timer;
	int m_State = 0;	// 0:나타나는중 1:나타남 2:사라지는중, 3:완전사라짐
	const float c_speed = 0.075f;
// private method
private:
	void Init() {}
	void Destroy() {}
}; // class XCloudMove

class XWndCloudLayer : public XWnd
{
	struct xtDisappear {
		CTimer timerFade;	///< 구름 사라질때 타이머
		ID idCloud;			///< 사라지는 구름.
		xtDisappear() {
			idCloud = 0;
		}
		ID getid() {
			return idCloud;
		}
	};
	//	XSurface *m_psfcCloud[3];
//	XSprDat *m_psdCloud;
	xSpr::XSPDat m_spDatCloud;
	XArrayLinearN<ID, 512> m_aryCloud;	// 구름그룹 어레이
	ID m_idSelected;		// 선택된 구름그룹
	XList4<xtDisappear> m_listDisappear;		// 사라지고 있는 구름들
	XCloudMove m_objMove[2];
	void Init() {
		//		XCLEAR_ARRAY( m_psfcCloud );
		m_idSelected = 0;
//		m_psdCloud = nullptr;
	}
	void Destroy();
	void AddDisappear( xtDisappear& fadeCloud );
public:
	XWndCloudLayer( const XE::VEC2& vPos, const XE::VEC2& vSize );
	virtual ~XWndCloudLayer() { Destroy(); }
	///< 
	GET_SET_ACCESSOR( ID, idSelected );
	int Process( float dt );
	void Draw();
	void UpdateClouds( XArrayLinearN<ID, 512>& ary );
	void UpdateClouds( const std::vector<ID>& ary );
	void DoDisappear( ID idCloud );
	BOOL GetDisappearCloud( ID idCloud, xtDisappear *pOut );
	BOOL ChangeidCloud( ID idFind, ID idNew );
	BOOL IsWndAreaIn( float lx, float ly );
private:
	void DrawCloudLayer( XArrayLinearN<ID, 512>& ary, bool bOutlineOnly, const XE::VEC2& vOffset, float alphaBase );
	void DrawCloudArea( XPropCloud::xCloud* pProp, const XE::POINT& nHexa, bool bOutlineOnly, const XE::VEC2& vOffset, float alphaBase );
	void DrawHexaElem( const XE::VEC2& vt, ID idCloud, const XPropCloud::xHexa& hexa, XSprite* psfcCloud, float alphaBase, float scaleCloud );
//	void DrawHexaByFade( const XE::VEC2& vt, ID idCloud, const XPropCloud::xHexa& hexa, XSprite* psfcCloud, float alphaBase, float scaleCloud );
#ifdef _xIN_TOOL
	void DrawHexaElemByOutline( XPropCloud::xCloud* pProp
														, const XE::VEC2& v
														, ID idCloud
														, const XPropCloud::xHexa& hexa
														, XSprite* psfcCloud
														, float alphaBase
														, float scaleCloud );
	void DrawHexaDebugInfo( XPropCloud::xCloud* pProp, const XE::VEC2& vSum );
#endif
}; // XWndCloudLayer

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/07/10 16:55
*****************************************************************/
//class XWndAreaBanner : public XWndImage
class XWndAreaBanner : public XWnd
											, public XDelegateCompWave
{
public:
	XWndAreaBanner( const XE::VEC2& vCenter, ID idArea, LPCTSTR szName );
	virtual ~XWndAreaBanner() { Destroy(); }
	// get/setter
	// public member
	void SetsecLife( float secLife ) {
		m_timerLife.Set( secLife );
		m_State = 1;
	}
// private member
private:
	CTimer m_timerLife;
	int m_State = 0;	// 0:가만있는상태 1:사라지는상태 2:완전히 사라진상태
//	XWndTextString *m_pTextCost = nullptr;
//	XWndImage *m_pImgGold = nullptr;
	XWndImage *m_pImgBg = nullptr;
	ID m_idArea = 0;
	CTimer m_timerFace;
	int m_idxUnit = 0;
// private method
private:
	void Init() {}
	void Destroy() {}
	int Process( float dt ) override;
	void Update() override;
	void DelegateEndState( XECompWave* pComp, int state ) override;
}; // class XWndAreaBanner

