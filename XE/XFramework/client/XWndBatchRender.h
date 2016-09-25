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
	XWndBatchRender( const char* cTag, bool bBatchRender, 
									 bool bZBuff, bool bAlphaTest, 
									 const XE::xRECT& rc );
	XWndBatchRender( const char* cTag, bool bBatchRender, 
									 bool bZBuff, bool bAlphaTest, 
									 const XE::VEC2& vPos, const XE::VEC2& vSize )
		: XWndBatchRender( cTag, bBatchRender, bZBuff, bAlphaTest, 
											 XE::xRECT( vPos, vSize ) ) {}
	// 크기를 따로 지정하지 않으면 부모윈도우의 크기를 따른다.
	XWndBatchRender( const char* cTag, bool bBatchRender, 
									 bool bZBuff, bool bAlphaTest )
		: XWndBatchRender( cTag, bBatchRender, 
											 bZBuff, bAlphaTest, XE::xRECT() ) {	}
	~XWndBatchRender() {
		Destroy();
	}
	// get/setter
	GET_ACCESSOR( XSPAtlasMng, spAtlas );
	GET_SET_BOOL_ACCESSOR( bZBuff );
	GET_SET_BOOL_ACCESSOR( bAlphaTest );
	GET_ACCESSOR( XBatchRenderer*, pRenderer );
	// public member
	
// 	void AttatchBatchRenderer();
protected:
	void Draw() override;
	void OnDrawAfter() override;
	void DestroyDevice();
	void OnPause();
	void OnUpdateBefore();
	void OnUpdateAfter();
	void OnProcessBefore();
	void OnProcessAfter();
	int Process( float dt ) override;
	void OnDrawBefore() override;
private:
	// private member
	XBatchRenderer* m_pRenderer = nullptr;
	XSPAtlasMng m_spAtlas;
	XBatchRenderer* m_pPrev = nullptr;
	bool m_bZBuff = false;
	bool m_bAlphaTest = false;
	bool m_bZBuffPrev = false;
	bool m_bAlphaTestPrev = false;
private:
	// private method
//	GET_ACCESSOR( XBatchRenderer*, pRenderer );
	void Init() {}
	void Destroy();
	BOOL OnCreate() override;
}; // class XWndBatchRender

