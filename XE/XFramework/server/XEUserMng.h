#pragma once
#include "XFramework/server/XEUser.h"
#include "XList.h"

class XEWinSocketSvr;
class XEUserMng
{
public:
	static void sResetSecAutoSave( float sec )	{
		s_bResetSaveTimer = 1;
		if( XASSERT(sec == 0 || sec > 30.f) )
			s_secAutoSave = sec;
	}
	static float sGetsecAutoSave() {
		return s_secAutoSave;
	}
private:
	static int s_bResetSaveTimer;
	static float s_secAutoSave;			// 자동세이브 주기
private:
	std::map<ID, XSPUserBase> m_mapUser;			// 유저 계정아이디를 기반으로 맵을 만든다.
	XEWinSocketSvr *m_pParentSocketSvr = nullptr;
	void Init() {}
	void _Destroy() {}
//	void _AddDestroyUser( XUser *pUser );
protected:
	XList4<XSPUserBase> m_listUser;
public:
	XEUserMng( XEWinSocketSvr *pParent, int maxUser );
	virtual ~XEUserMng() { _Destroy(); }	
	//
	GET_ACCESSOR( const XList4<XSPUserBase>&, listUser );
	GET_ACCESSOR( XEWinSocketSvr*, pParentSocketSvr );

	XSPUserBase GetspUserFromidAcc( ID idAccount );
	int GetnumUser() const {
		return m_listUser.size();
	}
// 	void Flush();
	void Process( float dt );
	void Save();
	//
	virtual void OnDestroyUser( XSPUserBase spUser ) {}
private:
	void DelUser( XSPUserBase spUser );
	bool Add( const XSPUserBase spUser );
//	void ResetAutoSaveTimer( float sec );
//
friend class XEWinSocketSvr;
};
