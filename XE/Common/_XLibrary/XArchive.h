#pragma once
#include <string>
#include "xVector.h"
#ifdef _VER_IOS
#include "etc/iOS/xStringiOS.h"
#endif

#ifdef _XCRYPT_PACKET
#define _XARCHIVE_CRYPTO	// 암호화 모듈을 사용하려면 이 디파인을 활성화 시킨다.
#endif

#ifdef _XARCHIVE_CRYPTO
class XCrypto;
#endif

#ifndef MAKE_CHECKSUM
#define		MAKE_CHECKSUM( A )					A << (DWORD) 0x12345678
#endif
#ifndef RESTORE_VERIFY_CHECKSUM
#define		RESTORE_VERIFY_CHECKSUM( A )	{ \
	DWORD __dw1; \
	A >> __dw1; \
	if( XBREAK( __dw1 != 0x12345678 ) ) \
		return FALSE;	\
}
#define		RESTORE_VERIFY_CHECKSUM_NO_RETURN( A )	{ \
	DWORD __dw1; \
	A >> __dw1; \
	if( XBREAK( __dw1 != 0x12345678 ) ) \
		return;	\
}
#endif // not RESTORE_VERIFY_CHECKSUM

/**
 아카이브 구조
 헤더와 패킷으로 나뉜다.
 헤더(4바이트): 패킷의 길이(P byte)
 패킷(P byte): 실제 패킷데이타
 전체 아카이브 사이즈 = 헤더 + 패킷길이
 예) ABCD라는 패킷의 경우
 [00] 4
 [01] 0
 [02] 0
 [03] 0
 [04] 'A'
 [05] 'B'
 [06] 'C'
 [07] 'D'

*/
class XPacketMem;
// 패킷보다 좀더 추상적인 상위개념의 뭐랄까~ 메모리 덩어리?
class XArchive
{
public:
private:
	// 버퍼를 고정으로 잡지말고 최소로 잡은다음 << 할때 메모리가 부족하면 두배씩 다이나믹으로 늘이는게 더 나을듯.
#ifdef _DUMMY_GENERATOR
	enum { _xBUFFER_SIZE = 0x8000 };	// 동적할당이 가능해지면서 private으로 들어옴
#else
	enum { _xBUFFER_SIZE = 8192	};	// 동적할당이 가능해지면서 private으로 들어옴
#endif // _DUMMY_GENERATOR
	enum xtReadMode { xNONE, xREAD, xWRITE };
	enum { COMPRESS_OBJ_IDENTIFIER = 0xff95ab3c };		// 압축 아카이브 식별자.
	std::shared_ptr<BYTE> m_spDynamicBuffer;				///< 동적할당된 아카이브 메모리
	BYTE _m_Buffer[ _xBUFFER_SIZE ];		// 정적할당시 최대 아카이브 크기
	int m_idxCurr;		// 현재 포인터
	int m_idxEnd;			// 버퍼의 마지막 포인터
	int m_verArchiveInstant = 0;	// 스트럭트를 노드로 가진 리스트류를 << >> 할때 Deserialize를 한다면 ver을 넘겨주는 용도.
	BOOL m_bEncryption;		// 암호화된 아카이브인가
//	int m_WriteMode;	// 0:읽기쓰기모두가능(기존호환을 위함. 이제 사용하지 말것.), 1:쓰기용 2:읽기용
	xtReadMode m_Mode;	// 현재 읽기중인지 쓰기 중인지
	bool m_bForDB = false;		///< 편의상 만든 특수한 변수. DB에 저장되는 아카이브인것을 표시.
	void Init() {
		m_bEncryption = FALSE;
		XCLEAR_ARRAY( _m_Buffer );
		m_idxCurr = 4;		// 맨앞 4바이트는 패킷의 총 길이다
		m_idxEnd = _xBUFFER_SIZE - 1;	// 디폴트로는 정적메모리의 크기
//		m_WriteMode = 0;
		m_Mode = xNONE;
	}
	void Destroy();
	// 헤더까지 포함한 순수 메모리 버퍼(내부용)
	const BYTE* GetBufferMem( void ) const {
		if( m_spDynamicBuffer )
			return m_spDynamicBuffer.get();
		return _m_Buffer;
	}
	// 수정가능한 버퍼포인터를 얻고 싶을때.
	BYTE* GetBufferMemMutable( void ) const {
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		return pBuff;
	}
public:
	// 이제 이 생성자는 사용하지 말것. 파라메터 있는 버전을 사용할것.
	XArchive() { Init();	}
	XArchive( int sizeMax );
	XArchive( int sizeMax, ID idPacket );
	XArchive( XPacketMem *pPacketMem );
	virtual ~XArchive() { Destroy(); }

