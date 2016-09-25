#include "stdafx.h"
#include "XClientMain.h"
#include "XApp.h"
#include "XEContent.h"
#include "etc/Timer.h"
#include "xLib.h"
#include "etc/XSurface.h"
#include "etc/xGraphics.h"
#include "XFontMng.h"
#include "XSystem.h"
// Graphics include
#ifdef _VER_OPENGL
	#include "OpenGL2/XGraphicsOpenGL.h"
#else 
	#include "_DirectX/XGraphicsD3DTool.h"
#endif
// platform include
#ifdef WIN32
	#include "etc/InputMng.h"
	#include "sound/windows/OpenAL/XSoundMngOpenAL.h"
#elif defined(_VER_IOS)
		#include "InputMngiPhone.h"
	#include "XSoundMngOpenAL.h"
#elif defined(_VER_ANDROID)
		#include "etc/InputMng.h"
		#include "Sound/Android/XSoundMngAndroid.h"
	#include "Sound/Android/XSoundMngAndroid2.h"
	#include "XFramework/android/com_mtricks_xe_Cocos2dxHelper.h"
	#include "XFramework/android/JniHelper.h"
	#include "android_xe/CrashHandler.h"
#endif
#ifdef _XFTGL
#include "FTGL/ftgles.h"
#endif
#include "sprite/SprObj.h"
#include "sprite/SprMng.h"
#include "etc/xLang.h"
#include "XFont.h"
#include "XImage.h"
#include "XImageMng.h"
#include "XFramework/XSoundTable.h"
#include "XFramework/XFacebook.h"
#include "XFramework/XReceiverCallback.h"
#include "XHSLMap.h"
#include "XFramework/XEProfile.h"
#include "OpenGL2/XBatchRenderer.h"
#include "OpenGL2/XTextureAtlas.h"

//#define _SOUND_TEST
//#define _BACK_TEST

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XELibrary::ConsoleMessage( LPCTSTR szMsg )
{
//	XLibrary::ConsoleMessage( szMsg );		
	// 서버와 클라간의 코드공유로인한 복잡도를 줄이기위해 따로 씀.
//	__xLog2( XLOGTYPE_LOG, szMsg );
	//
//	CONSOLE( "%s", szMsg );
#ifdef WIN32
	// Dlgconsole에 추가로 로그를 출력한다.
	m_pMain->ConsoleMessage( szMsg );
#endif
}

XFps XClientMain::s_fps;

//////////////////////////////////////////////////////////////////////////
void XClientMain::Destroy()
{
	XTRACE("XClientMain::Destroy\n");
// #ifdef _CHEAT
// 	SaveCheat();		// 치트모드에서는 종료할때 치트정보를 저장한다.
// #endif
// 	if( m_pGame )
// 		m_pGame->Release();
	SAFE_DELETE( m_pGame );
//	SAFE_DELETE( m_pfoSystem );
//	SAFE_RELEASE2( FONTMNG, m_pfdSystem );
	SAFE_DELETE( FONTMNG );
#ifdef _XFTGL
	xFTGL::DestroyShader();
#endif
	SAFE_DELETE( SPRMNG );
	SAFE_DELETE( IMAGE_MNG );
	SAFE_DELETE( SOUND_TBL );
	SAFE_DELETE( SOUNDMNG );
	SAFE_DELETE( INPUTMNG );
	SAFE_DELETE( GRAPHICS );
	SAFE_DELETE( XLIB );
	XTRACE( "--XClientMain::Destroy\n" );
}

XClientMain::XClientMain()
{
		XE::SetApp( this );
//	__xLog( XLOGTYPE_LOG, _T("ClientMain:0x%08x"), (DWORD)this );
	Init();
#ifdef WIN32
	m_idThreadMain = ::GetCurrentThreadId();
#endif // WIN32
	__xLog( XLOGTYPE_LOG, _T("create APP") );
//	XE::g_Lang = XE::xLANG_KOREAN;
#ifdef _VER_ANDROID
	// 크래쉬 핸들러
	__xLog( XLOGTYPE_LOG, _T("call AddSignalHandlers") );
//	XLOG("%s","call AddSignalHandlers");
	XE::AddSignalHandlers();
#endif
	SetAlertHandler( this );
}

