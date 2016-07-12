#include "stdafx.h"
#include "XFramework/XFacebook.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/client/XEContent.h"
#ifdef _VER_ANDROID
#include "XFramework/android/JniHelper.h"
#elif defined(_VER_IOS)
#include "objc/xe_ios.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef WIN32
static std::vector<std::string> s_aryFbid = {
	"557055802"
	, "100000786878603"
	, "100000786878203"
	, "100000786878605"
	, "422888354515288"
	, "100004602252270"
	, "1917811051777352"
	, "100010463058461"
	, "100000686899365"
	, "100002882537209"
	, "100000274211603"
	, "1377662711"
	, "171018822936702"
	, "171019052936679"
	, "1379861695612685"
	, "664936773579417"
	, "494164730653236"
	, "278203089013244"
	, "375691212504985"
	, "405424532872449"
	, "1489534137948675"
	, "2303430311544"
	, "10203589094112277"
	, "10206843052747901"
	, "10200388672078845"
	, "10153241046122776"
	, "304785792997554"
	, "10104882054605370"};

#define FB_PLAYER				"1097088886976908"		// 윈도우환경에서의 시뮬레이션을 위해
#endif // WIN32
// XDelegateFacebook* XFacebook::s_pDelegate = NULL;
// XFacebook* XFacebook::s_pSingleton = NULL;
// static XFacebook::XFacebookDestroyer s_FacebookDestroyer;		// 앱이종료될때 자동으로 파괴되게 했음.

std::shared_ptr<XFacebook> XFacebook::s_spInstance;
XDelegateFacebook* XFacebook::s_pDelegate = nullptr;
////////////////////////////////////////////////////////////////
std::shared_ptr<XFacebook>& XFacebook::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XFacebook>( new XFacebook );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
// 페이스북쪽으로 인증을 요청한다.
BOOL XFacebook::DoRequestCertification( XDelegateFacebook *pDelegate )
{
	XTRACE("XFacebook::DoRequestCertification");
	s_pDelegate = pDelegate;
	//
	XE::SetbResumeBlock( TRUE );	// OnResume()을 실행하지 않도록 한다.
	//	TIMEOUT_MNG->Add( this->GetID(), xTIMEOUT_FACEBOOK_REQUEST, 10.f );
	CallFacebook();		// virtual
#ifdef WIN32	
	// win32는 편의상 인증했다치고 바로 인증성공으로 보냄
	if( s_pDelegate ) {
		s_pDelegate->DelegateFacebookCertResult( FB_PLAYER, "", 0 );
	}
#endif // not win32
	return TRUE;
}

void XFacebook::CallFacebook( void )
{
#ifdef _VER_ANDROID
	JniHelper::DoLoginFacebook();
#elif defined(_VER_IOS)
	IOS::DoLoginFacebook();
#endif
}

/**
 @brief 페북 프로필 사진을 요청한다.
*/
void XFacebook::GetRequestProfileImage( const char* cUserid )
{
//	XTRACE("XFacebook::GetRequestProfileImage: fbid=%s", C2SZ(cUserid) );
	if( XBREAK( XE::IsEmpty( cUserid ) ) )
		return;
//		const std::string strcFbUserId = "1385798771667701";	// 이동하.
	const std::string strcFbUserId = cUserid;
//	const std::string strcFbUserId = "1097088886976908";
//	const std::string strcFbUserId = "278203089013244";
	// 이미 요청목록에 있는 아이디면 다시 하지 않는다.
// 	if( IsExistRequest( strcFbUserId ) )
// 		return;
	// 요청하려는 아이디를 큐에 밀어넣는다. 이는 메인 프로세스에서 하나씩 빼서 요청한다.
	m_listRequestProfileImage.push_back( strcFbUserId );
	// 결과는 XGame::OnRecvProfileImageByFacebook()로 받는다.
}

/**
 @brief 요청 클리어처리
*/
void XFacebook::OnRecvProfileImage( const std::string& strcFbUserId )
{
	auto itor = m_mapRequestingProfileImage.find( strcFbUserId );
	if( XASSERT(itor != m_mapRequestingProfileImage.end()) )
		m_mapRequestingProfileImage.erase( itor );
	// 이미지 처리 핸들링은 XGame::OnRecvProfileImageByFacebook()에서 한다.
}
/**
 @brief 프로필사진 요청중인지.
*/
bool XFacebook::IsRequestingProfileImage( const std::string& strcFbUserId ) const
{
	auto itor = m_mapRequestingProfileImage.find( strcFbUserId );
	return ( itor != m_mapRequestingProfileImage.end() );
}

