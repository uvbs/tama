#pragma once
#include <stdlib.h>
#include "xe.h"
#include "XLibrary.h"
#include "etc/Timer.h"
#include "Debug.h"
//using namespace std;

#ifdef _XBOT
#define GET_FREQ_TIME()				XE::GetFreqTime()
#else
#define GET_FREQ_TIME()				0
#endif // _XBOT

template <typename NEWT, typename OLDT>
NEWT SafeCast( OLDT p ) {
	if( p == NULL )
		return NULL;
#ifdef DEBUG
	NEWT newp = dynamic_cast<NEWT>(p);
	XBREAK( newp == NULL );
	return newp;
#else
	NEWT newp = static_cast<NEWT>(p);
	return newp;
	//	return (NEWT)(p);
#endif
}
template <typename NEWT, typename OLDT>
NEWT SafeCast2( OLDT p ) {
	if( p == nullptr )
		return nullptr;
#ifdef DEBUG
	return dynamic_cast<NEWT>(p);
#else
	return static_cast<NEWT>(p);
#endif
}

#define		SWAP( A, B )	{ A ^= B; B ^= A; A ^= B; }

#define XFORMAT( F, ... )		XE::Format( _T(F),##__VA_ARGS__ )
inline BYTE xboolToByte( bool flag ) {
	return (flag)? (BYTE)1 : (BYTE)0;
}
inline bool xbyteToBool( BYTE b0 ) {
	return (b0 != 0);
}
inline bool xbyteToBool( char b0 ) {
	return (b0 != 0);
}
inline BOOL xboolToBOOL( bool flag ) {
	return ( flag ) ? TRUE : FALSE;
}
inline bool xBOOLToBool( BOOL flag ) {
	return ( flag != 0 );
}

////////////////////////////////////////////////////////////////
#define DWORD_COMPOSITION(N1,N2,N3,N4)		((DWORD)((((N1)&0xff)<<24)|(((N2)&0xff)<<16)|(((N3)&0xff)<<8)|((N4)&0xff)))

/**
 @brief rand()를 사용하는 버전
*/
// class XRandom
// {
// 	DWORD m_Seed;
// 	void Init() {
// 		m_Seed = 0;
// 	}
// 	void Destroy() {}
// public:
// 	XRandom( DWORD seed=0 ) { 
// 		Init(); 
// 		SetSeed( seed );
// 	}
// 	~XRandom() { Destroy(); }
// 	//
// //	GET_ACCESSOR( DWORD, Seed );
// 	void SetSeed( DWORD seed ) {
// 		m_Seed = seed;
// 		::srand( m_Seed );
// 	}
// 	DWORD GetRand() {
// 		BYTE n1 = (BYTE)(rand() & 0xff);
// 		BYTE n2 = (BYTE)(rand() & 0xff);
// 		BYTE n3 = (BYTE)(rand() & 0xff);
// 		BYTE n4 = (BYTE)(rand() & 0xff);
// 		DWORD dwNum = DWORD_COMPOSITION( n1, n2, n3, n4 );
// 		return dwNum;
// 	}
// 	DWORD GetRandom( DWORD range ) {
// 		if( XBREAK(range == 0) )	return 0;
// 		return GetRand() % range;
// 	}
// 	/**
// 	 max - min이 0x7fffffff를 넘지 않도록 주의
// 	 리턴값이 0x7fffffff을 넘지 않도록 주의
// 	*/
// 	int GetRandom( int min, int max ) {
// 		if( min == max )
// 			return min;
// 		if( max < min )
// 			SWAP( min, max );
// 		XASSERT( max - min + 1 > 0 );
// 		XBREAK( max - min + 1 > 0x7fffffff );	// 이렇게 큰수가 넘어가면 (int)했을때 마이너스가 될 우려가 있다.
// 		return min + (int)GetRandom( max - min + 1 );
// 	}
// 	float GetRandomF( float range ) {
// 		return (float)(((double)GetRand() / 0xFFFFFFFF) * range);
// 	}
// 	float GetRandomF( float min, float max ) {
// 		if( min == max )
// 			return min;
// 		if( max < min )
// 			std::swap( min, max );
// 		float f = min + GetRandomF( max - min );
// 		return f;
// 	}
// };

