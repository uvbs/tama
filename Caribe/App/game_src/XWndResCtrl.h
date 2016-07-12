/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "_Wnd2/XWnd.h"
#include "XStruct.h"

class XWndTextNumberCounter;
/****************************************************************
* @brief
* @author xuzhu
* @date	2015/04/20 15:05
*****************************************************************/
class XWndResourceCtrl : public XWnd
{
	struct xEach {
		XGAME::xtResource type = XGAME::xRES_NONE;
		int num = 0;
		XCOLOR col = XCOLOR_WHITE;
	};
	XList4<xEach> m_listResource;
	XVector<int> m_aryNumResPrev;		// TextCounter타입을 쓸경우 값이 바뀔때 이전값과 비교하기 위해 이전 리소스양을 항상 받아둔다.
	bool m_bVert = true;			// 세로로 나열할건지 가로로 나열할건지.
	bool m_bCounter = false;	// XWndTextStringCounter로 생성하는버전.
	bool m_bSymbolNumber = false;		// 큰숫자를 K,M,B단위로 보여주기
	float m_spaceY = 1.f;		// 각 라인간 추가간격
	void Init() {}
	void Destroy();
public:
	XWndResourceCtrl() : m_aryNumResPrev( XGAME::xRES_ALL ) {
		Init();
		SetbUpdate( true );
	}
	XWndResourceCtrl( const XE::VEC2& vPos )
		: XWnd( vPos ), m_aryNumResPrev( XGAME::xRES_ALL ) {
		Init();
		SetbUpdate( true );
	}
	XWndResourceCtrl( const XE::VEC2& vPos, bool bCounter )
		: XWnd( vPos ), m_aryNumResPrev( XGAME::xRES_ALL ) {
		Init();
		m_bCounter = bCounter;
		SetbUpdate( true );
	}
	XWndResourceCtrl( const XE::VEC2& vPos, const std::vector<XGAME::xRES_NUM>& aryRes, bool bVert, float scale = 1.f )
		: XWnd( vPos ), m_aryNumResPrev( XGAME::xRES_ALL ) {
		Init();
		m_bVert = bVert;
		AddRes( aryRes );
		XWndResourceCtrl::SetScaleLocal( scale, scale );
		SetbUpdate( true );
	}
	XWndResourceCtrl( const XE::VEC2& vPos, const XList4<XGAME::xRES_NUM>& listRes, bool bVert, float scale = 1.f )
		: XWnd( vPos ), m_aryNumResPrev( XGAME::xRES_ALL ) {
		Init();
		m_bVert = bVert;
		AddRes( listRes );
		XWndResourceCtrl::SetScaleLocal( scale, scale );
		SetbUpdate( true );
	}
	XWndResourceCtrl( const XE::VEC2& vPos, XGAME::xtResource type, int num, XCOLOR col = XCOLOR_WHITE );
	virtual ~XWndResourceCtrl() { Destroy(); }
	//
	GET_SET_BOOL_ACCESSOR( bVert );
	GET_SET_BOOL_ACCESSOR( bSymbolNumber );
	GET_SET_ACCESSOR_CONST( float, spaceY );
// 	void SetScaleLocal( float sx, float sy ) override;
// 	inline void SetScaleLocal( float scalexy ) {
// 		SetScaleLocal( scalexy, scalexy );
// 	}
	void AddRes( XGAME::xtResource type, int num, XCOLOR col = XCOLOR_WHITE );
	inline void AddRes( XGAME::xtResource type, float num, XCOLOR col = XCOLOR_WHITE ) {
		AddRes( type, (int)num, col );
	}
	void AddRes( const XGAME::xReward& reward, XCOLOR col = XCOLOR_WHITE ) {
		if( XASSERT(reward.IsTypeResource()) )
			AddRes( reward.GetResourceType(), reward.num, col );
	}
	void AddResWithAry( const XVector<XGAME::xReward>& aryReward );
	void AddRes( const std::vector<XGAME::xRES_NUM>& aryRes ) {
		for( auto& res : aryRes )
			AddRes( res.type, res.num );
	}
	void AddRes( const XList4<XGAME::xRES_NUM>& listRes ) {
		for( auto& res : listRes )
			AddRes( res.type, res.num );
	}
	void AddRes( const XGAME::xRES_NUM& res, XCOLOR col = XCOLOR_WHITE );
	void EditRes( XGAME::xtResource type, int num, XCOLOR col = 0 );
	void EditResColor( XGAME::xtResource type, XCOLOR col );
	void EditResNum( XGAME::xtResource type, int num );
	void SetColText( XCOLOR col );
private:
	void UpdateWnd();
	void Update() override;
	void Draw() override;
	void UpdateTextCounter( XGAME::xtResource typeRes );
	void UpdateTextCounter( XWndTextNumberCounter* pTextCnt, XGAME::xtResource typeRes, int numCurr );
}; // class XWndResourceCtrl

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/02/23 17:45
*****************************************************************/
class XWndResSpr : public XWnd
{
public:
	XWndResSpr( XGAME::xtResource resType );
	virtual ~XWndResSpr() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	XGAME::xtResource m_resType = XGAME::xRES_NONE;
private:
	// private method
	void Init() {}
	void Destroy() {}
	XE::VEC2 GetSizeNoTransLayout() override {
		return GetSizeLocal();
	}
}; // class XWndResSpr

