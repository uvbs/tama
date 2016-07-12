#pragma once

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/21 15:32
*****************************************************************/
class XEProfile;
class XEProfileDelegate
{
public:
	XEProfileDelegate() {}
	~XEProfileDelegate() {}
	// get/setter
	// public member
	virtual void OnFinishProfile( XEProfile* pProfile ) {}
}; // class XEProfileDelegate
//////////////////////////////////////////////////////////////////////////
/**
 @brief 프로파일러 사용법
 */
class XEProfile
{
public:
	static void sCreateSingleton( void );
	static XEProfile* sGet( void ) {
		if( PROFILE == NULL )
			sCreateSingleton();
		return PROFILE;
	}
  static void sDestroy() {
    SAFE_DELETE( PROFILE );
  }
	struct xFUNC;
	struct xFUNC {
		TCHAR szFunc[ 256 ];				// 함수명
		unsigned long long llTotalExec;		// 1프레임동안 총 실행된 시간
		int cntExec;						// 실행된 횟수
		int depth;						// 스택 깊이(0:top 0~ +로 갈수록 깊어짐)
		int cntCall;						// 호출된 순서
		xFUNC *pParent;						// 트리 구조를 위한 부모와 자식노드
		XList<xFUNC*> listChild;
		xFUNC() {
			szFunc[0] = 0;
			llTotalExec = 0;
			cntExec = 0;
			depth = 0;
			cntCall = 0;
			pParent = NULL;
		}
	};
	struct xRESULT {
		_tstring strFunc;
		float ratioShare;		// 점유율
		int mcsExecAvg;			// 한프레임당 실행된 시간의 평균
		int mcsExecMin;			// 최소치
		int mcsExecMax;			// 최대치
		int cntExec;
		int depth;				// 콜스택 깊이(0:top 0~ +로 갈수록 깊어짐)
		xRESULT() {
			ratioShare = 0;
			mcsExecAvg = 0;
			mcsExecMin = 0;
			mcsExecMax = 0;
			depth = 0;
			cntExec = 0;
		}
	};
	static XArrayLinear<xRESULT>& sGetResult( void ) {
		return s_aryResult;
	}private:
	static XEProfile *PROFILE;
	static XArrayLinear<xRESULT> s_aryResult;
	enum xtState {
		xOFF,		// 프로파일링이 꺼져있는 상태
		xACTIVE,	// 프로파일링중인 상태
		xFINISH,	// 프로파일링끝내고 평가를 준비중인상태
	};
	xtState m_State;
	// 함수명을 키로하는 맵
	std::map<_tstring, xFUNC*> m_mapFuncs;
	int m_cntCurrCallStackDepth;		// 현재 콜스택 깊이
	int m_cntCall;
	xFUNC *m_pTreeCall;		// 콜 트리
	xFUNC *m_pCurrCall;		// 콜 트리에서 현재 콜스택
	CTimer m_timerDelayStart;		// 자동시작 딜레이 타이머
	CTimer m_timerExec;					// 자동 끝내기 수행시간
	int m_secExec = 0;
	XEProfileDelegate* m_pDelegate = nullptr;
	void Init() {
		m_State = xOFF;
		m_cntCurrCallStackDepth = 0;
		m_cntCall = 0;
		m_pTreeCall = NULL;
		m_pCurrCall = NULL;
	}
	void Destroy();
	GET_SET_ACCESSOR( xtState, State );
public:
	XEProfile() { Init(); }
	virtual ~XEProfile() { Destroy(); }
	//
	SET_ACCESSOR( XEProfileDelegate*, pDelegate );
	// 프로파일링 시작!
	static void sDoStart( void );
	// 프로파일링 끝!
	static void sDoFinish( void );
	static BOOL sIsActive( void ) {
		return sGet()->GetState() == xACTIVE;
	}
	static BOOL sIsOff( void ) {
		return sGet()->GetState() == xOFF;

	}
	static BOOL sIsFinish( void ) {
		return sGet()->GetState() == xFINISH;
	}
	void AutoStart( int secDelay ) {
		m_timerDelayStart.Set( secDelay );
	}
	void AutoFinish( int secExec ) {
		m_secExec = secExec;
	}
	GET_ACCESSOR( int, cntCurrCallStackDepth );
	xFUNC* RegisterObj( LPCTSTR szFunc );
	void Update( xFUNC *pFunc, unsigned long long llExec );
	void DoEvaluation( void );
	BOOL RecursiveEvaluation( XArrayLinear<xRESULT> *pOut, xFUNC *pFunc, unsigned long long llExecAvgParent );
	void PushCallStack( void ) {
		++m_cntCurrCallStackDepth;
	}
	void PopCallStack( void ) {
		--m_cntCurrCallStackDepth;
	}
	void AddChild( xFUNC *pFunc );
	void Process();
};

//////////////////////////////////////////////////////////////////////////

class XProfileObj
{
	TCHAR m_szFunc[ 256 ];
	unsigned long long m_llStart;
	unsigned long long m_llEnd;
	XEProfile::xFUNC *m_pFunc;
	void Init() {
		m_szFunc[0] = 0;
		m_pFunc = NULL;
	}
	void Destroy();
public:
	XProfileObj( LPCTSTR szFunc );
	virtual ~XProfileObj() { Destroy(); }
};

#ifdef _XPROFILE
  #define XPROF_OBJ(STR)		XProfileObj __profObj(_T(STR));
  #define XPROF_OBJ_AUTO()		XProfileObj __profObj(__TFUNC__);
#else
  #define XPROF_OBJ(STR)		{}
  #define XPROF_OBJ_AUTO()		{}
#endif