DWORD xRand();
DWORD xRandom( DWORD nRange );
int xRandom( int min, int max );
float xRandomF( float fRange );
float xRandomF( float start, float end ); 
void xSRand( DWORD seed );
//DWORD xGetSeed();
inline int random( DWORD range ) {
	if( range == 0 )
		return 0;
	return xRand() % range;
// 	XBREAK( range > RAND_MAX );
// 	return rand() % range;
}
//XUINT64 xGenerateSeed64();
DWORD xGenerateSeed();

int random( short min, short max );

int CalcFPS();

namespace XE 
{
	extern XRandom2 RANDOM_OBJ;		// 싱글톤 랜덤객체
	// TCHAR
	inline BOOL IsEmpty( LPCTSTR str ) {
		if( str == NULL )
			return TRUE;
		return (str[0] == _T('\0')) ? TRUE : FALSE;
	}
	inline	BOOL IsHave( LPCTSTR str ) {
		return !IsEmpty(str);
	}
	inline BOOL IsSame( LPCTSTR sz1, LPCTSTR sz2 ) {
		if( sz1 == NULL || sz2 == NULL )
			return FALSE;
		return (_tcsicmp( sz1, sz2 ) == 0);
	}
	bool IsAsciiStr( LPCTSTR szStr );
	// 0~9, A~z까지인지 검사한다.
	inline BOOL IsAsciiChar( LPCTSTR szString ) {
		int len = _tcslen( szString );
		BOOL bSuccess = TRUE;
		for( int i = 0; i < len; ++i ) {
			TCHAR c = szString[ i ];
			if( ( c >= _T( 'a' ) && c <= _T( 'z' ) ) || ( c >= _T( 'A' ) && c <= _T( 'Z' ) ) ||
				( c >= _T( '0' ) && c <= _T( '9' ) ) ) {
				// 합격
			} else
				return FALSE;
		}
		return TRUE;
	}
	inline bool IsAsciiChar( TCHAR c ) {
		if( ( c >= 'a' && c <= 'z' )
			|| ( c >= 'A' && c <= 'Z' )
			|| ( c >= '0' && c <= '9' ) )
			return true;
		return false;
	}
#ifdef _UTF8
#else
#endif

#ifdef _XUTF8
	inline bool IsHangul( char cChar ) {
		return ( cChar & 0x80 ) ? true : false;
	}
    // 한글/영문/아스키 포함한 문자열에서 한글을 길이1로 인식하도록 길이를 센다.
    inline int GetHangulLen( LPCTSTR szStr ) {
        XBREAK(1);
        return 1;
    }
#else
	inline bool IsHangul( TCHAR tChar ) {
// 		return ( tChar >= 0x80 ) ? true : false;
		return (tChar >= 0xac00 && tChar <= 0xd7a3 );
	}
	inline int GetHangulLen( LPCTSTR szStr ) {
		return _tcslen( szStr );
	}
#endif

	inline bool IsHangulForUTF8( const char utf8Char ) {
		return ( utf8Char >= 0x80 ) ? true : false;
	}

#ifdef WIN32
	// char
	inline BOOL IsEmpty( const char *str )	{
		if( str == NULL )
			return TRUE;
		return (str[0] == 0) ? TRUE : FALSE;
	}
	inline	BOOL IsHave( const char *str )	{
		return !IsEmpty(str);
	}
	inline BOOL IsSame( const char *sz1, const char *sz2 ) {
		if( sz1 == NULL || sz2 == NULL )
			return FALSE;
		return (_stricmp( sz1, sz2 ) == 0);
	}
	const char* Format( const char* cFormat, ... );
	const char* GetFilePath( char *pOut, int sizeOut, const char * cSrc, char cSlash='/' );
	template<int _Size>
	inline const char* GetFilePath( char (&buff)[_Size], const char * cSrc, char cSlash='/' ) {
		return GetFilePath( buff, _Size, cSrc, cSlash );
	}
	const char* GetFilePath( const char * cSrc, char cSlash='/' );
	inline const char* GetFilePath( const std::string& strcSrc, char cSlash = '/' ) {
		return GetFilePath( strcSrc.c_str(), cSlash );
	}
	const char* GetFileName( char *pOut, int sizeOut, const char* cSrc );
	template<int _Size>
	inline const char* GetFileName( char (&buff)[_Size], const char* cSrc ) {
		return GetFileName( buff, _Size, cSrc );
	}
	const char* GetFileName( const char* cSrc );
	inline const char* GetFileName( const std::string& str ) {
		return GetFileName( str.c_str() );
	}
#endif // win32
	LPCTSTR Format( LPCTSTR szFormat, ... );
	LPCTSTR StrTrimLeft( LPCTSTR szStr, int n );

