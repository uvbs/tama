#include "stdafx.h"
#include "XDownloadTask.h"
#include "Network/XDownloaderCurl.h"
#include "XResMng.h"
#include "xeDef.h"
#include "XSystem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//XDownloadTask *CURL_TASK = NULL;

void XDownloadTask::Destroy()
{
	m_pDownloader->SetDestroy( TRUE );
	SAFE_DELETE( m_pDownloader );
}
BOOL XDownloadTask::Create( const char *cServerName, const char *cDefaultURL )
{
	m_pDownloader = new XDownloaderCurl( cServerName, cDefaultURL );
	if( m_pDownloader->Create() == false )
		return FALSE;
	//
	return TRUE;
}

/**
 다운로드를 시작하라       
*/
void XDownloadTask::Go( void )
{
	XTRACE("XDownloadTask Go! listReq.size=%d", m_listReq.size());
	if( XBREAK( m_listReq.size() == 0 ) )	{
		XTRACE("error go");
		CallEventHandler( XWM_ERROR_DOWNLOAD, (DWORD) XDownloader::xERR_INVALID_REQUEST );
		return;
	}
	m_bGo = TRUE;
	m_bComplete = FALSE;
	m_bCallbackAll = FALSE;
// 	m_itor = m_listReq.GetNextClear2();
	m_itor = m_listReq.begin();
// 	m_idxCurrDownload = 0;
	m_Error = 0;
//	m_pDownloader->
	
}