void XClientMain::OnDestroy()
{
#ifdef _CHEAT
	SaveCheat();		// 치트모드에서는 종료할때 치트정보를 저장한다.
#endif
	if( m_pGame )
		m_pGame->Release();
}

#ifdef WIN32
bool XClientMain::IsThreadMain() const 
{
	const ID idThread = ::GetCurrentThreadId();
	return m_idThreadMain == idThread;
}
#endif // WIN32

XLibrary* XClientMain::CreateXLibrary( void ) 
{
	return new XELibrary( this );
}

void XClientMain::Create( XE::xtDevice device,
						 int wPhy, int hPhy,
						 float wLog, float hLog )
{
	XInputMng::s_Device = device;
	XLIB = CreateXLibrary();

	//xLT_PACKAGE_ONLY; 루아파일같은거 땜에 사실상 패키지 온리를 못쓰기땜에 일단 패키지 카피를 디폴트로 함. 해결책이 필요함.
	/*
	win32개발에서는 패키지온리로 해서 개발하면 되지만 기기테스트시에는 루아/png/폰트 등등의 파일같은거 땜에 
	패키지온리로 할수가 없다. 워킹폴더로 한번 카피해서 써야 하는데 패치서버가 붙지 않는이상
	xLT_WORK_TO_PACKAGE_COPY 는 버전관리가 안되서 무용지물이다. 해결방법 필요함
	1.개발시에는 약간 느려도 상관없으므로 워크폴더와 패키지의 파일을 상호 비교하는 방법
		cks파일이 없으면 
			워크폴더파일의 크기와 변경날짜/시간을 같은파일명_확장자.cks파일에 써둔다.
		있으면
			cks의 정보와 apk내 파일의 정보와 비교해서 달라졌다면
			워크폴더로 다시 카피해주고 cks를 갱신한다.
		문제는 apk로 파일이 들어갈때 변경시간이 변하는지가 관건..

	*/
#ifdef WIN32
	XE::xtLoadType typeLoad = XE::xLT_PACKAGE_ONLY;
#else
	XE::xtLoadType typeLoad = XE::xLT_WORK_TO_PACKAGE_COPY; 
#endif
	for( int i = 0; i < XE::xLT_MAX; ++i ) {
		if( OnSelectLoadType( ( XE::xtLoadType ) i ) ) {
			typeLoad = ( XE::xtLoadType ) i;
			break;
		}
	}
	XE::InitResourceManager( this, typeLoad );
	//
	XTRACE("Load lang.txt");
	XE::LANG.LoadINI( XE::MakePath( _T(""), _T("lang.txt") ) );
	XTRACE("Load lang.txt 2");

#ifndef WIN32
// 	const _tstring strModel = XSYSTEM::GetDeviceModel();
// 	XTRACE("device:[%s]", strModel.c_str() );
#endif // not WIN32
#ifdef _VER_ANDROID
	XTRACE("XE::SetLogicalGameSize");
	XE::SetLogicalGameSize( wLog, hLog );
//	int scrh = (int)(XE::GetGameWi`dth() * ((float)height / width));		// 물리적 해상도 비율에 맞춰 논리적 세로해상도를 자동으로 계산한다.
	XE::VEC2 sizeLogReso = sCalcLogicalResolution( XE::VEC2(wPhy, hPhy), XGAME_SIZE );
	XGraphicsOpenGL *pGraphicsGL = new XGraphicsOpenGL( sizeLogReso.w, sizeLogReso.h, xPIXELFORMAT_RGB565 );
//	XGraphicsOpenGL *pGraphicsGL = new XGraphicsOpenGL( XE::GetGameWidth(), scrh, xPIXELFORMAT_RGB565 );
	pGraphicsGL->SetPhyScreenSize( wPhy, hPhy );
	XTRACE("pGraphicsGL->RestoreDevice();");
	pGraphicsGL->RestoreDevice();
	GRAPHICS = pGraphicsGL;
		XLOGXN("created graphics engine. physize(%dx%d), logsize(%dx%d), logsize(%dx%d)",
			 wPhy, hPhy,
			 (int)sizeLogReso.w, (int)sizeLogReso.h,
			 (int)XE::GetGameWidth(), (int)XE::GetGameHeight() );
#endif
#ifdef _XDYNA_RESO
	{
//		int scrh = (int)(XE::GetGameWidth() * ((float)height / width));		// 물리적 해상도 비율에 맞춰 논리적 세로해상도를 자동으로 계산한다.
//		if( scrh > XE::GetGameHeight() )
		XE::VEC2 sizeLog = GRAPHICS->GetLogicalScreenSize();
		// 물리적해상도 비율에 맞춰 계산된 논리적해상도의 세로사이즈가 게임사이즈보다 크면
		if( (int)sizeLog.h > XGAME_SIZE.h )
		{
			// 세로가 긴 형태
			XE::VEC2 vScrLT;
			vScrLT.y = (float)((sizeLog.h - XE::GetGameHeight()) / 2);		// 남는 영역의 절반만큼을 내려서 스크린 상단좌표로 한다.
			GRAPHICS->SetvScreenLT( vScrLT );
		} else
//		if( scrh < XE::GetGameHeight() )
		if( (int)sizeLog.w > XGAME_SIZE.w )
		{
#ifndef _XMODE_TOP_CUT
			// 논리적해상도 비율보다 세로비율이 짧은 해상도의 경우(정사각형 형태)
			// 가로가 넓은 형태
			XE::VEC2 vScrLT;
			vScrLT.x = (float)((sizeLog.w - XE::GetGameWidth()) / 2);		// 남는 영역의 절반만큼을 밀어서 스크린 상단좌표로 한다.
			GRAPHICS->SetvScreenLT( vScrLT );
#endif // not _XMODE_TOP_CUT
		}
	}
#endif // _XDYNA_RESO
//	XSurface::SetMaxSurfaceWidth( 2048 );	// 나중에 제대로된 값읽어서 세팅하도록 바꿀것
//    XLOGXN("max surface size: %d", XSurface::GetMaxSurfaceWidth() );
		//
		XLOGXN("device:%s", XE::GetDeviceString(device));
#ifdef WIN32
	INPUTMNG = new XInputMng;
		XLOGXN("created XInputMng.");
#endif
#ifdef _VER_ANDROID
	INPUTMNG = new XInputMng;
		XLOGXN("created XInputMng.");
#endif
#ifdef _VER_IOS
	XInputMngiPhone *pInputMng = NULL;
	if( device == XE::DEVICE_IPAD )
		{
		pInputMng = new XInputMngiPad;
				XLOGXN("created XInputMngiPad.");
		}
	else
		{
		pInputMng = new XInputMngiPhone;
				XLOGXN("created XInputMngiPhone.");
		}
	INPUTMNG = pInputMng;
	INPUTMNG_IPHONE = pInputMng;
#endif		// 워킹 폴더를 세팅한다.
		//
#if defined(_VER_IOS) || defined(WIN32)
	SOUNDMNG = new XSoundMngOpenAL;
#else
		SOUNDMNG = new XSoundMngAndroid;
#endif
		XLOGXN("created soundmng");
	//
	SOUND_TBL = new XSoundTable;
	SOUND_TBL->Load( _T("sound.txt") );
	//
	auto seedNew = xGenerateSeed();
	xSRand( seedNew );
	srand( seedNew );
// 	xSRand( timeGetTime() );
// 	srand( timeGetTime() );
	CTimer::Initialize();		// pause가능한 가상타이머를 초기화한다

	IMAGE_MNG = new XImageMng();
	SPRMNG = new XSprMng;
	SPRMNG->OnCreate();
	FONTMNG = CreateFontMng();		// virtual
		if( FONTMNG )
				XLOGXN("creat fontmng.....success");
		else
				XLOGXN("creat fontmng.....failed");
		// 여기서부터 리소스를 읽기 시작하므로 패치 클라이언트가 붙으려면 이곳에서 붙어야 한다.
	// text_ko같은것도 앞으로 이쪽으로 옮겨올 예정이므로 그것도 고려해야함.
		DidFinishInitEngine();
#ifdef _VER_OPENGL
	XLOGXN("xFTGL ver: v%d", xFTGL::GetVersion());
	xFTGL::CreateShader();
#endif 
	// 있으면 읽고 없어도 무방.
	XHSLMap::sGet()->Load( _T("hslmap.txt") );
#ifdef _BACK_TEST
#else
	//
		XLOGXN("create Game.....");
	m_pGame = CreateGame();				// virtual 게임 생성
	if( m_pGame == nullptr ) {
		XLOGXN( "create game failed" );
	}
#ifdef _CHEAT
	LoadCheat();		// XGC->객체에 로딩해야해서 이리로 옮김
#endif
	XBREAK( m_pGame->GetID() != 0 );
	m_pGame->SetID( XE::GenerateID() );
	XTextureAtlas::XAutoPushObj spAuto( m_pGame->GetspAtlas() );
	m_pGame->OnCreate();
	m_pGame->DidFinishCreated();
#endif
		
	DidFinishCreate();		// this의 Create가 끝난후 다른 Create가 있다면 하위클래스에 맡긴다.
	GRAPHICS->ClearScreen( XCOLOR_BLACK );	// 최초 한번은 화면 지워줌
	
}