	GET_SET_ACCESSOR( int, verArchiveInstant );
	// 헤더까지 포함한 순수 메모리 버퍼(외부용)
	const BYTE* GetBuffer( void ) const {
		if( m_spDynamicBuffer )
			return m_spDynamicBuffer.get();
		return _m_Buffer;
	}
	// 버퍼를 다이나믹으로 전환하고 현재 버퍼의 두배크기로 재할당받은후 기존데이타를 카피한다.
	void ReAllocBuffer();
	void ReAllocBuffer( int sizeBuf );
	bool IsOverBuffer( int sizeWillStore = 16 ) {
		return (m_idxCurr + sizeWillStore >= GetMaxSizeArchive() );	// 안전하게 현재 포인터에서 16바이트 더한크기로 함.
	}
	void SetBufferMem( const BYTE* pSrc, int size ) {
		XBREAK( size <= 0 );
		CheckReallocArchive( size );
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		XBREAK( GetMaxSizeArchive() <= size );
		memcpy_s( pBuff, GetMaxSizeArchive(), pSrc, size );
		m_idxCurr = 4;
	}
	// 패킷길이 헤더4바이트를 제외한 순수 패킷포인터
	BYTE* GetBufferPacket( void ) { 
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		return pBuff + 4;		
	}

	int GetMaxBufferSize( void ) {	
		return GetMaxSizeArchive() - 4;
	}
	// 패킷길이를 포함한 버퍼전체 사이즈
	int size( void ) const {
		int lenPacket = GetPacketLength();	// 헤더제외한 순수 패킷길이
		return lenPacket + 4;	// 헤더까지 포함해서
	}
	int GetCurrPos( void ) {
		return m_idxCurr;
	}
	bool IsStart() const {
		return m_idxCurr == 4;
	}
	/// 현재 위치를 포인터로 반환
	BYTE* GetCurrPtr( void ) {
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		return pBuff + m_idxCurr;
	}
	int GetLeftSize( void ) {
		return m_idxEnd - m_idxCurr;
	}
	/// 아카이브 버퍼의 최대 크기
	int GetMaxSizeArchive( void ) const {
		return m_idxEnd + 1;
	}
	/// 현재 위치에서 남은 아카이브 사이즈
	int GetLeftSizeArchive( void ) {
		return GetPacketLength() - (m_idxCurr - 4);		// 이게 맞나?
	}
	// 현재 위치가 패킷의 끝인가.
	BOOL IsEop( void ) {
		int totalSize = GetPacketLength() + 4;
		if( size() >= totalSize )
			return TRUE;
		return FALSE;
	}
	void SetPacketLength( DWORD byteLen ) {
		*((DWORD*)GetBufferMem()) = byteLen;
		m_idxCurr = 4;
	}
	DWORD GetPacketLength( void ) const {
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		return *((DWORD*)pBuff);
	}
	bool IsHave() {
		return GetPacketLength() != 0;
	}
	bool IsEmpty() {
		return !IsHave();
	}
	void Clear( void ) {
		SetPacketLength( 0 );
		m_idxCurr = 4;
	}
	void CurrMoveToStart( void ) {
		m_idxCurr = 4;
	}
	DWORD GetPacketHeader( void ) {
		DWORD *ptr = (DWORD*)GetBufferPacket();
		return *ptr;
	}
#ifdef _XARCHIVE_CRYPTO
	GET_ACCESSOR( BOOL, bEncryption );
	BOOL DoCrypto( XCrypto *pCrypter, DWORD dwParam );
	BOOL DoDecrypto( XCrypto *pCrypter );
	// m_Buffer의 내용이 암호화되었는가 아닌가 표시한다.
	void IsCryptThenSetFlag( void );
#endif
// 	XArchive& operator = ( const XArchive& rhs ) {
// 
// 	}
	//
	XArchive& operator << ( int d ) {
		return *this << (long)d;
	}
	XArchive& operator << ( unsigned long d ) {
		return *this << (long)d;
	}
	XArchive& operator << ( unsigned int d ) {
		return *this << (long)d;
	}
	XArchive& operator << ( unsigned char d ) {
		return *this << (char)d;
	}
	XArchive& operator << ( unsigned short d ) {
		return *this << (short)d;
	}