	LPCTSTR GetFilePath( TCHAR *pOut, int sizeOut, LPCTSTR szSrc, TCHAR tSlash='/' );
	template<int _Size>
	inline LPCTSTR GetFilePath( TCHAR (&tBuff)[_Size], LPCTSTR szSrc, TCHAR tSlash='/' ) {
		return GetFilePath( tBuff, _Size, szSrc, tSlash );
	}
	LPCTSTR GetFilePath( LPCTSTR szSrc, TCHAR tSlash='/' );
	inline LPCTSTR GetFilePath( const _tstring& strSrc, TCHAR tSlash='/' ) {
		return GetFilePath( strSrc.c_str(), tSlash );
	}
	LPCTSTR GetFileName( TCHAR *pOut, int sizeOut, LPCTSTR szSrc );
	template<int _Size>
	inline LPCTSTR GetFileName( TCHAR (&tBuff)[_Size], LPCTSTR szSrc ) {
		return GetFileName( tBuff, _Size, szSrc );
	}
	LPCTSTR GetFileName( LPCTSTR szSrc );
	inline LPCTSTR GetFileName( const _tstring& str ) {
		return GetFileName( str.c_str() );
	}
	LPCTSTR GetFileTitle( LPCTSTR szSrc );
	inline LPCTSTR GetFileTitle( const _tstring& str ) {
		return GetFileTitle( str.c_str() );
	}
	LPCTSTR GetFileTitleX( LPCTSTR szSrc );
// 	inline LPCTSTR GetFileTitleX( LPCTSTR szSrc ) {
// 		return GetFileTitle( szSrc );
// 	}
#ifdef WIN32
	LPCTSTR GetFileExt( LPCTSTR szSrc );
	inline LPCTSTR GetFileExt( const _tstring& str ) {
		return GetFileExt( str.c_str() );
	}
#endif
	const char* GetFileExt( const char *cSrc );
	inline const char* GetFileExt( const std::string& str ) {
		return GetFileExt( str.c_str() );
	}
	LPCTSTR GetClassName( LPCTSTR szSrc );
    unsigned char* GetFileDataFromZip( const char* pszZipFilePath, const char* pszFileName, unsigned long *pOutSize );
	BOOL IsExistFileFromZip( const char* pszZipFilePath, const char* pszFileName );
//	BOOL GetFileInfoFromZip( const char* pszZipFilePath, const char* pszFileName, unz_file_info *pOutInfo );
	time_t GetFileModifyTimeFromZip( const char* pszZipFilePath, const char* pszFileName );

// 	#define GET_FILE_NAME(S) ((_tcsrchr((S), ('\\')) ? _tcsrchr((S), ('\\')) : (S) - 1) + 1) 
// 	#define GET_FILE_EXT(S) ((_tcsrchr((S), ('.')) ? _tcsrchr((S), ('.')) : (S) - 1) + 1) 

