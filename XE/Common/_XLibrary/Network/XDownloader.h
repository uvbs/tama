#pragma once

#include <string>

#if !defined(_XE) && !defined(WIN32)
#define strcpy_s		strcpy
#define strcat_s		strcat
test.
#endif // !_XE && ! WIN32


// 패치 클라이언트 객체의 기본형
class XDownloader
{
public:
	enum xtError {
		xERR_NONE = 0,
		xOK,
		xERR_UNKNOWN,
		xERR_COULD_NOT_CONNECT,		// 접속 불가
		xERR_FAILED_REQUEST,			// 파일요청 실패
		xERR_RECV_FILE_SIZE_IS_ZERO,	// 받은파일의 사이즈가 0이다.
		xERR_INVALID_REQUEST,			// 잘못된 파일요청(요청목록 개수가 0)
		xERR_NO_RESPONSE,					// 응답이 없음.
	};
private:
	BOOL m_Destroy;
	void Init() {
		m_Destroy = FALSE;
		memset( m_cServerName, 0, sizeof(m_cServerName) );
	}
	void Destroy();
protected:
	// 서버이름. IP같은거.
	char m_cServerName[128];
	// 서버url의 루트 패스
	std::string m_strSubDir;
	// 클라가 다운을 요청한 파일명(서버명을 뺀 패스포함)
	std::string m_strRequestURL;	
	// m_strRequestFile파일을 받아서 저장할 로컬 풀패스
	std::string m_strDstFullpath;
public:
	XDownloader( const char *cServerName, const char* cSubDir );
	virtual ~XDownloader() { Destroy(); }
	//
#ifdef _XE
	GET_ACCESSOR( const char*, cServerName );
	GET_ACCESSOR( std::string&, strDstFullpath );
	GET_SET_ACCESSOR( BOOL, Destroy );
#else
	const char* GetcServerName( void ) { 
		return m_cServerName; 
	}
	std::string& GetstrDstFullpath( void ) {
		return 
	}
	BOOL GetDestroy( void ) {
		return m_Destroy;
	}
#endif
	// 파일을 다운로드 중인가
	bool IsRequesting( void ) {
		return m_strRequestURL.empty() == false;
	}
	//
	virtual bool Create( void );
	// cServerPath: 서버측의 패스. update/image/ 와같은 형식으로 와야한다.
	inline bool RequestFile( const char* cServerPath, const char* cFilename, const char* cDstFullpath ) {
		char szURL[ 1024 ];
		strcpy_s( szURL, cServerPath );
		strcat_s( szURL, cFilename );
		return RequestFile( szURL, cDstFullpath );
	}
	virtual bool RequestFile( const char* _cURL, const char* _cDstFullpath );
};