// 물리적 화면사이즈와 사용할 게임화면의 크기를 넘겨주면 가로나 세로폭이 고정된
// 논리적 해상도를 계산해서 돌려준다.
XE::VEC2 XClientMain::sCalcLogicalResolution( const XE::VEC2& sizePhy, 
											const XE::VEC2& sizeGame )
{
	XE::VEC2 sizeLog;
	// 기준해상도를 물리적크기의 비율에 맞춰서 조절해본다.
	int scrh = (int)(sizeGame.w * (sizePhy.h/ sizePhy.w));
	if( scrh > sizeGame.h )
	{
		// 조절해봤더니 세로해상도가 길어서 세로 논리해상도를 길게 해준다.
		sizeLog.w = sizeGame.w;
		sizeLog.h = (float)scrh;
	} else
	if( scrh < sizeGame.h )
	{
#ifdef _XMODE_TOP_CUT
		sizeLog.w = XE::GetGameWidth();
		sizeLog.h = (float)scrh;
#else
		// 조절해봤더니 세로해상도가 짧아서 좌우에 공백을 넣고 가로 논리해상도를 넓게 해준다.
		// 가로 논리해상도를 다시 계산한다.
		int scrw = (int)(sizeGame.h * ((float)sizePhy.w / sizePhy.h));
		sizeLog.w = (float)scrw;
		sizeLog.h = sizeGame.h;
#endif
	} else
	{
		sizeLog = sizeGame;
	}
	return sizeLog;
}


