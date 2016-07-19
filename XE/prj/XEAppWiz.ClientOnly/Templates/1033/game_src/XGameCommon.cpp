#include "stdafx.h"
#include "XGameCommon.h"
#include "XGlobal.h"
#include "XConstant.h"
#include "XGameLua.h"
#include "constGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XGameCommon::XGameCommon()
{
	Init();
}

void XGameCommon::Destroy()
{
	DestroyCommon();
}

void XGameCommon::Create( void )
{
}

// 클라/서버 공통 데이타 로드부를 코딩합니다.(프로퍼티 같은거)
void XGameCommon::CreateCommon( void )
{
	// 스크립트 상수
	XBREAK( CONSTANT != NULL );
	CONSTANT = new XConstant;
	CONSTANT->Load( _T("defineGame.h") );			// 상수값들 읽어서 메모리에 적재
	// 루아 생성
	m_pLua = new XGameLua("game.lua");
	m_pLua->Initialize();
	// 글로벌 상수 싱글톤 객체 생성
	XGlobalConst::sCreateSingleton( this );
	// 프로퍼티 로딩

}

void XGameCommon::DestroyCommon( void )
{
	// 전역상수 객체 삭제
	XGlobalConst::sDestroySingleton();
	//
	SAFE_DELETE( m_pLua );
	SAFE_DELETE( CONSTANT );
}