/**
 @brief 현재 다운로드중인 파일의 정보를 리턴한다.
*/
XDownloadTask::xRECV_INFO XDownloadTask::GetCurrDownloadInfo( void )
{
	xRECV_INFO recvInfo;
//	if( m_idxCurrDownload <= m_listReq.size() && m_idxCurrDownload > 0 )
	if( m_itor != m_listReq.end() )
	{
//		xREQ_INFO reqInfo = m_listReq.GetFromIndex( m_idxCurrDownload - 1 );
		xREQ_INFO reqInfo = *m_itor;
		DWORD sizeFile = m_pDownloader->GetsizeFile();
		reqInfo.sizeCurr = sizeFile;
		recvInfo = reqInfo;
	}
	return recvInfo;
}
//
int XDownloadTask::AddRequest( LPCTSTR pathSub, LPCTSTR szURL, LPCTSTR szDstLocal )
{
	if( XBREAK( m_bGo == TRUE ) )
		return 0;
	// 다운 다받고 나서 콜백처리 아직 안했는데 애드되는걸 막기위함.
	if( XBREAK( m_bComplete == TRUE ) )
		return 0;
	xREQ_INFO info;
	
	info.strURL = SZ2C( szURL );	// 스레드안에서 SZ2C를 안쓰려고 여기서 변환함.
	info.strSubPath = SZ2C( pathSub );
	// 기본적으로 워킹폴더/szURL로 받는다. 다운경로에 pathSub는 무시된다.
	_tstring strPath = XE::GetPathWork();
	if( szDstLocal == NULL )
	{
		strPath += szURL;
		TCHAR szPath[ 1024 ];
		_tcscpy_s( szPath, strPath.c_str() );
		XPLATFORM_PATH( szPath );
		info.strToRename = SZ2C( szPath );
		_tstring path = XE::GetFilePath( szPath );
		_tstring filename = XE::GetFileName( szPath );
		strPath = path;	// 원본파일명으로 받을땐 파일명 앞에 특수문자를 붙여서 받은후 원본파일을 지우고 받은파일을 리네임시킨다.
		strPath += _T("__");
		strPath += filename;
	}
	else
	{
		if( XE::IsHave( XE::GetFileName( szDstLocal ) ) )
		{
			// dstLocal에 파일명이 지정되어있다면 
			// dstFullpath = 워킹폴더+szDstLocal
			strPath += szDstLocal;
//			_tcscat_s( szPath, szDstLocal );
		} else
		{
			// 파일명이 안지정되어 있다면
			// dstFullpath = 워킹폴더+szDstLocal+szURL
			strPath += szDstLocal;
			strPath += szURL;
//			_tcscat_s( szPath, szDstLocal );
//			_tcscat_s( szPath, szURL );
		}
	}
	TCHAR szPath[ 1024 ];
	_tcscpy_s( szPath, strPath.c_str() );
	XPLATFORM_PATH( szPath );
	info.strDstFullpath = SZ2C( szPath );	
	m_listReq.Add( info );
	return m_listReq.size();
}
//
int XDownloadTask::Process( float dt )
{
	// 다운로드 시작
	if( m_bGo )	{
		// 다운로더가 다운받는 동작을 하고 있지 않을때
		if( m_pDownloader->GetbGo() == FALSE )		{
			if( m_itor != m_listReq.end() )			{
				// 다운로드 리스트에서 하나씩 꺼내서 요청
				xREQ_INFO info = *m_itor;
				std::string strURL = info.strSubPath;
				strURL += info.strURL;
				bool result = m_pDownloader->RequestFile( strURL.c_str(), info.strDstFullpath.c_str() );
				if( result )				{
				} else
					++m_Error;
			}
		} else	{
			auto& refTimer = m_pDownloader->GettimerTimeout();
			if( !m_pDownloader->IsDownloading() ) {
				// 요청은 했으나 다운로드가 시작이 안되고 있음.
				if( refTimer.IsOff() )
					refTimer.Set( 10.f );
				// 일정시간 지나도 응답이 없으면 에러 출력.
				if( refTimer.IsOver() ) {
					refTimer.Off();
					if( m_RiseError == 0 ) {
						m_RiseError = 1;
						CallEventHandler( XWM_ERROR_DOWNLOAD, (DWORD)XDownloader::xERR_NO_RESPONSE );
					}
				}
			} else {
				// 다운로드가 한번이라도 시작됐으면 타임아웃 타이머 꺼줌.(다운받다가 서버연결안되면 멈출수도 있음)
				refTimer.Off();
			}
			// 에러가 있었으나 방금 해결됨.
			if( m_RiseError ) {
				// 문제가 해결된걸 알려줘야 함. <= 어따쓰는거지 -_-?
				CallEventHandler( XWM_ERROR_DOWNLOAD, (DWORD)XDownloader::xOK );
				m_RiseError = 0;
			}
			// 다운로더 내부에서 타임아웃이 발생했다면 에러 핸들링
// 			if( m_pDownloader->GettimerTimeout().IsOver() ) {
// 				m_pDownloader->GettimerTimeout().Off();
// 				if( m_RiseError == 0 ) {
// 					m_RiseError = 1;
// //					XDownloader::xtError codeErr = m_pDownloader->GetErrorCode();
// //					CallEventHandler( XWM_ERROR_DOWNLOAD, (DWORD) codeErr );
// 					CallEventHandler( XWM_ERROR_DOWNLOAD, (DWORD)XDownloader::xERR_NO_RESPONSE );
// 				}
// 			} else {
// 				// 에러가 있었으나 방금 해결됨.
// 				if( m_RiseError ) {
// 					// 문제가 해결된걸 알려줘야 함. <= 어따쓰는거지 -_-?
// 					CallEventHandler( XWM_ERROR_DOWNLOAD, (DWORD) XDownloader::xOK );
// 					m_RiseError = 0;
// 				}
// 			}

			// 파일 하나를 다 받음.
			if( m_pDownloader->GetbComplete() )	{
				xREQ_INFO reqInfo;
				{
					reqInfo = *m_itor++;
					if( reqInfo.strToRename.empty() == false )	{
						// 받은 파일을 리네임 시킴
// 						const auto size1 = XSYSTEM::GetFileSize( reqInfo.strDstFullpath.c_str() );
// 						const auto size2 = XSYSTEM::GetFileSize( reqInfo.strToRename.c_str() );
						// rename해야함.
						// 원래 파일 삭제
						XSYSTEM::RemoveFile( reqInfo.strToRename.c_str() );		
// 						const _tstring str1 = C2SZ(reqInfo.strDstFullpath);
// 						const _tstring str2 = C2SZ(reqInfo.strToRename);
// 						XTRACE("%s(%d) => %s(%d)", str1.c_str(), size1
// 																			, str2.c_str(), size2 );
						XBREAK( XSYSTEM::RenameFile( reqInfo.strDstFullpath.c_str(), reqInfo.strToRename.c_str() ) == FALSE );
					}
				}
				if( m_itor == m_listReq.end() ) {
					// 모든 파일을 다 다운받음.
					XTRACE("XDownloadTask Complete. call handler");
					m_bComplete = TRUE;
					m_bCallbackAll = FALSE;
					m_bGo = FALSE;
				} else {
					XTRACE("XDownloadTask each Complete. next file....");
					CallEventHandler( XWM_EACH_COMPLETE, (DWORD)(&reqInfo) );
					// complete상태를 풀어줘서 Go 대기상태로 만듬.
					m_pDownloader->ClearComplete();
				}
			}
		}

	}
	if( m_Error )
	{
		CallEventHandler( XWM_ERROR_DOWNLOAD, XDownloader::xERR_FAILED_REQUEST );
		m_Error = 0;
	}
	// 다운로드가 다 끝나고 콜백호출을 아직 안했으면 호출함.
	if( m_bComplete && m_bCallbackAll == FALSE )
	{
		// 콜백호출까지 하면 다 마무리
//		m_idxCurrDownload = 0;
		m_itor = m_listReq.begin();
		m_bCallbackAll = TRUE;
		m_bComplete = FALSE;
		m_listComplete = m_listReq;
		m_listReq.clear();
		BOOL bSuccess = TRUE;
		XDownloader::xtError codeErr = XDownloader::xERR_UNKNOWN;
		// 받은 파일의 에러검사.
		if( XBREAK( m_listComplete.size() == 0 ) )
			bSuccess = FALSE;
		for( auto& info : m_listComplete )
		{
			if( XBREAK( info.strDstFullpath.empty() == true ) )
				bSuccess = FALSE;
			if( XBREAK( info.strURL.empty() == true ) )
				bSuccess = FALSE;
		}
		//
		if( bSuccess )
		{
			// 다운 다받은후 이벤트 발생시킬땐 에러가 없음을 보증하고 발생시킬것.
			XTRACE("XDownloadTask Call handler");
			CallEventHandler( XWM_ALL_COMPLETE );
		} else
			CallEventHandler( XWM_ERROR_DOWNLOAD, codeErr );
		m_pDownloader->ClearComplete();
	}
	return 1;
}

