#pragma once
#ifdef _XPATCH
#include "XBaseScene.h"

class XPatchClient;
class XDownloadTask;
class MWScenePatchClient : public XBaseScene 
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
				xST_CHECK_FIRST,		// 최초 실행인지 검사
//				xST_REQ_SUB_LIST,		// sub_list.txt 요청
//				xST_DOWNLOADING_SUBLIST,	// 서브파일들 다운로딩중
				xST_REQ_VER_PACKET,		// 프로토콜 버전 파일 요청
				xST_REQ_VER_UPDATE,		// 업데이트 버전 파일 요청
				xST_REQ_UPDATE_LIST,		// 업데이트 리스트 요청
				xST_REQ_RECV_RES,			// 업데이트 리소스 파일들 받는중
				xST_EXIT_SCENE = 50,			// 씬 끝냄
				xST_ERROR = 100,			// 패치 에러
				xST_ERROR_UPDATE_LIST,		// 
				xST_ERROR_DOWNLOAD,		// 리소스 업데이트 실패
				xST_ERROR_COULD_NOT_CONNECT,
	};
private:
//	XPatchClient *m_pPatch;
	xtState m_State;
	xtState m_StateOld;
	int m_MyUpdateVer;
	int m_ServerUpdateVer;
	char m_cRequestURL[ 256 ];	// 서버에 요청할 url을 여기다 받아두면 메인스레드에서 꺼내 요청한다.
	int m_idxAryUpdate;
	XWndTextString *m_pWndText;
	XArrayLinearN<std::string, 100> m_aryErrorFile;		// 다운받기 에러난 파일들 목록.
	XList<_tstring> m_listSubFiles;			// 서브리소스 파일들 리스트
	XArrayLinear<std::string> m_aryCompleteSubFiles;
	XDownloadTask *m_pTask;
	int m_Error;
	std::string m_strDownloadedFile;
	//
	void Init()  {
//		m_pPatch = NULL;
		m_State = xST_NONE;
		m_StateOld = xST_NONE;
		m_MyUpdateVer = 0;
		m_ServerUpdateVer = 0;
		m_idxAryUpdate = 0;
		m_pWndText = NULL;
		m_Error = 0;
		m_pTask = NULL;
		XCLEAR_ARRAY( m_cRequestURL );
	}
	void Destroy();

	void SetcRequestURL( const char* cURL ) {
		XBREAK( XE::IsHave( m_cRequestURL ) == TRUE );
		strcpy_s( m_cRequestURL, cURL );
	}
	void ClearRequestURL( void ) {
		m_cRequestURL[0] = 0;
	}
protected:
public:
	MWScenePatchClient( void );
	virtual ~MWScenePatchClient(void) { Destroy(); }
	//
	//
	int ChangeState( xtState state );
	int DoFSMState( xtState state, xtAct event, string *pFilename=NULL );
	int FSMCheckFirst( xtAct event );
//	int FSMReqSubList( xtAct event );
//	int FSMDownloadingSubList( xtAct event, string *pFilename=NULL );
	int FSMReqVerPacket( xtAct event );
	int FSMReqVerUpdate( xtAct event );
	int FSMReqUpdateList( xtAct event );
	int FSMReqRecvRes( xtAct event );
	int FSMExitScene( xtAct event );
	int FSMError( xtAct event );
	//
	int GetUpdateVer( LPCTSTR szRes );
	int GetUpdateVerFromPackage( LPCTSTR szRes );
	int DoMergeUpdateList( XList<_tstring>& listUpdateList, XList<_tstring> *pOut );
	BOOL IsFirstPlay( void );
	BOOL SavePatchIni( void );
//	BOOL LoadDownloadedSubList( void );
//	BOOL SaveDownloadedSubList( void );
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnCompleteRecvFile( const char *cURL, const char *cFullpath, int size );
	//
	int OnRecvVerPacket( XWnd *pWnd, DWORD, DWORD );
	int OnRecvUpdateVer( XWnd *pWnd, DWORD, DWORD );
	int OnRecvUpdateList( XWnd *pWnd, DWORD, DWORD );
	int OnRecvRes( XWnd *pWnd, DWORD, DWORD );
//	int OnRecvSubList( XWnd *pWnd, DWORD, DWORD );
//	int OnRecvSubRes( XWnd *pWnd, DWORD, DWORD );
//	int OnRecvEachSubRes( XWnd *pWnd, DWORD, DWORD p2 );
	int OnYesCDMADownload( XWnd* pWnd, DWORD p1, DWORD p2 );
	//
};

#endif