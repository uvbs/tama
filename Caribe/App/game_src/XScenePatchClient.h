#pragma once
#include "XSceneBase.h"

#define XPATCH_URL				"caribe/update/"
#define XFULL_DIR				"full/"
#define XCORE_DIR				"core/"
#define FILE_LAST_UPDATE		"LastUpdate.txt"
#define XUPDATE_DIR				"update/"
#define XUPDATE_CORE_DIR		"update/core/"
#define FILE_FULL_LIST			"full_list.txt"


class XWndTextString;
class XPatchClient;
class XDownloadTask;
class XScenePatchClient : public XSceneBase 
{
	enum xtAct { xACT_ENTER,
				xACT_PROCESS,
				xACT_RECV,		// 다운 다받은 이벤트
				xACT_RECV_EACH,	// 여러개 다운받을때 각각 파일받은 후 발생.
				xACT_ON_YES,
				xACT_ON_NO,
				xACT_LEAVE,
	};
	enum xtState {
				xST_NONE,
				xST_REQ_VER,			// VerPacket.h와 LastUpdate.txt를 요청한다
				xST_REQ_FULL_LIST,		// 풀 리스트 파일 요청
				xST_BRANCH_UPDATE_LIST,
				xST_REQ_UPDATE_LIST,		// 업데이트 리스트 요청
				xST_REQ_RECV_RES,			// 업데이트 리소스 파일들 받는중
				xST_AFTER_DOWNLOAD,			// 파일패치가 모두 끝난후 마무리
				xST_EXIT_SCENE = 50,			// 씬 끝냄
				xST_ERROR = 100,			// 패치 에러
				xST_ERROR_UPDATE_LIST,		// 
				xST_ERROR_DOWNLOAD,		// 리소스 업데이트 실패
				xST_ERROR_COULD_NOT_CONNECT,
	};
	enum xtDownload {
		xDL_NONE,
		xDL_UPDATE,	// 업데이트된 파일만 받음
		xDL_FULL,		// 풀리스트를 받음.
	};
// 	enum xtPatchFrom {
// 		xPF_NONE,
// 		xPF_FROM_SERVER,
// 		xPF_FROM_APK,
// 	};
private:
	xtState m_State;
	xtState m_StateOld;
	xtState m_StateNext = xST_NONE;
	XParamObj2 m_paramNext;
	//int m_verLastUpdateInApk = 0;
	int m_LocalLastUpdate = 0;
	int m_LastUpdateOnServer = 0;
	XWndTextString *m_pWndText;
	XList4<std::string> m_listErrorFiles;	// 다운받던중 에러난 파일들 목록.
	XList4<std::string> m_listCompleteFiles;		// 다운받은 파일목록
	XList4<_tstring> m_listWillDownload;			// 
	XDownloadTask *m_pTask;
	CTimer m_timerCreateUI;
	int m_Error;
//	std::string m_strDownloadedFile;
	bool m_bDrawProgress = false;		// 다운받을때 프로그레스바를 그릴지 말지
	bool m_bPatched = false;
	xSec m_secPatchStart = 0;		// 총 패치시간 측정용
	xSec m_secTotalPatch = 0;		// 패치에 걸린 시간
//	xtPatchFrom m_modePatchFrom = xPF_NONE;
	//
	void Init()  {
		m_State = xST_NONE;
		m_StateOld = xST_NONE;
		m_pWndText = nullptr;
		m_Error = 0;
		m_pTask = nullptr;
	}
	void Destroy();
protected:
public:
	XScenePatchClient( void );
	virtual ~XScenePatchClient(void) { Destroy(); }
	//
	//
	int ChangeState( xtState state, const XParamObj2& param );
	int DoFSMState( xtState state, xtAct event, const XParamObj2& param );
	int FSMReqVersion( xtAct event, const XParamObj2& param );
	int FSMReqUpdateList( xtAct event, const XParamObj2& param );
	int FSMReqRecvRes( xtAct event, const XParamObj2& param );
	int FSMExitScene( xtAct event, const XParamObj2& param );
	int FSMError( xtAct event, const XParamObj2& param );
	//
	int GetUpdateVerFromWork( LPCTSTR szRes );
	inline int GetUpdateVerFromWork( const _tstring& strRes ) {
		return GetUpdateVerFromWork( strRes.c_str() );
	}
	int DoMergeUpdateList( XList4<_tstring>& listUpdateList, XList4<_tstring> *pOut );
	BOOL IsFirstPlay( void );
	BOOL SavePatchIni( void );
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	//virtual void OnCompleteRecvFile( const char *cURL, const char *cFullpath, int size );
	//
	//int OnRecvVersions( XWnd *pWnd, DWORD, DWORD );
	//int OnRecvUpdateList( XWnd *pWnd, DWORD, DWORD );
	int OnRecvRes( XWnd *pWnd, DWORD, DWORD );
	int OnBack( XWnd *pWnd, DWORD p1, DWORD p2 ) override;
	void CreateUI();
	int FSMReqFullList( xtAct event, const XParamObj2& param );
	//int OnRecvFullList( XWnd* pWnd, DWORD p1, DWORD p2 );
	int FSMAfterDownload( xtAct event, const XParamObj2& param );
	bool LoadDownloadedList( int verServer );
	bool SaveDownloadedList( void );
	//int OnRecvEachRes( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnErrorDownload( XWnd* pWnd, DWORD p1, DWORD codeErr );
	int OnTouch( XWnd* pWnd, DWORD p1, DWORD p2 );
	bool IsApkFile( const _tstring& strRes, std::map<_tstring, int>& mapApk );
	void LoadApkList( std::map<_tstring, int>* pMapOut );
	int GetUpdateVerFromApk( LPCTSTR szRes );
	//int OnYesCDMADownloadInRecvRes( XWnd* pWnd, DWORD p1, DWORD p2 );
	int FSMBranchUpdateList( xtAct event, const XParamObj2& param );
};