void XClientMain::RestoreDevice( void )
{
	XLOGXN("restore start");
#ifndef _VER_DX					// 홈버튼 테스트를 위해 넣음.
	GRAPHICS->RestoreDevice();
	//
	XLOGXN("restore font");
	FONTMNG->RestoreDevice();
	XLOGXN("restore image");
	IMAGE_MNG->RestoreDevice();
	// WIN32쪽은 sprite restore를 아직 구현하지 않아서 뺌
	XLOGXN("restore sprite");
	SPRMNG->RestoreDevice();
	XLOGXN("restore game");
	if( m_pGame )
		m_pGame->RestoreDevice();
#endif // not dx
	XLOGXN("restore end");
	//
	m_Restore = xRST_NONE;	// 리스토어 완료
}

static DWORD dwPrevTime = 0;
void XClientMain::ClearDT()
{
	dwPrevTime = GetTickCount();
}

float XClientMain::CalcDT( void )
{
	if( dwPrevTime == 0 )
		dwPrevTime = GetTickCount();
	DWORD dwTime = GetTickCount();
// 	static DWORD dwPrevTime = CTimer::sGetTime();
// 	const DWORD dwTime = CTimer::sGetTime();;
	DWORD delta = dwTime - dwPrevTime;
	dwPrevTime = dwTime;
	float dt = (float)delta / ((float)TICKS_PER_SEC/60.0f);
	// pause
	{
#ifdef _DEBUG
		if( m_bFrameSkip )
			dt *= m_fAccel;
		else
			dt = 1.0f * m_fAccel;				// 디버깅중엔 프레임 스키핑 적용안함.
#else
			dt *= m_fAccel;				// 프레임스키핑 적용
#endif
	}
		
	if( m_bPause )
		dt = 0;
#ifdef _CHEAT
	if( m_bNextFrame ) {
		dt = 1.0f;
		m_bNextFrame = FALSE;
	}
#endif
	if( m_bFrameSkipReset ) {
		// 리셋명령이 예약되어있으면 이번프레임은 dt를 0으로 보내고 담프레임부터 정상dt를 리턴한다.
		m_bFrameSkipReset = false;
		dt = 0;
	}

	return dt;
}