	XArchive& operator << ( long d ) {
//		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		// 버퍼 오버플로우 검사할것
		if( IsOverBuffer() )
			ReAllocBuffer();
		*((long *)(GetBufferMem()+m_idxCurr)) = d;
		m_idxCurr += sizeof(long);
    XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬
		XBREAK( m_idxCurr >= m_idxEnd );
		// 버퍼 맨앞에 패킷의 총길이를 넣는다
		AddPacketLength( sizeof(long) );
		return *this;
	}
	XArchive& operator << ( short d ) {
//		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		if( IsOverBuffer() )
			ReAllocBuffer();
		*((short *)(GetBufferMem()+m_idxCurr)) = d;
		m_idxCurr += sizeof(short);
		XBREAK( m_idxCurr >= m_idxEnd );
    XBREAK( m_idxCurr & 0x01 );     // 2바이트 정렬
		AddPacketLength( sizeof(short) );
		return *this;
	}
	XArchive& operator << ( char d ) {
		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		if( IsOverBuffer() )
			ReAllocBuffer();
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		*(pBuff+m_idxCurr) = d;
		m_idxCurr += sizeof(char);
		XBREAK( m_idxCurr >= m_idxEnd );
		AddPacketLength( sizeof(char) );
		return *this;
	}
	XArchive& operator << ( float d ) {
		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		if( IsOverBuffer() )
			ReAllocBuffer();
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		*((float *)(pBuff+m_idxCurr)) = d;
		m_idxCurr += sizeof(float);
		XBREAK( m_idxCurr >= m_idxEnd );
    XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬
		AddPacketLength( sizeof(float) );
		return *this;
	}
	XArchive& operator << ( double d ) {
		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		if( IsOverBuffer() )
			ReAllocBuffer();
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		*((double *)(pBuff+m_idxCurr)) = d;
		m_idxCurr += sizeof(double);
		XBREAK( m_idxCurr >= m_idxEnd );
//     XBREAK( m_idxCurr & 0x07 );
		AddPacketLength( sizeof(double) );
		return *this;
	}
	XArchive& operator << ( unsigned long long d ) {
		*this << (long long)d;
		return *this;
	}
	XArchive& operator << ( long long d ) {
		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		// 버퍼 오버플로우 검사할것
		if( IsOverBuffer() )
			ReAllocBuffer();
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		*((long long*)(pBuff+m_idxCurr)) = d;
		m_idxCurr += sizeof(long long);
		XBREAK( m_idxCurr >= m_idxEnd );
		// 버퍼 맨앞에 패킷의 총길이를 넣는다
		AddPacketLength( sizeof(long long) );
		return *this;
	}
	XArchive& operator << ( LPCTSTR szStr ) {
		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		WriteString( szStr );
		return *this;
	}
	XArchive& operator << ( char *cStr ) {
		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		WriteString( cStr );
		return *this;
	}
	XArchive& operator << ( const _tstring& tstr ) {
		*this << tstr.c_str();
		return *this;
	}
	XArchive& operator << ( _tstring& tstr ) {
		*this << tstr.c_str();
		return *this;
	}
#ifdef WIN32
	XArchive& operator << ( const char *cStr ) {
		WriteString( cStr );
		return *this;
	}
	XArchive& operator << ( const std::string& str ) {
		*this << str.c_str();
		return *this;
	}
	XArchive& operator << ( std::string& str ) {
		*this << str.c_str();
		return *this;
	}
#endif
	/// sizeSrc를 아카이빙하려고 하는데 충분한 버퍼가 있는지 검사한다.
	/// sizeSrc가 소스측의 전체 크기가 아님을 주의. 그래서 계속 2배씩 늘여가며 검사하는 것임.
	inline void CheckReallocArchive( const int sizeSrc, const int bytesSpare = 16 ) {
		int __cnt = 5;
		while( __cnt-- ) {
			if( IsOverBuffer( sizeSrc + bytesSpare ) ) {	// 걍 안전을 위해 spare바이터 더함.
				ReAllocBuffer();
				const int sizeThis = GetMaxSizeArchive();
//				XBREAK( /*__cnt == 0 &&*/ sizeThis > 0x10000 );		// 64k 이상의 크기는 확인해봐야함.
			} else
				break;
		}
	}
	XArchive& operator << ( XArchive& p ) {
		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		int sizeSrc = p.size();
		XBREAK( sizeSrc & 0x03 ); // 아카이브는 정렬되어있어야 한다.
		CheckReallocArchive( sizeSrc );
		int leftsize = GetLeftSize();
		XBREAK( sizeSrc >= leftsize);
		XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사
		memcpy_s( (void*)(GetBufferMem()+m_idxCurr), leftsize, (void*)p.GetBufferMem(), sizeSrc );
		m_idxCurr += sizeSrc;
		XBREAK( m_idxCurr >= m_idxEnd );
		AddPacketLength( sizeSrc );
		return *this;
	}
	XArchive& operator << ( const XArchive& p ) {
		//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
		int sizeSrc = p.size();
		XBREAK( sizeSrc & 0x03 ); // 아카이브는 정렬되어있어야 한다.
		CheckReallocArchive( sizeSrc );
		int leftsize = GetLeftSize();
		XBREAK( sizeSrc >= leftsize );
		XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사
		memcpy_s( (void*)(GetBufferMem()+m_idxCurr), leftsize, (void*)p.GetBufferMem(), sizeSrc );
		m_idxCurr += sizeSrc;
		XBREAK( m_idxCurr >= m_idxEnd );
		AddPacketLength( sizeSrc );
		return *this;
	}
	XArchive& operator >> ( XArchive& p );
	XArchive& operator << ( const XE::VEC2& v ) {
		*this << v.x;
		*this << v.y;
		return *this;
	}
	XArchive& operator << ( XE::VEC2& v ) {
		*this << v.x;
		*this << v.y;
		return *this;
	}
	XArchive& operator >> ( XE::VEC2& v ) {
		*this >> v.x;
		*this >> v.y;
		return *this;
	}
	XArchive& operator << ( const XE::VEC3& v ) {
		*this << v.x;
		*this << v.y;
		*this << v.z;
		return *this;
	}
	XArchive& operator << ( XE::VEC3& v ) {
		*this << v.x;
		*this << v.y;
		*this << v.z;
		return *this;
	}
	XArchive& operator >> ( XE::VEC3& v ) {
		*this >> v.x;
		*this >> v.y;
		*this >> v.z;
		return *this;
	}
	void AddPacketLength( int size ) {
		int *pSize = (int*)GetBufferMem();
		*pSize = *pSize + size;
		m_Mode = xWRITE;
	}
	void WriteBuffer( void *pSrc, int _size );
	// 버퍼사이즈를 앞에 저장하지 않는 버전
	void WriteBufferNoHeader( void *pSrc, int _size );
	// 서버에서 보낼땐 utf16로. iOS에서 받을땐 utf16를 받아 UTF8로 변환해서 사용
	// 클라에서 보낼땐 UTF8로 보내서 서버에서 utf16로 변환. iOS에서 utf8->utf16변환방법을 아직 모르니 일단 이렇게 해야함.
	void WriteString( LPCTSTR szStr );
#ifdef WIN32
	void WriteString( const char *cStr );
#endif
	template<size_t _Size>
	BOOL ReadString( TCHAR (&szBuffer)[_Size] ) {
		return ReadString( szBuffer, _Size );
	}
	template<size_t _Size>
	BOOL ReadBuffer( BYTE (&szBuffer)[_Size], int *pOutSizeBuffer ) {
		return ReadBuffer( szBuffer, _Size, pOutSizeBuffer );
	}

