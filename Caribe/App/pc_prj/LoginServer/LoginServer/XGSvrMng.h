#pragma once
#if 0
#include "XList.h"
#include "XRefObj.h"

class XEWinSocketSvr;
class XEWinConnectionInServer;
class XPacket;
class XGSvrMng;

class XGSvr :public XRefObj
{
private:
	ID m_ServerID;
	int	m_ConnectUserCnt;
	
	XEWinConnectionInServer *m_pConnect;		// 다시 부활~이제 this가 죽기전까진 커넥션은 살아있으니 맘껏써도 됨.
	XGSvrMng					*m_pSvrMng;		// this가 속해있는 유저매니저
	void Init() {
		m_pConnect = NULL;		
		m_pSvrMng  = NULL;
	}
	void Destroy(){}
public:
	XGSvr(XGSvrMng *pSvrMng, XEWinConnectionInServer *pConnect);
	virtual ~XGSvr(){ Destroy(); }

	GET_SET_ACCESSOR( ID, ServerID );
	GET_SET_ACCESSOR( int, ConnectUserCnt);

	GET_ACCESSOR( XEWinConnectionInServer*, pConnect );

	void DoDisconnect( void );

	virtual void Process( float dt ) {}
	virtual BOOL Save( void ) { return TRUE; }
	virtual void OnConnect( void ) {}
	virtual void OnDestroy( void ) {}
};

class XGSvrMng
{
private:
	std::map<ID, XGSvr*> m_mapSvr;			// SvrID 기반으로 맵을 만든다.
	XEWinSocketSvr *m_pParentSocketSvr;
	void Init() {
		m_pParentSocketSvr = NULL;
	}
	void _Destroy() {	}

protected:
	XList2<XGSvr> m_listGSvr;
public:
	XGSvrMng( XEWinSocketSvr *pParent, int maxUser );
	virtual ~XGSvrMng() { _Destroy(); }	
	//
	GET_ACCESSOR( XList2<XGSvr>&, listGSvr );
	GET_ACCESSOR( XEWinSocketSvr*, pParentSocketSvr );

	BOOL Add( XGSvr *pSvr);
//	void DestroyUser( XUser *pUser );
	XGSvr* GetSvrFromSvrID( ID SvrID);
	int GetnumGSvr( void ) {
		return m_listGSvr.size();
	}

	void DelGSvr( XGSvr *pSvr );
	void Flush( void );
	void Process( float dt );
	void Save( void );
	//
	virtual void OnDestroyGSvr( XGSvr *pSvr ) {}
};
#endif // 0