/**
 @brief 이미 요청목록에 있는 아이디인지 검사.
*/
bool XFacebook::IsExistRequest( const std::string& strcFbUserId )
{
	return m_listRequestProfileImage.Findp( strcFbUserId ) != nullptr;
}

/**
 @brief 
*/
void XFacebook::Process()
{
	// 프로필사진 요청한 아이디가 있으면 요청을 보낸다.
	if( m_listRequestProfileImage.size() ) {
		// 페북으로 프로필사진 요청.
		ProcessRequestProfileImage();
	}
	// 요청한 결과가 돌아왔으면 차례로 콜백을 호출시킨다.
	{
		XLockObj lockObj( &m_lockProfileImage );
		if( m_listResultProfileImage.size() ) {
			const auto& result = m_listResultProfileImage.front();
			OnRecvProfileImage( result.m_strFbUserId );
			if( XE::GetGame() )
					XE::GetGame()->OnRecvProfileImageByFacebook( result.m_strFbUserId
																											, result.m_pImg
																											, result.m_sizeImg.w
																											, result.m_sizeImg.h
																											, result.m_bpp );
			m_listResultProfileImage.pop_front();
//			CONSOLE("11");
		}
	}
}

/**
 @brief strcFbUserId의 프로필 정보를 페북에 요청한다.
*/
void XFacebook::ProcessRequestProfileImage()
{
	// 뭔가 요청하고 기다리는 중이면 요청하지 않음.
	if( m_mapRequestingProfileImage.size() )
		return;
	// 큐에서 하나 뺌.
	const std::string strcFbUserId = m_listRequestProfileImage.front();
	m_listRequestProfileImage.pop_front();
	// 요청중 정보 등록(멀티스레드로 요청이 돌아오는거라 미리 등록함).
	m_mapRequestingProfileImage[ strcFbUserId ] = true;
	// 플램폼별로 요청을 전송함.
#if defined(WIN32)
	// 윈도우 환경에서의 페북 시뷸레이션.
	DWORD *pImg = nullptr;
	int w = 0, h = 0;
	// 장차 콜백방식으로 바꿔야 할듯.
	if( strcFbUserId == FB_PLAYER )
		GRAPHICS->LoadImg( XE::MakePath( DIR_IMG, _T( "pf_me.png" ) ), &w, &h, &pImg );
	else {
		int idx = 0;
		for( const auto& strid : s_aryFbid ) {
			if( strid == strcFbUserId )
				break;
			++idx;
		}
		idx %= 20;
		const _tstring strFile = XFORMAT( "pf_%d.png", idx + 1 );
		GRAPHICS->LoadImg( XE::MakePath( DIR_IMG, strFile.c_str() ), &w, &h, &pImg );
	}
	if( strcFbUserId == "10206843052747901" ) {
		SAFE_DELETE_ARRAY( pImg );		// 프로필사진 못읽어왔을때의 시뮬레이션.
		w = h = 0;
	}
	cbOnRecvProfileImageByFacebook( strcFbUserId, pImg, w, h, 4 );
#elif defined(_VER_ANDROID)
	JniHelper::GetUserProfileImage( strcFbUserId.c_str() );
#elif defined(_VER_IOS)
#error "implement here for iOS"
#else
#error "unknown platform"
#endif
}

void XFacebook::cbOnRecvProfileImageByFacebook( const std::string& strFbUserId, DWORD* pImage, int w, int h, int bpp )
{
// 	XBREAK( bpp != 4 );
// 	XBREAK( pImage == nullptr );
// 	XBREAK( w == 0 );
// 	XBREAK( h == 0 );
	XBREAK( strFbUserId.empty() );
	// 요청결과를 큐에 밀어넣는다.
	XLockObj lockObj( &m_lockProfileImage );
	xResultProfileImage result( strFbUserId, pImage, w, h, bpp );
	m_listResultProfileImage.push_back( result );
}