	int ReadBufferSize( void );
	BOOL ReadBuffer( void *pDst, int dstSize, int *sizeRead );
	BOOL ReadString( TCHAR *szBuffer, int lenBuffer );
//	bool ReadString( void *pBuffer, int sizeBuff );
	///////////////////////////////////////////////////////
	// 현재까지 쓰기모드 상태였는데 읽기모드로 시도하려고 하면 첨부터 다시 읽게 바꾼다.
	inline void CheckMode( void ) {
		if( m_Mode == xWRITE )
			m_idxCurr = 4;
		m_Mode = xREAD;
	}
	XArchive& operator >> ( unsigned long long& d ) {
		long long llVal;
		*this >> llVal;
		d = (unsigned long long)llVal;
		return *this;
	}
	XArchive& operator >> ( long long& d ) {
		//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
		CheckMode();
		XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사. 이건 8바이트 정렬해야 하는거 아닌가?
		XBREAK( m_idxCurr - 4 >= (int)GetPacketLength() );	// 메모리의 첫 4바이트는 패킷의 길이이므로 4바이트 빼야 순수 패킷위치가 나옴.
		d = *((long long*)(GetBufferMem()+m_idxCurr));
		m_idxCurr += sizeof(long long);
		return *this;
	}
	XArchive& operator >> ( float& d ) {
		//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
		CheckMode();
		XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사(정렬을 해야하는 이유는 iOS에서 정렬안된 어드레스에서 읽으면 에러나기때문)
		XBREAK( m_idxCurr - 4 >= (int)GetPacketLength() );	// 메모리의 첫 4바이트는 패킷의 길이이므로 4바이트 빼야 순수 패킷위치가 나옴.
		d = *((float*)(GetBufferMem()+m_idxCurr));
		m_idxCurr += sizeof(float);
		return *this;
	}
	XArchive& operator >> ( double& d ) {
		//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
		CheckMode();
		XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사(정렬을 해야하는 이유는 iOS에서 정렬안된 어드레스에서 읽으면 에러나기때문)
		XBREAK( m_idxCurr - 4 >= (int)GetPacketLength() );	// 메모리의 첫 4바이트는 패킷의 길이이므로 4바이트 빼야 순수 패킷위치가 나옴.
		d = *((double*)(GetBufferMem()+m_idxCurr));
		m_idxCurr += sizeof(double);
		return *this;
	}
	XArchive& operator >> ( long& d ) {
		//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
		CheckMode();
		XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사
		XBREAK( m_idxCurr - 4 >= (int)GetPacketLength() );	// 메모리의 첫 4바이트는 패킷의 길이이므로 4바이트 빼야 순수 패킷위치가 나옴.
		d = *((long*)(GetBufferMem()+m_idxCurr));
		m_idxCurr += sizeof(long);
		return *this;
	}
	XArchive& operator >> ( short& d ) {
		//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
		CheckMode();
		XBREAK( m_idxCurr & 0x01 );		// 2byte정렬 검사
		XBREAK( m_idxCurr - 4 >= (int)GetPacketLength() );	// 메모리의 첫 4바이트는 패킷의 길이이므로 4바이트 빼야 순수 패킷위치가 나옴.
		d = *((short*)(GetBufferMem()+m_idxCurr));
		m_idxCurr += sizeof(short);
		return *this;
	}
	XArchive& operator >> ( char& d ) {
		//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
		CheckMode();
		XBREAK( m_idxCurr - 4 >= (int)GetPacketLength() );	// 메모리의 첫 4바이트는 패킷의 길이이므로 4바이트 빼야 순수 패킷위치가 나옴.
		d = *((char*)(GetBufferMem()+m_idxCurr));
		m_idxCurr += sizeof(char);
		return *this;
	}
	//
	XArchive& operator >> ( int& d ) {
		return *this >> ((long&)d);
	}
	XArchive& operator >> ( unsigned long& d ) {
		return *this >> ((long&)d);
	}
	XArchive& operator >> ( unsigned int& d ) {
		return *this >> ( (long&)d );
	}
	XArchive& operator >> ( unsigned char& d ) {
		return *this >> ((char&)d);
	}
	XArchive& operator >> ( unsigned short& d ) {
		return *this >> ((short&)d);
	}
	XArchive& operator >> ( _tstring& tstr ) {
		TCHAR szBuff[0x8000];
		ReadString(szBuff);
		tstr = szBuff;
		return *this;
	}
#ifdef WIN32
	// 기존에 이미 std::string으로 <<하고 서버에서 _tstring으로 >>하는 부분이 많아서 일단 접음.
// 	XArchive& operator >> ( std::string& cstr ) {
// 		char cBuff[ 0x8000 ];
// 		ReadString( (void*)cBuff, sizeof(cBuff) );
// 		cstr = cBuff;
// 		return *this;
// 	}
	XArchive& operator >> ( std::string& cstr );
#endif // WIN32
	template<typename T>
	XArchive& operator << ( T& rhs ) {
		rhs.Serialize( *this );
		return *this;
	}
	template<typename T>
	XArchive& operator << ( const T& rhs ) {
		rhs.Serialize( *this );
		return *this;
	}
	// 버전정보가 함께 넘어가야한다면 사전에 버전값을 넣어두어야 함.
	template<typename T>
	XArchive& operator >> ( T& rhs ) {
		rhs.DeSerialize( *this, m_verArchiveInstant );
		return *this;
	}
	//
	XArchive& operator = ( const XArchive& rhs ) {
		const int sizeRhs = rhs.size();
		CheckReallocArchive( sizeRhs );
		XBREAK( GetMaxSizeArchive() <= sizeRhs );
		memcpy_s( (void*)GetBufferMem(), GetMaxSizeArchive()
															, (void*)rhs.GetBuffer(), sizeRhs );
		m_idxCurr = 4;
		return *this;
	}
	template<typename T, int N>
	XArchive& operator << ( XArrayLinearN<T, N>& ary ) {
		int size = ary.size();
		*this << size;
		XARRAYLINEARN_LOOP( ary, T, elem ) {
			*this << elem;
		} END_LOOP;
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T, int N>
	XArchive& operator << ( const XArrayLinearN<T, N>& ary ) {
		int size = ary.size();
		*this << size;
		XARRAYLINEARN_LOOP( ary, T, elem ) {
			*this << elem;
		} END_LOOP;
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T, int N>
	XArchive& operator >> ( XArrayLinearN<T, N>& ary ) {
		int size;
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			ary.Add( data );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T, int N>
	XArchive& operator << ( XArrayN<T, N>& ary ) {
		int size = ary.GetMax();
		*this << size;
		XARRAYN_LOOP( ary, T, elem ) {
			*this << elem;
		} END_LOOP;
		XBREAK( sizeof(T) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T, typename A, int N>
	XArchive& operator << ( XArrayN<T, N>& ary ) {
		int size = ary.GetMax();
		*this << size;
		XARRAYN_LOOP( ary, T, elem ) {
			*this << (A)elem;
		} END_LOOP;
		XBREAK( sizeof(T) != 4 );		// 이경우 xuzhu에게 문의.. 4바이트 정렬해야하는데 귀찮아서 안만듬.
		return *this;
	}
	template<typename T, int N>
	XArchive& operator >> ( XArrayN<T, N>& ary ) {
		int size;
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			ary[ i ] = data;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T, typename A, int N>
	XArchive& operator >> ( XArrayN<T, N>& ary ) {
		int size;
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			A data;
			*this >> data;
			ary[ i ] = (T)data;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( XArrayLinear<T>& ary ) {
		int size = ary.size();
		*this << size;
		XARRAYLINEAR_LOOP( ary, T, elem ) {
			*this << elem;
		} END_LOOP;
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( const XArrayLinear<T>& ary ) {
		int size = ary.size();
		*this << size;
		XARRAYLINEAR_LOOP( ary, T, elem ) {
			*this << elem;
		} END_LOOP;
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator >> ( XArrayLinear<T>& ary ) {
		int size;
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			ary.Add( data );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( XList<T>& list ) {
		int size = list.size();
		*this << size;
		XLIST_LOOP2_H( list, T, pElem ) {
			*this << (*pElem);
		} END_LOOP;
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator >> ( XList<T>& list ) {
		int size;
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			list.Add( data );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( const XList4<T>& list ) {
		int size = list.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( const T& pElem: list ) {
			*this << pElem;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( XList4<T>& list ) {
		int size = list.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( T& pElem : list ) {
			*this << pElem;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = (sizeAligned & ~(0x03)) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator >> ( XList4<T>& list ) {
		int size;
		list.clear();
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			list.Add( data );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( const std::list<T>& list ) {
		int size = list.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( const T& pElem : list ) {
			*this << pElem;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = (sizeAligned & ~(0x03)) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( std::list<T>& list ) {
		int size = list.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( const T& pElem : list ) {
			*this << pElem;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = (sizeAligned & ~(0x03)) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator >> ( const std::list<T>& list ) {
		int size;
		list.clear();
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			list.push_back( data );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = (sizeAligned & ~(0x03)) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( const std::vector<T>& ary ) {
		int size = ary.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( const T& pElem: ary ) {
			*this << pElem;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( std::vector<T>& ary ) {
		int size = ary.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( T& pElem: ary ) {
			*this << pElem;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator >> ( std::vector<T>& ary ) {
		int size;
		ary.clear();
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			ary.push_back( data );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( XVector<T>& ary ) {
		int size = ary.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( const T& pElem: ary ) {
			*this << pElem;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator >> ( XVector<T>& ary ) {
		int size;
		ary.clear();
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			ary.push_back( data );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator << ( const XVector<T>& ary ) {
		int size = ary.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( const T& pElem: ary ) {
			*this << pElem;
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
		return *this;
	}
	template<typename T>
	XArchive& operator >> ( const XVector<T>& ary ) {
		int size;
		ary.clear();
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			T data;
			*this >> data;
			ary.push_back( data );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
		return *this;
	}
	template<typename T>
	void DeSerializeSharedPtr( XVector<T>& ary, int ver ) {
		int size;
		ary.clear();
		*this >> size;
		for( int i = 0; i < size; ++i ) {
			auto spElem = T();
			spElem->DeSerialize( *this, ver );
			ary.push_back( spElem );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			BYTE b0;
			for( int i = 0; i < over; ++i )
				*this >> b0;
		}
	}
// 	template<typename T>
// 	void DeSerializePtr( XVector<T>& ary, int ver ) {
// 		int size;
// 		ary.clear();
// 		*this >> size;
// 		for( int i = 0; i < size; ++i ) {
// 			auto pElem = new T();
// 			pElem->DeSerialize( *this, ver );
// 			ary.push_back( pElem );
// 		}
// 		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
// 		if( sizeof( T ) == 1 ) {
// 			int sizeAligned = size;
// 			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
// 				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
// 			int over = sizeAligned - size;
// 			BYTE b0;
// 			for( int i = 0; i < over; ++i )
// 				*this >> b0;
// 		}
// 	}
	template<typename T>
	void SerializePtr( const XVector<T>& ary ) {
		int size = ary.size();
		XBREAK( size > 0xffff );
		*this << size;
		for( const T& pElem : ary ) {
			pElem->Serialize( *this );
		}
		XBREAK( sizeof( T ) == 2 );		// 아직 이처리는 없음.
		if( sizeof( T ) == 1 ) {
			int sizeAligned = size;
			if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
				sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
			int over = sizeAligned - size;
			for( int i = 0; i < over; ++i )
				*this << (BYTE)0;
		}
	}
	/**
	 arSrc의 현재 위치부터 아카이브 끝까지 데이타를 모두 this로 옮겨온다.
	*/
	void MoveFromArchive( XArchive& arSrc ) {
		BYTE *pStart = arSrc.GetCurrPtr();
		const int sizeLeftSrc = arSrc.GetLeftSizeArchive();
		const int sizeLeftDst = GetMaxBufferSize() - m_idxCurr;
		CheckReallocArchive( sizeLeftSrc );
		XBREAK( sizeLeftDst <= sizeLeftSrc );
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		memcpy_s( pBuff+m_idxCurr, sizeLeftDst, pStart, sizeLeftSrc );
		AddPacketLength( sizeLeftSrc );
		m_idxCurr += sizeLeftSrc;
		XBREAK( m_idxCurr >= m_idxEnd );
	}
	/**
	 @brief 아카이브 데이타를 pDst에 카피한다.
	 @param pDst 복사받을 메모리 주소
	 @param sizeDst pDst의 바이트사이즈
	 @param offset 복사할 아카이브의 시작 바이트오프셋
	 @param sizeCopy 복사할 아카이브의 바이트크기
	*/
	bool CopyToBinary( BYTE *pDst, int sizeDst, int offset, int sizeCopy ) {
		const int sizeArchive = GetMaxSizeArchive();
		if( offset >= sizeArchive )
			return false;
		XBREAK( sizeDst < sizeCopy );
// 		XBREAK( offset + sizeCopy > GetMaxSizeArchive() );
		// 오프셋에서 카피해야할 크기가 전체 크기를 벗어나면.
		if( offset + sizeCopy > sizeArchive ) {		
			sizeCopy = sizeArchive - offset;		// 남은 아카이브만 카피한다.
			memcpy_s( pDst, sizeDst, (BYTE*)(GetBufferMem() + offset), sizeCopy );
			return false;
		} else {
			memcpy_s( pDst, sizeDst, (BYTE*)(GetBufferMem() + offset), sizeCopy );
			return true;
		}
	}
	template<size_t size>
	bool CopyToBinary( BYTE (&pDst)[size], int offset, int sizeCopy ) {
		return CopyToBinary( pDst, size, offset, sizeCopy );
	}
	/**
	 @brief 외부 바이너리로부터 아카이브 메모리를 채운다.
	 @param offsetDst 카피받을 아카이브측 오프셋
	 @param pSrc 외부 바이너리 포인터
	 @param sizeSrc 외부 바이너리 크기
	*/
	void CopyFromBinary( int offsetDst, BYTE* pSrc, int sizeSrc ) {
		XBREAK( sizeSrc <= 0 );
		const int sizeLeft = GetMaxSizeArchive() - offsetDst;
		if( sizeLeft < sizeSrc ) {
			ReAllocBuffer( sizeSrc + offsetDst );
		}
		const int sizeBuff = GetMaxSizeArchive();
		XBREAK( offsetDst >= sizeBuff );
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		memcpy_s( pBuff + offsetDst, sizeBuff - offsetDst, pSrc, sizeSrc );
		m_idxCurr = 4;
	}
	bool IsForDB() {
		// 읽기모드에선 쓰지말것. 
		// 읽기모드에서 쓰려면 명시적으로 m_bForDB를 true로 하고 써야하는데 실수로 누락할수가 있음.
		// 따라서 읽기모드시 이런걸 신경안쓰고도 읽어지게끔 serialize를 해야함.
		XBREAK( m_Mode == xREAD );	
		return m_bForDB;
	}
	SET_ACCESSOR( bool, bForDB );
	int DoCompress();
	bool DoUnCompress();
}; // XArchive

class XPacketMem
{
	BYTE *m_Buffer;
	int m_Size;
	void Init() {
		m_Buffer = NULL;
		m_Size = 0;
	}
	void Destroy() {
		SAFE_DELETE_ARRAY( m_Buffer );
	}
public:
	XPacketMem( XArchive& arSrc );
	virtual ~XPacketMem() { Destroy(); }
	//
	GET_ACCESSOR( int, Size );
	GET_ACCESSOR( const BYTE*, Buffer );
};


