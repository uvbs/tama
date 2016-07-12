#pragma once
#include "XAccount.h"
//#include "XWorld.h"
#include "XPostInfo.h"
#include "XPool.h"

class XPacket;
class XPushNotifyObj;

class XSAccount : public XAccount
#ifdef _GAME_SERVER
						, public XMemPool<XSAccount>	// 게임 계정에 대한 정보
#endif
{
public:
	enum eCONNECT_STATE
	{
		CONNECTION_NONE = 0,
		CONNECTION_NEW,
		CONNECTION_RE,
	};
private:

//	_tstring		m_strUUID;

	BOOL			m_Reconnected;	
	int			m_idLastConnectSvr;
	ID			m_battletargetid;
	int			m_secbattletime;
	int			m_SpotSearchCount;
	int m_verCGPK = 0;		// 클라이언트의 현재 프로토콜 버전
	void Init() {
		m_Reconnected = FALSE;
		m_idLastConnectSvr = 0;
		m_SpotSearchCount = 0;
		m_battletargetid = 0;
		m_secbattletime = 0;
	}
	void Destroy();
	void IncreaseSpotSearch(){ m_SpotSearchCount++; }
	void InitSpotSearch(){ m_SpotSearchCount = 0; }

	std::list<XPushNotifyObj*>	m_PushNotifyList;
public:
	XSAccount();
	XSAccount( ID idAccount );
	XSAccount( ID idAccount, LPCTSTR szID );
// 	XSAccount( const _tstring& strUUID ) : XAccount( strUUID ) {}
	XSAccount( ID idAcc, const _tstring& strUUID ) : XAccount( idAcc, strUUID ) {}
	virtual ~XSAccount() { Destroy(); }
	
	GET_SET_ACCESSOR( BOOL, Reconnected );
//	GET_TSTRING_ACCESSOR( strUUID );		// LPCTSTR GetstrUUID();	SetstrUUID( LPCTSTR );
	GET_ACCESSOR_CONST( int, idLastConnectSvr );
	GET_SET_ACCESSOR_CONST(ID, battletargetid);
	GET_SET_ACCESSOR_CONST(int, secbattletime);
	SET_ACCESSOR( int, verCGPK );
	int GetverCGPK() override {
		if( m_verCGPK == 0 )
			return VER_CGPK;	// DB저장같은거 할때.
		return m_verCGPK;
	}
	int CalculateLadderPoint( int point , bool bWin);
	bool IsExistUserSpot( ID idacc);
	inline bool CheckSpotSearchAble() {
		if (m_SpotSearchCount > XGAME::MAX_SEARCH_COUNT) {
			InitSpotSearch();
			return false;
		}			
		IncreaseSpotSearch();
		return true;
	}
	//Contents
#ifdef _GAME_SERVER
	BOOL NewAccount( );
	void CreateDefaultAccount( void );
//	void InitializeTimer();
#endif // _GAME_SERVER

	//void ChangePostStatus(int idx , XPostInfo::POST_STATUS e);

//	void DeletePostInfo(ID postidx);
//	BOOL PostItemReceive(ID postidx, int itempos, int itempropid);
//	BOOL PostItemReceiveAll(ID postidx); //수령하지 않은 모든 아이템 수령.
// 	void SerializePostUpdate(XArchive& ar);
// 	void DeSerializePostUpdate(XArchive& ar);

	void AsyncPushNotify();
	void ClearPushNotify();

	bool AddPushNotify(int type, DWORD sendtime, DWORD sendendtime, _tstring strMessage, XArchive& ar);
	bool DelPushNotify(ID snNum, int type);
	ID	FindPushNotify(int type);

	bool SerializePushNotify(XArchive& ar);
	bool DeSerializePushNotify(XArchive& ar);

	void Save( );
	void Load( );

	//
	virtual int Serialize( XArchive& ar ) override;
	virtual int DeSerialize( XArchive& ar ) override;
#if defined(_DB_SERVER) || defined(_LOGIN_SERVER)
	friend class CUserDB;
	friend class XDatabase;

#endif
};