	template<typename T, typename P>
	inline BOOL IsArea( T l, T t, T r, T b, P x, P y )	{
		if( r < l )
			std::swap( l, r );
		if( b < t )
			std::swap( t, b );
		if( (T)x >= l && (T)x <= r )
			if( (T)y >= t && (T)y <= b )
				return TRUE;
		return FALSE;
	}
	template<typename T, typename P>
	inline BOOL IsArea2( T l, T t, T w, T h, P x, P y ) {
		return IsArea( l, t, l+w-1, t+h-1, x, y );
	}
	template<typename T>
	inline BOOL IsArea( T l, T t, T w, T h, const VEC2& v ) {
		return IsArea( l, t, l+w-1, t+h-1, v.x, v.y );
	}
	inline BOOL IsArea( const VEC2& vAreaLT, const VEC2& size, const VEC2& vPoint ) {	// vAreaLT(좌상귀) - size영역에 vPoint가 들어가는지 검사
		return IsArea2( vAreaLT.x, vAreaLT.y, size.w, size.h, vPoint.x, vPoint.y );
	}
	// v1-v2 사각영역안에 vScr좌표가 들어가는지. 아직 size는 지원하지 않는다
	inline BOOL IsInside( const VEC2& vScr, const VEC2& v1, const VEC2& v2/*, const VEC2& size*/ )	{	// 사이즈가 왜 필요하지?
		float l  = xmin( v1.x, v2.x );
		float r = xmax( v1.x, v2.x );
		float t = xmin( v1.y, v2.y );
		float b = xmax( v1.y, v2.y );
		if( vScr.x >= l && vScr.x < r )
			if( vScr.y >= t && vScr.y < b )
				return TRUE;
		return FALSE;
	}
	// Image
#ifdef WIN32
	// 윈도우외에 사용할일 없음.
	void ConvertBlockARGB8888ToARGB4444( WORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc );
	inline void ConvertBlockARGB8888ToARGB4444( WORD *pDst, const XE::POINT& sizeDst
																						, DWORD *pSrc, const XE::POINT& sizeSrc ) {
		ConvertBlockARGB8888ToARGB4444( pDst, sizeDst.w, sizeDst.h, pSrc, sizeSrc.w, sizeSrc.h );
	}
	void ConvertBlockARGB8888ToRGB565( WORD *pDst, int wDst, int hDst
																	, DWORD *pSrc, int wSrc, int hSrc );
	inline void ConvertBlockARGB8888ToRGB565( WORD *pDst, const XE::POINT& sizeDst
																					, DWORD *pSrc, const XE::POINT& sizeSrc ) {
		ConvertBlockARGB8888ToRGB565( pDst, sizeDst.w, sizeDst.h, pSrc, sizeSrc.w, sizeSrc.h );
	}
#endif // WIN32
#ifdef _VER_OPENGL
	// argb8888 -> abgr4444
// 	void ConvertBlockARGB8888ToABGR4444( WORD *pDst, int wDst, int hDst
// 																		, DWORD *pSrc, int wSrc, int hSrc );
// 	inline void ConvertBlockARGB8888ToABGR4444( WORD *pDst, const XE::POINT& sizeDst
// 																						, DWORD *pSrc, const XE::POINT& sizeSrc ) {
// 		ConvertBlockARGB8888ToABGR4444( pDst, sizeDst.w, sizeDst.h, pSrc, sizeSrc.w, sizeSrc.h );
// 	}
	//
	void ConvertBlockARGB8888ToABGR8888( DWORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc );
	void ConvertBlockABGR8888ToABGR4444( WORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc );
	void ConvertBlockABGR8888ToRGBA4444( WORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc );
	void ConvertBlockABGR8888ToRGBA1555( WORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc );
	void ConvertBlockARGB8888ToBGR565( WORD *pDst, int wDst, int hDst		// argb8888 -> bgr565
																	, DWORD *pSrc, int wSrc, int hSrc );
	void ConvertBlockABGR8888ToBGR565( WORD *pDst, int wDst, int hDst
																	, DWORD *pSrc, int wSrc, int hSrc );
	void ConvertBlockABGR8888ToRGB565( WORD *pDst, int wDst, int hDst
																	, DWORD *pSrc, int wSrc, int hSrc );
	inline void ConvertBlockABGR8888ToABGR4444( WORD *pDst, const XE::POINT& sizeDst
		, DWORD *pSrc, const XE::POINT& sizeSrc ) {
		ConvertBlockABGR8888ToABGR4444( pDst, sizeDst.w, sizeDst.h, pSrc, sizeSrc.w, sizeSrc.h );
	}
	inline void ConvertBlockABGR8888ToRGBA4444( WORD *pDst, const XE::POINT& sizeDst
		, DWORD *pSrc, const XE::POINT& sizeSrc ) {
		ConvertBlockABGR8888ToRGBA4444( pDst, sizeDst.w, sizeDst.h, pSrc, sizeSrc.w, sizeSrc.h );
	}
	inline void ConvertBlockARGB8888ToBGR565( WORD *pDst, const XE::POINT& sizeDst
		, DWORD *pSrc, const XE::POINT& sizeSrc ) {
		ConvertBlockARGB8888ToBGR565( pDst, sizeDst.w, sizeDst.h, pSrc, sizeSrc.w, sizeSrc.h );
	}
	inline void ConvertBlockABGR8888ToBGR565( WORD *pDst, const XE::POINT& sizeDst
		, DWORD *pSrc, const XE::POINT& sizeSrc ) {
		ConvertBlockABGR8888ToBGR565( pDst, sizeDst.w, sizeDst.h, pSrc, sizeSrc.w, sizeSrc.h );
	}
	inline void ConvertBlockABGR8888ToRGB565( WORD *pDst, const XE::POINT& sizeDst
		, DWORD *pSrc, const XE::POINT& sizeSrc ) {
		ConvertBlockABGR8888ToRGB565( pDst, sizeDst.w, sizeDst.h, pSrc, sizeSrc.w, sizeSrc.h );
	}
#endif // _VER_OPENGL
//	// abgr8888 -> rgb565
// 	void XE::ConvertBlockABGR8888ToBGR565( WORD *pDst, int wDst, int hDst
// 																			, DWORD *pSrc, int wSrc, int hSrc )
	void NumberToMoneyString( TCHAR acDest[], int nNumber );
	/// 숫자를 K,M,B단위로 쓰는 버전
	void NumberToMoneyStringK( TCHAR acDest[], int nNumber );
	LPCTSTR NumberToMoneyString( int number );
	LPCTSTR NumberToMoneyStringK( int number );
	inline LPCTSTR NtS( int number ) {
		return NumberToMoneyString( number );
	}
	inline LPCTSTR NtSK( int number ) {
		return NumberToMoneyStringK( number );
	}
	template<typename T>
	inline void xSwap( T& v1, T& v2 ) {
		T temp = v1;
		v1 = v2;
		v2 = temp;
	}
	LPCTSTR GetTimeString( TCHAR *pBuff, int len );
	LPCTSTR GetTimeString();
	template<int _Size>
	LPCTSTR GetTimeString( TCHAR (&pBuff)[_Size] ) {
		return GetTimeString( pBuff, _Size );
	}
#ifdef WIN32
	DWORD GetHDDSerial();
#endif
	// num를 4바이트 정렬해서 돌려준다.
	inline int Align4Byte( int num ) {
		if( num == 0 )	return 0;
		if( num & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
			num = (num & ~(0x03)) + 4;		// 비트 절삭하고 4바이트 더 더해줌
		return num;
	}
#ifdef WIN32
	unsigned long long GetCheckSum( LPCTSTR szStr );
	inline unsigned long long GetCheckSum( const _tstring& str ) {
		return GetCheckSum( str.c_str() );
	}
#endif // WIN32
	unsigned long long GetCheckSum( const char* cStr );
	inline unsigned long long GetCheckSum( const std::string& str ) {
		return GetCheckSum( str.c_str() );
	}
	unsigned long long GetCheckSum( const BYTE *pMem, int size );
	void ReplaceReturnChar( TCHAR *szDst, int sizeDst, LPCTSTR szSrc );
	template<int _Size>
	void ReplaceReturnChar( TCHAR (&szDst)[_Size], LPCTSTR szSrc ) {
		return ReplaceReturnChar( szDst, _Size, szSrc );
	}
	int UnZipFile(LPCTSTR szSrcZip, LPCTSTR szSrcFile, LPCTSTR szDstFile = NULL);
	void GetUUID( char *cOut, int len );
	bool GetGUID( std::string* pOutStr );
	template<size_t _Size>
	inline void GetUUID( char (&cOut)[_Size] ) {
		GetUUID( cOut, _Size );
	}
	inline std::string GetUUID() {
		char cUUID[48];
		GetUUID( cUUID );
		return std::string(cUUID);
	}
	inline std::string GetGUID() {
		std::string strGUID;
		GetGUID( &strGUID );
		return strGUID;
	}
//	_tstring GetStrJosa( LPCTSTR szStr, int& idx );
//	_tstring ConvertStrJosa( LPCTSTR szStr );
	void ConvertJosaStr( TCHAR *szDst, int lenDst, LPCTSTR szStr );
	template<size_t _Size>
	void ConvertJosaStr( TCHAR (&szDst)[_Size], LPCTSTR szStr ) {
		ConvertJosaStr( szDst, _Size, szStr );
	}
	template<size_t _Size>
	void ConvertJosaStr( TCHAR( &szDst )[ _Size ], const _tstring& strSrc ) {
		ConvertJosaStr( szDst, _Size, strSrc.c_str() );
	}
#ifdef WIN32
	void ConvertJosaStr( char *szDst, int lenDst, const char *szStr );
#endif
	bool IsBachim( WCHAR wChar );
	bool ReplaceStr( _tstring& strSrc, LPCTSTR szFind, LPCTSTR szReplace );
	void MakeExt( _tstring& strFile, LPCTSTR szExt, LPCTSTR szDefaultName = nullptr );
	int GetNumCR( LPCTSTR szStr );

} // namespace XE
//#define xmax(a, b) ((a) > (b) ? (a) : (b))
//#define xmin(a, b) ((a) < (b) ? (a) : (b))
//#define XCLEAR_ARRAY(BUFF)	memset( BUFF, 0, sizeof(BUFF) )

// 죽는상태를 가지며 삭제되는 객체
class XDestroyObj
{
	XE::xtDestroy m_State;
	void Init() {
		m_State = XE::xDES_LIVE;
	}
	void Destroy() {}
public:
	XDestroyObj() { Init(); }
	virtual ~XDestroyObj() { Destroy(); }
	//
	XE::xtDestroy GetDestroy() {
		return m_State;
	}
	// 죽을준비끝남 상태. 이상태가 되면 돌아오는 다음 메인스레드턴에서 삭제된다.
	void SetDestroyFinal() {
		XBREAK( m_State != XE::xDES_READY );		// 이전상태가 READY가 아니었으면 에러
		m_State =XE::xDES_COMPLETE;
	}
	// 외부로부터 죽을것을 명령받음.
	void DoDestroy() {
		XBREAK( m_State != XE::xDES_LIVE );		// 이미 죽음명령을 받은상태면 경고
		m_State = XE::xDES_READY;
	}
	BOOL IsLive() {
		return (m_State == XE::xDES_LIVE);
	}
	// 죽는중 상태는 모두 TRUE
	BOOL IsDestroy() {
		return (m_State == XE::xDES_READY || m_State == XE::xDES_COMPLETE );
	}
	// 죽음을 명령받은상태냐
	BOOL IsDestroyReady() {
		return (m_State == XE::xDES_READY);
	}
	// 죽음을 위한 처리는 다 끝났고 SAFE_DELETE만 남은 상황
	BOOL IsDestroyFinal() {
		return (m_State == XE::xDES_COMPLETE);
	} 
}; // XDestroyObj

class XCalcFPS
{
	float m_dt;
	int m_frame;
	int m_frameRate;
	DWORD m_dwPrev;
	void Init() {
		m_dt = 0;
		m_frame = 0;
		m_frameRate = 0;
		m_dwPrev = GetTickCount();
	}
	void Destroy() {}
public:
	XCalcFPS() { Init(); }
	virtual ~XCalcFPS() { Destroy(); }

