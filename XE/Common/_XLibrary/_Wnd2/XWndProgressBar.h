#pragma once

#include "XWndView.h"
#include "../XDrawGraph.h"

 class XWndAlert;
// class XWndButtonString;
class XWndProgressBar2;
class XSprObj;
// class XParticleMng;
// class XParticleDelegate;
// class XLayout;
//class XWndTextStringArea;
//////////////////////////////////////////////////////////////////////////
class XWndProgressBar : public XWnd, public XProgressBar {
	float m_Lerp;
	void Init() {
		m_Lerp = 1.0f;
	}
	void Destroy() {}
public:
	XWndProgressBar( float x, float y, LPCTSTR szImg, LPCTSTR szImgBg = nullptr );
	XWndProgressBar( float x, float y, float w, float h, XCOLOR col = XCOLOR_GREEN );
	virtual ~XWndProgressBar() {
		Destroy();
	}
	//
	GET_SET_ACCESSOR( float, Lerp );
	//
	void Draw( void );
};

//////////////////////////////////////////////////////////////////////////
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/12/01 18:22
*****************************************************************/
class XEDelegateProgressBar2
{
public:
	XEDelegateProgressBar2() {}
	virtual ~XEDelegateProgressBar2() {}
	/// 그래프바가 애니메이션되면서 현재레벨을 넘어서면 발생한다.
	virtual void OnDelegateChangeLevel( XWndProgressBar2 *pBar, int lvCurrByAni ) {}
}; // class XEDelegateProgressBar2
/**
 @brief 
*/
class XWndProgressBar2 : public XWnd
{
public:
	static XWndProgressBar2* sUpdateCtrl( XWnd* pParent, const XE::VEC2& vPos, const _tstring& strRes, const std::string& ids );
private:
	struct xLayer {
	public:
		ID m_idLayer = 0;
		float m_LerpStart = 0.f;
		float m_Lerp = 0.f;
		float m_LerpDst = -1.f;		// 애니메이션시 목표값.
		XSurface *m_psfcBar = nullptr;
		XSprObj* m_psoTail = nullptr;
		ID m_idAct = 0;
		XE::xtBlendFunc m_blendFunc = XE::xBF_MULTIPLY;
		bool m_bFlipH = false;
		bool m_bFlipV = false;
		XE::VEC2 m_vAdj;		// draw좌표 보정
		bool m_bFixed = false;		// lerp값에 영향을 받지 않음.
		bool m_bShow = true;
//		bool m_bReverse = false;		// 일반적으론 바의 원점이 왼쪽이지만 반대로 하고싶을때 true
		ID m_idTraceTail = 0;				// 바의 끝부분에 붙어다니는 효과를 어느레이어기준으로 할건지.
		ID getid() const {
			return m_idLayer;
		}
	};
public:
	XWndProgressBar2() { Init(); }
	XWndProgressBar2( float x, float y, LPCTSTR szImgBar = nullptr );
	XWndProgressBar2( const XE::VEC2& vPos, LPCTSTR szImgBar = nullptr )
		: XWndProgressBar2( vPos.x, vPos.y, szImgBar ) {}
	XWndProgressBar2( const XE::VEC2& vPos, const _tstring& strImg )
		: XWndProgressBar2( vPos.x, vPos.y, strImg.c_str() ) {}
//	XWndProgressBar2( float x, float y, float w, float h, XCOLOR col = XCOLOR_GREEN );
	virtual ~XWndProgressBar2() { Destroy(); }
	//
//	GET_SET_ACCESSOR( float, Lerp );
	GET_BOOL_ACCESSOR( bHoriz );
	GET_SET_BOOL_ACCESSOR( bAnimation );
	GET_ACCESSOR( int, lvCurrByAni );
	GET_SET_ACCESSOR( XEDelegateProgressBar2*, pDelegate );
	GET_SET_BOOL_ACCESSOR( bReverse );
	//
	xLayer* AddLayer( ID idLayer, LPCTSTR szImg, bool bFixed = false, const XE::VEC2& vAdj = XE::VEC2(0) );
	inline xLayer* AddLayer( ID idLayer, const _tstring& strImg, bool bFixed = false, const XE::VEC2& vAdj = XE::VEC2(0) ) {
		return AddLayer( idLayer, strImg.c_str(), bFixed, vAdj );
	}
	const xLayer* GetpLayer( ID idLayer );
	void DoLerpAni( ID idLayer
								, int lvOrig
								, int lvStart, float lerpStart
								, int lvDst, float lerpDst
								, float secTotal );
	void SetLerp( ID idLayer, float lerp );
	inline void SetLerp( ID idLayer, float curr, float max ) {
		SetLerp( idLayer, curr / max );
	}
	inline void SetLerp( ID idLayer, int curr, int max ) {
		SetLerp( idLayer, (float)curr / (float)max );
	}
	inline void SetLerp( ID idLayer, DWORD curr, DWORD max ) {
		SetLerp( idLayer, (float)curr / (float)max );
	}
	void SetLerp( ID idLayer
							, int lvOrig
							, int lvDst, float lerpDst );
//	xLayer* GetLayer( ID idLayer );
	void SetShowLayer( ID idLayer, bool bShow );
	// fixed속성의 레이어만 제외하고 모든 레이어 전부의 lerp를 동일하게 맞춤.
	void SetLerp( float lerp );
	void Draw() override;
	int Process( float dt ) override;
	XE::VEC2 GetvlPosTail( ID idLayer );
private:
	void Init() {}
	void Destroy();
private:
	bool m_bHoriz = true;
	bool m_bReverse = false;
	bool m_bAnimation = false;
	CTimer m_timerAni;					// 애니메이션이 설정되었을때 목표값까지 도달하는 시간
	XList4<xLayer> m_listLayers;
	int m_lvOrig = 0;
	int m_lvStartByAni = 0;
	int m_lvCurrByAni = 0;			// 애니메이션 되는중의 현재 레벨
	int m_lvDstByAni = 0;
	XEDelegateProgressBar2 *m_pDelegate = nullptr;
};