void XClientMain::StopAnimation( void )
{
	// 갱신을 멈춘다.
	m_bFrameMove = FALSE;
	m_bDraw = FALSE;
}

void XClientMain::StartAnimatiom( void )
{
	m_bFrameMove = TRUE;
	m_bDraw = TRUE;
}

void XClientMain::FrameMove( void )
{
	XPROF_OBJ_AUTO();
	float dt = CalcDT();
	if( dt > 60.f )
		dt = 1.f;			// 브레이크 걸었을때 지나치게 오래 잡고 있어서 갑자기 뛰는걸 방지하려고.
	CTimer::UpdateTimer( dt );
	//
// 	if( m_Restore == xRST_TRUE )	{
// 		//리스토어를 먼저하고 OnResume()이 호출됨.
// 		RestoreDevice();
// 	} else
	if( m_Restore == xRST_FALSE )	// 디바이스 자원 잃은 상태에선 더이상 진행 안함.
		return;
	if( m_bFrameMove == FALSE )
		return;
	//
	do 	{
		if( m_pGame )	{
			if( m_bResume )	{
				// resume이 블로킹 되어있으면 실행하지 않는다. 페이스북 인증같은거 할때 사용.
//				if( XE::GetbResumeBlock() == FALSE )	
				m_pGame->OnResume();
				RestoreDevice();
				CONSOLE( "dt: %.3f", dt );
				m_bResume = FALSE;
			}
			// 인증결과가 담겨돌아왔다.
			if( m_pResultAuthen )	{
				if( XASSERT(XFacebook::sGetpDelegate()) )	{
					XLOGXNA("DelegateFacebookCertResult: id=%s, username=%s", m_pResultAuthen->string[0].c_str(), m_pResultAuthen->string[1].c_str() );
					XFacebook::sGetpDelegate()->DelegateFacebookCertResult( 
											m_pResultAuthen->string[0].c_str(),
											m_pResultAuthen->string[1].c_str(),
											m_pResultAuthen->dwValue[0] );
//					XE::SetbResumeBlock( FALSE );	// OnResume()이 다시 동작하도록 한다.
				}
				SAFE_DELETE( m_pResultAuthen );		// 쓰고나서 바로 지움.
			}
			if( XFacebook::sIsActivated() )
				XFacebook::sGet()->Process();
			XReceiverCallback::sGet()->Process();
			OnDelegateFrameMove( dt );	// m_pGame->Process( dt );
			SOUNDMNG->Process( dt );
		}
	} while(0);
	//
// 	CTimer::UpdateTimer( dt );
}


void XClientMain::OnDelegateFrameMove( float dt ) 
{
	m_pGame->Process( dt );
}

void XClientMain::Draw( void )
{
	XPROF_OBJ_AUTO();
	XSurface::sClearCntDPCall();
	if( m_Restore == xRST_FALSE ) {	// 디바이스 자원 잃은 상태에선 더이상 진행 하지 않는다.
#ifdef WIN32
		GRAPHICS->ClearScreen( XCOLOR_RGBA( 0, 0, 0, 255 ) );
#endif // WIN32
		return;
	}
#ifdef _XMODE_TOP_CUT
	{
/*		XE::VEC2 sizeProj;
		XE::VEC2 sizePhy = GRAPHICS->GetPhyScreenSize();
		float ratioPhy = GRAPHICS->GetPhyScreenSize().h / GRAPHICS->GetPhyScreenSize().w;
		sizeProj.w = sizePhy.w * (XE::GetGameWidth() / sizePhy.w);
		sizeProj.h = sizeProj.w * ratioPhy;
		XE::SetProjection( sizeProj.w, sizeProj.h ); */
		GRAPHICS->SetViewport( XE::VEC2(0), GRAPHICS->GetLogicalScreenSize() );
		XE::SetProjection( GRAPHICS->GetLogicalScreenSize().w, GRAPHICS->GetLogicalScreenSize().h );
	}
#else
	GRAPHICS->SetViewport( XE::VEC2(0), XGAME_SIZE );
	XE::SetProjection( XE::GetGameWidth(), XE::GetGameHeight() );
#endif
	if( m_bDraw == FALSE ) {
		return;
	}
#if defined(_VER_ANDROID) || (defined(WIN32) && defined(_VER_OPENGL))
	GRAPHICS->ClearScreen( XCOLOR_RGBA( 0, 0, 0, 255 ) );
#endif
	if( m_pGame ) {
// 		SET_RENDERER( m_pGame->GetpRenderer() ) {
		m_pGame->OnDrawBefore();
		m_pGame->Draw();
		m_pGame->OnDrawAfter();
// 		} END_RENDERER;		// RenderBatch();
		// 큐에 쌓인 모든 배치렌더러를 한꺼번에 렌더링 한다.
//		XBatchRenderer::sRenderBatchs();
	}
	//
// 	{
// 		XPROF_OBJ( "render_batch" );
// 		XRenderCmdMng::sGet()->RenderBatch();	// 레이어방식으로 해서 레이어별로 가지고 있어야 할듯.
// 	}
	//
	{
		XPROF_OBJ( "draw dbginfo" );
		DrawDebugInfo( 96.f, 15.f );
	}
#ifdef _XPROFILE
	// 프로파일링 결과를 받는다.
	if( m_pGame )	{	// 프로파일링 끝을 명령받았다면 프로파일 결과를 만들고 초기화를 시킨다.
		if( XEProfile::sIsFinish() ) {
			XEProfile::sGet()->DoEvaluation();
			m_pGame->OnFinishProfiling( XEProfile::sGetResult() );
		}
		if( XEProfile::sIsActive() ) {
			XEProfile::sGet()->ClearDepth();
		}
	}
#endif
	XGraphicsOpenGL::s_numCallBindTexture = 0;
}

