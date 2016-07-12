#include "stdafx.h"
#include "XFramework/XEProfile.h"
#include "etc/Timer.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/**
 @brief 프로파일러 사용법
 초기화는 따로 안해도 된다.
 프로파일링을 원하는 시점에서 XEProfile::sDoStart()를 실행
 프로파일링을 끝내는 시점에서 XEProfile::sDoFinish()를 실행
 결과를 보고자 할때 XEProfile::DoEvaluation()을 실행하여 결과를 분석하고 리스트형태로 만든다.
 XEProfile::sGetResult()로 결과 어레이를 꺼내서 적당히 찍는다.
*/

XEProfile* XEProfile::PROFILE = NULL;
XArrayLinear<XEProfile::xRESULT> XEProfile::s_aryResult;

//////////////////////////////////////////////////////////////////////////
void XEProfile::sCreateSingleton( void ) 
{
	PROFILE = new XEProfile;
}

void XEProfile::sDoStart( void ) 
{
	PROFILE->SetState( xACTIVE );
	CONSOLE("profiling start");
}
// 프로파일링 끝!
void XEProfile::sDoFinish( void ) {
	CONSOLE( "profiling end" );
	PROFILE->SetState( xFINISH );
}
//////////////////////////////////////////////////////////////////////////
void XEProfile::Destroy() 
{
  for( auto pFunc : m_mapFuncs ) {
    xFUNC *p = pFunc.second;
    SAFE_DELETE( p );
  }
}

// 프로파일링 객체가 생성될때 호출해야함.
XEProfile::xFUNC* XEProfile::RegisterObj( LPCTSTR szFunc )
{
	xFUNC *pFunc = NULL;
	_tstring strFunc = szFunc;
	std::map<_tstring, xFUNC*>::iterator itor;
	itor = m_mapFuncs.find( strFunc );
	if( itor == m_mapFuncs.end() )
	{
		pFunc = new xFUNC;
		_tcscpy_s( pFunc->szFunc, szFunc );
		pFunc->cntExec = 1;			// 실행된 횟수 초기화
		m_mapFuncs[ strFunc ] = pFunc;
	//	pFunc->cntCall = ++m_cntCall;	// 최초 불려진 순서대로 콜 카운터가 기록된다.
		// 최초 만들어진 노드는 트리에 등록
		if( m_pCurrCall )
		{
			m_pCurrCall->listChild.Add( pFunc );
			pFunc->pParent = m_pCurrCall;
			pFunc->depth = m_pCurrCall->depth + 1;
		} else
		{
			XBREAK( m_pTreeCall != NULL );	// 보통 최상위 루트노드가 2개일때 발생
			m_pTreeCall = pFunc;
			pFunc->pParent = NULL;	// this is top
			pFunc->depth = 0;
		}
	} else
	{
		pFunc = (*itor).second;
		XBREAK( pFunc == NULL );
	}
	// 콜스택이 현재 함수로 바뀜
	m_pCurrCall = pFunc;
	return pFunc;
}

void XEProfile::Process()
{
	// 딜레이 후 자동시작
	if( m_timerDelayStart.IsOver() ) {
		m_timerDelayStart.Off();
		sDoStart();
		m_timerExec.Set( m_secExec );
	}
	// 일정시간 실행후 자동 종료
	if( m_timerExec.IsOver() ) {
		m_timerExec.Off();
		sDoFinish();
		DoEvaluation();
		XEProfile::sGet()->m_pDelegate->OnFinishProfile( PROFILE );
	}
}

void XEProfile::Update( xFUNC *pFunc, unsigned long long llExec )
{
	// 실행된 시간을 누적시킨다.
	pFunc->llTotalExec += llExec;
	// 실행된 횟수를 기록한다.
	++pFunc->cntExec;
	// 현재 콜스택의 깊이를 넣는다.
	//	pFunc->cntDepth = PROFILE->GetcntCurrCallStackDepth();
	// 콜스택 복귀
	m_pCurrCall = pFunc->pParent;
}


void XEProfile::AddChild( xFUNC *pFunc )
{
	if( m_pTreeCall )
	{
		m_pCurrCall->listChild.Add( pFunc );
		pFunc->pParent = m_pCurrCall;
	} else
	{
		m_pTreeCall = pFunc;
		m_pCurrCall = pFunc;
		pFunc->pParent = NULL;	// this is top
	}
}

// 이제까지 쌓인 프로파일 데이타를 평가해서 목록으로 만들어라
void XEProfile::DoEvaluation( void )
{
	if( m_pTreeCall == NULL )
		return;
	int size = m_mapFuncs.size();
	// 이전 어레이크기와 달라졌다면 재활용 못하므로 삭제하고 다시 만듬.
	if( s_aryResult.size() != size )
	{
		s_aryResult.DestroyAll();	// 
		s_aryResult.Create( size );
	}
	RecursiveEvaluation( &s_aryResult, m_pTreeCall, 0 );
	//
	m_State = xOFF;
}

BOOL XEProfile::RecursiveEvaluation( XArrayLinear<xRESULT> *pOut, 
									xFUNC *pFunc,
									unsigned long long llExecAvgParent )
{

	// 현재 노드의 평균 실행속도
	unsigned long long llExecAvg = (unsigned long long)(pFunc->llTotalExec / pFunc->cntExec);
	if( pFunc->pParent )
	{
		// 탑노드는 목록을 만들지 않는다.
		xRESULT result;
		result.strFunc = pFunc->szFunc;
		// 부모노드의 전체 실행속도에서 현재노드의 실행속도가 차지하는 비율
		result.ratioShare = (float)((double)llExecAvg / llExecAvgParent);
		result.mcsExecAvg = (int)llExecAvg;
		result.depth = pFunc->depth;
		result.cntExec = pFunc->cntExec;
		pOut->Add( result );

	}
	XLIST_LOOP( pFunc->listChild, xFUNC*, pChild )
	{
		RecursiveEvaluation( pOut, pChild, llExecAvg );
	} END_LOOP;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
XProfileObj::XProfileObj( LPCTSTR szFunc ) 
{ 
	Init(); 
	if( XEProfile::sIsActive() == FALSE )
		return;
	_tcscpy_s( m_szFunc, szFunc );
	m_pFunc = XEProfile::sGet()->RegisterObj( szFunc );
	XBREAK( m_pFunc == NULL );
	// 콜스택 푸쉬
//	PROFILE->PushCallStack();
	// 타이머 시작
	m_llStart = XE::GetFreqTime();
}

void XProfileObj::Destroy() 
{
	if( m_pFunc == NULL )
		return;
	m_llEnd = XE::GetFreqTime();
	// 이 프로파일 객체가 실행된 시간을 알아낸다.
	unsigned long long llPass = m_llEnd - m_llStart;
	// 프로파일 관리자에 함수명을 키로해서 실행시간을 업데이트 한다.
	XEProfile::sGet()->Update( m_pFunc, llPass );
//	PROFILE->PopCallStack();
}