	int GetCurrentFPS() {
		DWORD dwTick = GetTickCount();
		float t = (float)(dwTick - m_dwPrev) / 1000;
		m_dwPrev = dwTick;
		m_dt += t;
		m_frame ++;
		if( m_dt > 1.0f )
		{
			m_frameRate = (int)(m_frame / m_dt);
			m_frame = 0;
			m_dt = 0;
		}

		return m_frameRate;
	}
};

/****************************************************************
* @brief 시드값을 바꾸고 객체가 파괴될때 다시 랜덤 시드로 바꾼다.
* @author xuzhu
* @date	2015/07/16 14:43
*****************************************************************/
class XSeed
{
public:
	XSeed( DWORD seedNew ) { 
		::xSRand( seedNew );
	}
	~XSeed();
	// get/setter
	// public member
// private member
private:
// private method
private:
}; // class XSeed

/****************************************************************
* @brief 초당 실행횟수를 검사한다.
* @author xuzhu
* @date	2016/03/22 12:29
*****************************************************************/
class XFps : public XLock
{
public:
	XFps() 
	: m_aryCntFrame(3), m_aryFps(3) {}
	virtual ~XFps() {}
	// get/setter
//	GET_ACCESSOR_CONST( int, Fps );
	int GetFps( int idx = 0 ) const {
		return m_aryFps[idx];
	}
	// public member
	void Process( int idx = 0 );
	void AddCnt( int size );
private:
	// private member
	std::vector<int> m_aryCntFrame;
	DWORD m_dwStart = 0;
	std::vector<int> m_aryFps;
private:
	// private method
}; // class XFps