void XClientMain::DrawDebugInfo( float x, float y )
{
	XPROF_OBJ_AUTO();
	XE::VEC2 v( x, y );

#ifdef _CHEAT
	XE::VEC2 vMouse = INPUTMNG->GetMousePos();
	_tstring str;
//	const int fps = CalcFPS();
	s_fps.Process();
	const int fps = s_fps.GetFps();
	if( m_bViewFrameRate ) {	// 이건 치트모드 안해도 보임.
		str += XE::Format( _T( "fps:%d  " ), fps );
	}

	if( m_bDebugDrawArea && m_vTouchStart.IsValid() && m_vTouchCurr.IsValid() ) {
		GRAPHICS->DrawRect( m_vTouchStart, m_vTouchCurr, XCOLOR_WHITE );	// 마우스 우클릭 블럭 사각형
		XE::VEC2 vSize = m_vTouchCurr - m_vTouchStart;
		str += XE::Format( _T( "mouse:%d,%d(%dx%d)  " ), (int)vMouse.x, (int)vMouse.y,
																										(int)vSize.w, (int)vSize.h );
	} else {
		if( m_bDebugMode ) {
			str += XE::Format( _T( "mouse:%d,%d  " ), (int)vMouse.x, (int)vMouse.y );
		}
	}
	if( m_bDebugMode ) {
#ifdef WIN32
		if( XWnd::s_pMouseOver ) {
			XWnd::s_pMouseOver->GetDebugString( str );
		}
#else
		if( INPUTMNG->GetState() == XInputMng::xTS_TOUCHED 
			&& INPUTMNG->GettimerPush().IsOver( 1.f ) ) {
			vMouse.y -= 50.f;
			GRAPHICS->DrawHLine( v.x, v.y + vMouse.y, XE::GetGameWidth(), XCOLOR_WHITE );
			GRAPHICS->DrawVLine( v.x + vMouse.x, v.y, XE::GetGameHeight(), XCOLOR_WHITE );
		}
#endif
	}
	if( !str.empty() ) {
		PUT_STRING_STYLE( v.x, v.y, XCOLOR_WHITE, xFONT::xSTYLE_STROKE, str.c_str() );
		if( XGraphics::s_dwDraw & XE::xeBitNoFont ) {
			static auto fpsPrev = fps;
			if( fpsPrev != fps )
				CONSOLE("%s", str.c_str());
			fpsPrev = fps;
		}
	}
	if( m_bDebugMode )
		if( m_pGame )
			m_pGame->DrawDebugInfo( x, y, XCOLOR_WHITE, BASE_FONT );
#endif // cheat
}

// Alert의 확인을 누르면 들어온다.
void XClientMain::OnAlert( void )
{
	if( m_bExit )
		exit(1);
}

// exit를 예약함.
void XClientMain::DoExit( void )
{
	m_bExit = TRUE;
}

