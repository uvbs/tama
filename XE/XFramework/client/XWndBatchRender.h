/********************************************************************
	@date:	2016/09/17 10:56
	@file: 	C:\p4v\iPhone_zero\XE\XFramework\client\XWndBatchRender.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "_Wnd2/XWnd.h"

class XBatchRenderer;
class XTextureAtlas;
class XWndBatchRender;
/****************************************************************
* @brief
* @author xuzhu
* @date	2016/09/17 0:26
*****************************************************************/
class XAutoCurrAtlas {
public:
	XAutoCurrAtlas( XWndBatchRender* pWnd );
	~XAutoCurrAtlas();
	// get/setter
	// public member
private:
	// private member
	XWndBatchRender* m_pWndRenderer = nullptr;
private:
	// private method
}; // class XAutoCurrAtlas
	 /****************************************************************
* @brief 
* @author xuzhu
* @date	2016/09/16 21:51
*****************************************************************/
class XWndBatchRender : public XWnd
{
friend class XAutoCurrAtlas;
public:
	XWndBatchRender( const char* cTag, bool bBatchRender, const XE::xRECT& rc );
	// 크기를 따로 지정하지 않으면 부모윈도우의 크기를 따른다.
	XWndBatchRender( const char* cTag, bool bBatchRender )
		: XWndBatchRender( cTag, bBatchRender, XE::xRECT() ) {	}
//	: XWndBatchRender( cTag, bBatchRender, XE::xRECT( XE::VEC2( 0, 0 ), XE::GetGameSize() ) ) {	}
	~XWndBatchRender() {
		Destroy();
	}
	// get/setter
	GET_ACCESSOR( XTextureAtlas*, pAtlas );
	// public member
// 	void AttatchBatchRenderer();
protected:
	void Draw() override;
	void DrawAfter() override;
	int Process( float dt ) override;
	void DrawBefore() override;
private:
	// private member
	XBatchRenderer* m_pRenderer = nullptr;
	XTextureAtlas* m_pAtlas = nullptr;
	XBatchRenderer* m_pPrev = nullptr;
private:
	// private method
	GET_ACCESSOR( XBatchRenderer*, pRenderer );
	void Init() {}
	void Destroy();
	BOOL OnCreate() override;
}; // class XWndBatchRender

