/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "_Wnd2/XWnd.h"
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndSprObj.h"
#include "XParticle.h"
#include "Sprite/XDelegateSprObj.h"

class XQuestObj;
class XLayout;
class XWndTextString;
/****************************************************************
* @brief 월드맵로비에서 우측에 퀘스트 리스트 윈도우
* @author xuzhu
* @date	2014/11/13 18:15
*****************************************************************/
class XWndCheatQuestList : public XWnd 
{
	XArrayLinearN<XQuestObj*, 256> m_aryQuest;
	void Init() {}
	void Destroy();
public:
	XWndCheatQuestList( float x, float y );
	virtual ~XWndCheatQuestList() {
		Destroy();
	}
	//
	BOOL OnCreate();
	void Update();
	void Draw( void );
}; // class XWndQuestList

/****************************************************************
* @brief 정식 퀘스트 창
* @author xuzhu
* @date	2015/01/19 15:02
*****************************************************************/
class XWndQuestList2 : public XWndView
{
	XLayout *m_pLayout = nullptr;
	bool m_bRepeat = false;
	void Init() {}
	void Destroy();
public:
	XWndQuestList2( XLayout *pLayout, float x, float y );
	virtual ~XWndQuestList2() { Destroy(); }
	//
	void Update() override;
	int OnSelectQuest( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickChange( XWnd* pWnd, DWORD p1, DWORD p2 );
	bool IsUpdate( bool bRepeatTab );
}; // class XWndQuestList2

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/01/19 16:11
*****************************************************************/
class XWndQuestElem : public XWndImage
{
	XQuestObj *m_pQuestObj = nullptr;
	XWndTextString *m_pText = nullptr;
	CTimer m_timerBlink;
	CTimer m_timerGlow;
	void Init() {}
	void Destroy();
public:
	XWndQuestElem( XQuestObj *pQuestObj );
	virtual ~XWndQuestElem() { Destroy(); }
	//
	bool IsValid();
	void Update() override;
	void Draw() override;
	int Process( float dt ) override;
}; // class XWndQuestElem

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/01/20 9:57
*****************************************************************/
class XWndQuestInfo : public XWndPopup
{
public:
	static void sDestroyWnd();
private:
	XQuestObj *m_pQuestObj = nullptr;
	void Init() {}
	void Destroy() {}
public:
	XWndQuestInfo( XQuestObj *pQuestObj );
	virtual ~XWndQuestInfo() { Destroy(); }
	//
	int OnOk( XWnd *pWnd, DWORD p1, DWORD p2 ) override;
	int OnClickGoto( XWnd* pWnd, DWORD p1, DWORD p2 );
	bool DoIndicate( ID idType, DWORD dwParam, ID idArea = 0 );
	void OnFinishAppear() override;
}; // class XWndQuestInfo

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/12/04 14:50
*****************************************************************/
class XWndCongratulation : public XWndSprObj
													, public XDelegateSprObj
													, public XParticleDelegate
{
public:
	XWndCongratulation();
	virtual ~XWndCongratulation() { Destroy(); }
	// get/setter
	// public member
	void OnFinishAni() override;
private:
	// private member
	bool m_bClicked = false;
	CTimer m_timerLife;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void OnProcessDummy( ID id
										, const XE::VEC2& vLocalT
										, float ang
										, const XE::VEC2& vScale
										, const CHANNEL_EFFECT& cnEffect
										, float dt, float fFrmCurr ) override;
	void OnLButtonDown( float lx, float ly ) override;
	void OnLButtonUp( float lx, float ly ) override;
}; // class XWndCongratulation