#ifdef _CHEAT
void XClientMain::SaveCheat( void )
{
	FILE *fp;
	fopen_s( &fp, XE::MakeDocFullPath( "", "cheat.ini" ), "wt" );
	if( fp == NULL )
	{
		CONSOLE( "failed cheat info file." );
		return;
	}
	fprintf_s( fp, "debug_mode = %d\r\n", (int)m_bDebugMode );
	fprintf_s( fp, "frameskip = %d\r\n", (int)m_bFrameSkip );
	SaveCheat( fp );
	fclose( fp );
}
void XClientMain::LoadCheat( void )
{
	CToken token;
	if( token.LoadFromDoc( _T("cheat.ini"), XE::TXT_EUCKR ) == xFAIL )
		return;
	while(1) {
		if(  token.GetToken() == NULL )	break;
		if( token == _T("debug_mode") )	{
			token.GetToken();	// =
			m_bDebugMode = token.GetNumber();
		} else
		if( token == _T("frameskip") ) {
			token.GetToken();
			m_bFrameSkip = token.GetNumber();
		}
		LoadCheat( token );
	}
}

#endif // cheat

void XClientMain::OnCheatMode( void )
{
#ifdef _CHEAT
	m_bDebugMode = !m_bDebugMode;
	if( GetpGame() ) {
		GetpGame()->OnCheatMode();
		GetpGame()->SetbUpdate( true );
	}
	SaveCheat();
#endif
}

XFontMng* XClientMain::CreateFontMng( void )
{
		return new XFontMng;
}

