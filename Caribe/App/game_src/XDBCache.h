#pragma once
#ifdef _XDB_CACHE
class XCCAccount : public XLAccount
{
	XPacket m_Packet;
	CTimer m_timerLife;
	void Init() {}
	void Destroy() {}
public:
	XCCAccount() { 
		Init(); 
		m_timerLife.Set(0);
	}
	virtual ~XCCAccount() { Destroy(); }
	//
	void SetPacket( XPacket& p ) {
		// 패킷 카피
		m_Packet = p;
	}
};

//////////////////////////////////////////////////////////////////////////
class XPacket;
class XDBCache
{
	XList2<XCCAccount> m_listAcc;
	map<ID, XCCAccount*> m_mapIDAcc;			// 계정아이디 검색용 
	map<string, XCCAccount*> m_mapUUID;	
	map<string, XCCAccount*> m_mapID;			// 영문아이디 검색용
	void Init() {}
	void Destroy() {}
public:
	XDBCache( int max );
	virtual ~XDBCache() { Destroy(); }
	//
	XCCAccount* Add( ID idAccount, XPacket& p );
	XCCAccount* FindFromUUID( const char *cUUID );
	XCCAccount* FindFromAccountID( ID idAccount );
	XCCAccount* FindFromPassword( LPCTSTR szID, LPCTSTR szPassword );
};




#endif // _XDB_CACHE