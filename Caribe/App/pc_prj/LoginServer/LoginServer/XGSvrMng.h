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
	
	XEWinConnectionInServer *m_pConnect;		// �ٽ� ��Ȱ~���� this�� �ױ������� Ŀ�ؼ��� ��������� �����ᵵ ��.
	XGSvrMng					*m_pSvrMng;		// this�� �����ִ� �����Ŵ���
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
	std::map<ID, XGSvr*> m_mapSvr;			// SvrID ������� ���� �����.
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