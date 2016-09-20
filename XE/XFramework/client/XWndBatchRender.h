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
	XWndBatchRender( const char* cTag, bool bBatchRender );
	~XWndBatchRender() {
		Destroy();
	}
	// get/setter
	GET_ACCESSOR( XTextureAtlas*, pAtlas );
	// public member
// 	void AttatchBatchRenderer();
protected:
	void Draw() override;
	int Process( float dt ) override;
private:
	// private member
	XBatchRenderer* m_pRenderer = nullptr;
	XTextureAtlas* m_pAtlas = nullptr;
private:
	// private method
	GET_ACCESSOR( XBatchRenderer*, pRenderer );
	void Init() {}
	void Destroy();
	BOOL OnCreate() override;
}; // class XWndBatchRender

