#pragma once
//#include "XWindow.h"
#include "_Wnd2/XWnd.h"
#include "XLockPThreadMutex.h"
#include "Network/XDownloaderCurl.h"

class XDownloaderCurl;
class XDownloadTask : public XWnd//, public XLockPthreadMutex
{
public:
	struct xREQ_INFO {
		std::string strURL;		// 다운받을 파일의 경로. cServerName/cDefaultURL/strSubPath/strURL
		std::string strSubPath;	// strURL의 앞에 붙는 추가 패스
		std::string strDstFullpath;	// 다운받을 풀패스
		std::string strToRename;	// 받은 후 리네임 될 파일명
		int	invoke;				// 몇번에 걸쳐서 패킷을 나눠받고 있나
		DWORD sizeCurr;		// 현재까지 받은 사이즈
		DWORD sizeOrig;		// 원래 사이즈(0일수 있다)
		BOOL bComplete;		// 다운로드 완료?(아직 사용하지 않음)
		xREQ_INFO() {
			invoke = 0;
			sizeCurr = 0;
			sizeOrig = 0;
			bComplete = FALSE;
		}
	};
	struct xRECV_INFO {
//		TCHAR szURL[ 256 ];
		_tstring strURL;
		DWORD sizeCurr;
		DWORD sizeOrig;
		BOOL bComplete;	// 아직 사용하지 않음.
		xRECV_INFO() {
// 			szURL[0] = 0;
			sizeOrig = 0;
			sizeCurr = 0;
			bComplete = FALSE;
		}
		xRECV_INFO& operator = ( const xREQ_INFO& rhs ) {
//			_tcscpy_s( szURL, C2SZ( rhs.strURL.c_str() ) );
			strURL = C2SZ( rhs.strURL.c_str() );
			sizeCurr = rhs.sizeCurr;
			sizeOrig = rhs.sizeOrig;
			bComplete = rhs.bComplete;
			return *this;
		}

	};
private:
	XDownloaderCurl *m_pDownloader;
	BOOL m_bGo;
	BOOL m_bComplete;
	BOOL m_bCallbackAll;				// AllComplete Callback을 호출했는지.
	XList4<xREQ_INFO> m_listReq;		// 다운로드 요청 리스트
	XList4<xREQ_INFO>::iterator m_itor;
	XList4<xREQ_INFO> m_listComplete;	// 다운로드 완료된 리스트
//	int m_idxCurrDownload;			// 현재 다운받고 있는 파일 인덱스
	int m_Error;
	int m_Cnt;
	int m_RiseError;		// 에러발생
	void Init() {
		m_pDownloader = NULL;
//		m_hWorkThread = 0;
		m_bGo = FALSE;
		m_bComplete = FALSE;
		m_bCallbackAll = FALSE;
//		m_idxCurrDownload = 0;
		m_Error = 0;
		m_Cnt = 0;
		m_RiseError = 0;
	}
	void Destroy();
public:
	XDownloadTask() { Init(); }
	virtual ~XDownloadTask() { Destroy(); }
	//
	GET_ACCESSOR( XList4<xREQ_INFO>&, listReq );
	GET_ACCESSOR( XList4<xREQ_INFO>&, listComplete );
	GET_ACCESSOR( BOOL, bGo );
	GET_ACCESSOR( BOOL, bComplete );
//	GET_ACCESSOR( int, idxCurrDownload );
	GET_ACCESSOR( int, Error );
	XDownloader::xtError GetErrorCode( void ) {
		return m_pDownloader->GetErrorCode();
	}
	xRECV_INFO GetCurrDownloadInfo( void );
	int GetNumRequest( void ) {
		return m_listReq.size();
	}
	int GetNumComplete( void ) {
		return m_listComplete.size();
	}
	xREQ_INFO* GetCompleteFromIndex( int idx ) {
		return m_listComplete.GetpByIndex( idx );
	}
	//
	BOOL Create( const char *cServerName, const char *cDefaultURL );
	void Go( void );
	int AddRequest( LPCTSTR pathSub, LPCTSTR szURL, LPCTSTR szDstLocal = nullptr );
	inline int AddRequest( LPCTSTR pathSub, const _tstring& strURL, LPCTSTR szDstLocal = nullptr ) {
		return AddRequest( pathSub, strURL.c_str(), szDstLocal );
	}
	inline int AddRequest( LPCTSTR pathSub, LPCTSTR szURL, const _tstring& strDstLocal ) {
		return AddRequest( pathSub, szURL, strDstLocal.c_str() );
	}
	inline int AddRequest( LPCTSTR pathSub, const _tstring& strURL, const _tstring& strDstLocal ) {
		return AddRequest( pathSub, strURL.c_str(), strDstLocal.c_str() );
	}
// 	inline int AddRequest( LPCTSTR pathSub, LPCTSTR szURL, const _tstring&& strDstLocal ) {
// 		return AddRequest( pathSub, szURL, strDstLocal.c_str() );
// 	}
// 	inline int AddRequest( LPCTSTR pathSub, LPCTSTR szURL, _tstring&& strDstLocal ) {
// 		return AddRequest( pathSub, szURL, strDstLocal );
// 	}
	//
	virtual int Process( float dt );
	int GetidxCurrDownload() {
		return m_listReq.GetIndex( m_itor );
	}
};

