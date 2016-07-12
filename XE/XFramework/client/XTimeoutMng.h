#pragma once
#include "XAutoPtr.h"
class XWnd;

class XTimeoutDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XTimeoutDelegate() { Init(); }
	virtual ~XTimeoutDelegate() { Destroy(); }

//	virtual void OnTimeout( DWORD idPacket ) {}
	virtual void OnTimeout2( DWORD idPacket, ID idSubPacket ) {}
};


class XTimeoutMng;
class XTimeoutMng
{
public:
	static XTimeoutMng* sGet( void );
private:
	static XTimeoutMng *s_pSingleton;
	static XAutoPtr<XTimeoutMng*> s_autoDestroy;
	struct xREQUEST {
		ID idWnd;
		ID idPacket;
		ID idSendPacket;	// 보낼때 쓴 패킷
		ID idSubPacket;		// 이것은 장차 사라지고, aryPacketID를 쓰게 될 것이다.
		BOOL bBlocking;		// 응답을 기다리는동안 키입력을 막을것인가.
		CTimer timerTimout;
		XArrayLinearN<ID,16> aryPacketID;		// idPacket이 키값으로 쓰였을때 다수의 패킷아이디들이 입력된다.
		xREQUEST() {
			idWnd = 0;
			idPacket = 0;
			idSendPacket = 0;
			idSubPacket = 0;
			bBlocking = TRUE;
		}
		ID GetidPacket() {
			if( aryPacketID.size() > 0 )
				return aryPacketID[0];
			else
				return idPacket;
		}
		float GetsecRemain() {
			return timerTimout.GetRemainSec();
		}
	};
	std::list<xREQUEST> m_listTimeout;
//	XWnd *m_pWndTop;
	CTimer m_timerRequest;		// 타임아웃 객체가 add되면 작동하기 시작한다. 일정시간이 지나면 메시지를 보여주기 시작한다.
	BOOL m_bClear;
	void Init() {
//		m_pWndTop = NULL;
		m_bClear = FALSE;
	}
	void Destroy() {}
	xREQUEST* FindByKey( ID idKey );
public:
	XTimeoutMng() { Init(); }
	// 이건 앞으로 사라지게 될 생성자임
	XTimeoutMng( XWnd *pWndTop ) { 
		Init(); 
//		m_pWndTop = pWndTop;
	}
	virtual ~XTimeoutMng() { Destroy(); }
	//
	// 지금 뭔가 응답을 기다리는 중인가?
	BOOL IsRequesting( void );
	// 응답요청하고 메시지띄워야 할시간이 지났느냐.
	BOOL IsOverMsgTime( float sec=0.f ) {
		if( sec > 0 )
			return m_timerRequest.IsOver( sec );
		return m_timerRequest.IsOver();
	}
	// 로딩마크 띄우는 시간을 수동조절한다.
	void SetsecRequestTimeout( float sec ) {
		m_timerRequest.Set( sec );
	}
	BOOL IsHavePacketID( xREQUEST& req, ID idCompare );
	//
	void Add( ID idWnd, DWORD idPacket, float secTimeout = 20.f, BOOL bExclusive=TRUE );
	void Add( ID idWnd, DWORD idPacket, ID idSubPacket, BOOL bBlocking, float secTimeout = 20.f );
	void AddByKey( ID idWnd, ID idKey, ID idPacket, BOOL bBlocking, float secTimeout = 10.f );
	void Arrive( DWORD idPacket, ID idSubPacket=0 );
	void ArriveWithKey( DWORD idPacket );
	void Process( float dt );
	BOOL Find( ID idPacket );
	void Clear( void ) {
//		m_listTimeout.clear();
		m_bClear = TRUE;
	}
	xREQUEST* GetRequesting();
};

//extern XTimeoutMng *TIMEOUT_MNG;
