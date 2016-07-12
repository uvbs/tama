#include "stdafx.h"
#include "XEUser.h"
//#include "XResObj.h"
#include "XServerMain.h"
#include "XEUserMng.h"
#include "XWinConnection.h"
//#include "XWinSocketSvr.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//DWORD XUser::s_idGlobal = 0;

// 유저가 최초 생성되고 나서 내부의 메모리 할당같은 초기화가 있는부분.
/*
BOOL XUser::Create()
{
	// DB객체 생성
	// 유저 계정객체 생성(계정아이디 때문에 디폴트로 계정은 생성시켜야 한다)
	// 로그인해서 계정정보가 오면 그때 채워넣는다.
	m_pDBAccount = CreateDBAccount();	// virtual
}
*/

int XEUser::s_numUsed = 0;
//////////////////////////////////////////////////////////////////////////
XEUser::XEUser( XEUserMng *pUserMng, XSPWinConnInServer spConnect ) 
{ 
	Init(); 
	XBREAK( spConnect->IsDisconnected() == TRUE );		// 소켓이 끊긴상태로 와선 안된다.
	m_spConnect = spConnect;
	m_pUserMng = pUserMng;
	const auto secSave = XEUserMng::sGetsecAutoSave();
	if( secSave > 0 )
		m_timerSave.Set( secSave );
	::InterlockedIncrement( (LONG*)( &s_numUsed) );
}

void XEUser::Destroy()
{
	::InterlockedDecrement( (LONG*)( &s_numUsed ) );
}

ID XEUser::GetidUser() const
{
	auto spDBAcc = m_spDBAccount.lock();
	if( XBREAK( spDBAcc == nullptr ) )
		return 0;
	return spDBAcc->GetidAccount();
}

void XEUser::DoDestroy() 
{
	//		DoDisconnect();
	auto spConn = m_spConnect.lock();
	spConn->SetbDestroy( true );
}

bool XEUser::IsDestroy() const
{
	auto spConn = m_spConnect.lock();
	if( XASSERT(spConn) )
		return spConn->IsbDestroy();
	return true;		// 커넥션이 없는경우는 파괴된걸로 판단.
}

void XEUser::DoDisconnect()
{
	auto spConn = m_spConnect.lock();
	spConn->DoDisconnect();
}

ID XEUser::GetidConnect() const
{
	auto spConn = m_spConnect.lock();
	return spConn->GetidConnect();
}
