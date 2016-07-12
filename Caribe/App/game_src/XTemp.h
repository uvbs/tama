/********************************************************************
	@date:	
	@file: 	개발중 빌드시간 단축을 위해 이곳에서 코딩을 하며 개발을 하고 완료되면 다른 cpp로 옮긴다.
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

/**
 @brief UI객체를 큐에 쌓아뒀다가 일정시간 단위로 밀어내 출력하는 컴포넌트.
*/
class XMsgPusher
{
	XList4<XWnd*> m_qObj;			//
	XWnd* m_pWndLayer = nullptr;		// add받은 ui를 뿌려야할 레이어
	CTimer m_timerDelay;
	float m_secDelay = 0.f;
	void Init() {}
	void Destroy() {}
public:
	XMsgPusher( XWnd* pWndLayer, float secDelay );
	~XMsgPusher() { Destroy(); }
	//
	void Process( float dt );
	void Add( XWnd* pPushUI );
};