// 치트모드 인식을 위한.
#ifdef _CHEAT
static XArrayN<bool, 4> s_aryCheatTouch;
static int s_idxCheatTouch = 0;
#endif
// 터치이벤트 통합.
void XClientMain::OnTouchEvent( xtTouchEvent event, int cntTouches, float x, float y, float scale/*=0*/ )
{
	if( m_Restore != xRST_NONE )
		return;
//	CONSOLE("XClientMain::OnTouchEvent");
	XE::VEC2 vRatio = GRAPHICS->GetLogicalScreenSize() / GRAPHICS->GetPhyScreenSize();
	XE::VEC2 vTouch;
	vTouch.x = x * vRatio.x;
	vTouch.y = y * vRatio.y;
	vTouch -= GRAPHICS->GetvScreenLT();
	INPUTMNG->SetMousePos( vTouch );
	INPUTMNG->SetcntTouches( cntTouches );
	switch( event )
	{
	//////////////////////////////////////////////////////////////////////////
	case xTE_DOWN:
		#if defined(_VER_ANDROID) && defined(_CHEAT)
		m_PushState = 1;
		#endif
		if( m_pGame )
			m_pGame->OnLButtonDown( vTouch.x, vTouch.y );
		break;
		//
	//////////////////////////////////////////////////////////////////////////
	case xTE_MOVE:
		#if defined(_VER_ANDROID) && defined(_CHEAT)
		if( cntTouches >= 2 )	{
			if( m_PushState == 1 ){
//				OnCheatMode();
				m_PushState = 2;    // -_-;;;
			}
		}
		#endif
#ifdef _CHEAT
//		if( m_bDebugDrawArea )
			m_vTouchCurr = XE::VEC2( vTouch.x, vTouch.y );
#endif
		if( m_pGame ) {
			m_pGame->OnMouseMove( vTouch.x, vTouch.y );
#if defined(_CHEAT) && defined(WIN32)
		// 마우스 오버된 모든윈도우를 찾는다.
		int depth = 0;
		XWnd::s_aryMouseOver.clear();
		m_pGame->UpdateMouseOverWins( vTouch, depth, &XWnd::s_aryMouseOver );
#endif // defined(_CHEAT) && defined(WIN32)
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	case xTE_UP: {
		#ifdef _CHEAT
		#ifdef _VER_ANDROID
			m_PushState = 0;
		#else
// 			if( cntTouches >= 2 )	// 치트모드 전환방법이 바뀜.
// 				OnCheatMode();
		#endif
		#endif

		XE::VEC2 vSize = XE::GetGameSize();
		int idTouch = 0;	// 1:LT  2:RT  3:LB  4:RB
		if( vTouch.x < 64.f ) {
			// 1 or 3;
			if( vTouch.y < 64.f )
				idTouch = 1;
			else if( vTouch.y > vSize.h - 64 )
				idTouch = 3;
		} else 
		if( vTouch.x > vSize.w - 64.f ) {
			// 2 or 4
			if( vTouch.y < 64.f )
				idTouch = 2;
			else if( vTouch.y > vSize.h - 64 )
				idTouch = 4;
		}
		// LT찍을 차롄데 아니면 실패
		if( (s_idxCheatTouch == 0 && idTouch != 1) ||
			(s_idxCheatTouch == 1 && idTouch != 2) ||
			(s_idxCheatTouch == 2 && idTouch != 3) ||
			(s_idxCheatTouch == 3 && idTouch != 4) )
			s_idxCheatTouch = 0;
		else {
			++s_idxCheatTouch;
			CONSOLE("idxCheatTouch:%d", s_idxCheatTouch );
			if( s_idxCheatTouch >= 4 ) {
				s_idxCheatTouch = 0;
				// 치트모드 토글.
				OnCheatMode();
			}
		}
		if( m_pGame ) {
			m_pGame->OnLButtonUp( vTouch.x, vTouch.y );
		}
	} break;
#ifdef WIN32
	//////////////////////////////////////////////////////////////////////////
	case xTE_RDOWN:
		m_vTouchStart = XE::VEC2( vTouch.x, vTouch.y );
		m_vTouchCurr = m_vTouchStart;
		m_bDebugDrawArea = TRUE;
		if( m_pGame )
			m_pGame->OnRButtonDown( vTouch.x, vTouch.y );
		break;
	//////////////////////////////////////////////////////////////////////////
	case xTE_RUP:
		m_bDebugDrawArea = FALSE;
		m_vTouchStart.Set(0);
		m_vTouchCurr.Set(0);
		if( m_pGame )
			m_pGame->OnRButtonUp( vTouch.x, vTouch.y );
#endif
		break;
	//////////////////////////////////////////////////////////////////////////
	case xTE_ZOOM:	{
			if( m_pGame )	{
				m_pGame->OnZoom( scale, vTouch.x, vTouch.y );
			}
		}
		break;
	}

}

void XClientMain::OnPauseHandler( void )
{
// 	SOUNDMNG->SetbMuteBGM( true );
// 	SOUNDMNG->SetbMuteSound( true );
	SOUNDMNG->PushBGMVol( 0 );
	SOUNDMNG->PushSfxVol( 0 );
	XLOGXN("XClientMain::OnPauseHandler()");
	// 홈으로 나가기전 Pause핸들러를 한번 불러준다.
	if( m_pGame ) {
		m_pGame->DestroyDevice();
		m_pGame->OnPause();
	}
	StopAnimation();
	IMAGE_MNG->DoFlushCache();
	SPRMNG->DoFlushCache();
	FONTMNG->DoFlushCache();
	SPRMNG->OnPause();
	IMAGE_MNG->DestroyDevice();		// 디바이스 자원 날리고 클리어 함.
	XTextureAtlas::s_listSurfaceAll.clear();
	m_Restore = xRST_FALSE;		// 디바이스 자원 잃음.
// #ifdef WIN32
// 	SPRMNG->OnPauseForWin32();
// 	IMAGE_MNG->OnPauseByWin32();
// #endif // WIN32
}

void XClientMain::OnResumeHandler( void )
{
	XLOGXN("XClientMain::OnResumeHandler()");
	// 애니메이션 갱신을 재개한다.
	StartAnimatiom();
	// 플래그만 세팅하고 메인루프에서 Resume핸들러를 호출한다.
	m_bResume = TRUE;
	SOUNDMNG->PopBGMVol();
	SOUNDMNG->PopSfxVol();
	// 	SOUNDMNG->SetbMuteBGM( false );
// 	SOUNDMNG->SetbMuteSound( false );
}

void XClientMain::OnKeyDown( int keyCode )
{
	if( m_pGame )
		m_pGame->OnKeyDown( keyCode );
}

void XClientMain::OnKeyUp( int keyCode )
{
	if( m_pGame )
		m_pGame->OnKeyUp( keyCode );
}

XBaseFontDat* XClientMain::GetSystemFontDat( void ) 
{
	if( m_pGame )
		return m_pGame->GetpfdSystem();
	return NULL;
}
XBaseFontObj* XClientMain::GetSystemFontObj( void ) 
{
	if( m_pGame )
		return m_pGame->GetpfoSystem();
	return NULL;
}


