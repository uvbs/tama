#include "stdafx.h"
#include "XArchive.h"
#ifdef _XARCHIVE_CRYPTO
#pragma message("define _XARCHIVE_CRYPTO")
#include "XCrypto.h"
#endif
#include "XSystem.h"

#ifdef _XCRYPT_PACKET
#pragma message("define _XCRYPT_PACKET")
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XArchive::XArchive( int sizeMax )
{
	Init();
	auto pBuff = new BYTE[ sizeMax ];
	m_spDynamicBuffer = std::shared_ptr<BYTE>( pBuff );
	*((long*)pBuff) = 0;		// 젤앞에 4바이트 아카이브 사이즈 부분만 클리어해도 됨.
	m_idxEnd = sizeMax - 1;
}

XArchive::XArchive( XPacketMem *pPacketMem )
{
	Init();
	// pPacketMem으로부터 패킷내용을 복사해온다.
	SetBufferMem( (BYTE*)pPacketMem->GetBuffer(), pPacketMem->GetSize() );
}

void XArchive::Destroy( void )
{
//	SAFE_DELETE_ARRAY( m_pDynamicBuffer );
}

#ifdef WIN32
XArchive& XArchive::operator >> ( std::string& cstr ) {
	_tstring strt;
	( *this ) >> strt;
	cstr = SZ2C(strt);
	return *this;
}
#endif // WIN32

XArchive& XArchive::operator >> ( XArchive& p ) {
	//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
	CheckMode();
	XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사
	XBREAK( m_idxCurr - 4 >= (int)GetPacketLength() );	// 메모리의 첫 4바이트는 패킷의 길이이므로 4바이트 빼야 순수 패킷위치가 나옴.
	int sizePacket = *( (int*)( GetBufferMem() + m_idxCurr ) );		// 패킷길이(메모리 길이 아님)
	XBREAK( sizePacket & 0x03 ); // 아카이브는 정렬되어있어야 한다.
	m_idxCurr += sizeof( int );
	XBREAK( m_idxCurr >= m_idxEnd );
	if( sizePacket > 0 ) {
		// 아카이브패킷이 없으면 들어올필요 없다.
		XBREAK( m_idxCurr - 4 >= (int)GetPacketLength() );	// 메모리의 첫 4바이트는 패킷의 길이이므로 4바이트 빼야 순수 패킷위치가 나옴.
		CheckReallocArchive( sizePacket );
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		p.WriteBufferNoHeader( pBuff + m_idxCurr, sizePacket );
		p.CurrMoveToStart();	// 14.07.03추가됨. 아카이브에서 아카이브를 빼냈을때는 포인터가 처음으로 가있다고 가정했음.
		m_idxCurr += sizePacket;
	}
	return *this;
}

/**
 @brief 버퍼를 다이나믹으로 전환하고 현재 버퍼의 두배크기로 재할당받은후 기존데이타를 카피한다.
*/
void XArchive::ReAllocBuffer()
{
	const int size = GetMaxSizeArchive();
	const int sizex2 = size * 2;
	ReAllocBuffer( sizex2 );
}

void XArchive::ReAllocBuffer( int sizeBuf )
{
	const int sizeOld = GetMaxSizeArchive();
	const int sizeSrc = size();
	XBREAK( sizeBuf <= sizeSrc );
	auto pBuff = new BYTE[ sizeBuf ];
	XBREAK( pBuff == nullptr );
	// 새 버퍼에 기존 내용을 카피.
	if( m_spDynamicBuffer ) {
		memcpy_s( pBuff, sizeBuf, m_spDynamicBuffer.get(), sizeSrc );
	}	else {
		memcpy_s( pBuff, sizeBuf, _m_Buffer, sizeSrc );
	}
	// 기존 다이나믹 버퍼가 있었으면 삭제
//	SAFE_DELETE_ARRAY( m_pDynamicBuffer );
	m_spDynamicBuffer.reset();
	m_spDynamicBuffer = std::shared_ptr<BYTE>( pBuff );
	m_idxEnd = sizeBuf - 1;
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
#ifndef _DUMMY_GENERATOR
//	CONSOLE("XArchive::ReAllocBuffer: old_size=%d new_size=%d", sizeOld, sizeBuf );
#endif // not _DUMMY_GENERATOR
#endif
}
// iOS나 Android에서는 이것은 UTF-8이 된다.
void XArchive::WriteString( LPCTSTR szStr ) 
{
	//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
	const int len = _tcslen( szStr ) + 1;
	int size = len * sizeof( TCHAR );
	// 4byte 정렬
	if( size & 0x03 )
		size = ( size & ~( 0x03 ) ) + 4;
	XBREAK( m_idxCurr & 0x03 );
	XBREAK( size > 0xffff );
	CheckReallocArchive( size );
	const int leftsize = GetLeftSize();
	XBREAK( leftsize <= size + 4 );
	*( (WORD*)( GetBufferMem() + m_idxCurr ) ) = (WORD)size;
#if defined(_VER_IOS) || defined(_VER_ANDROID)
	*((WORD*)(GetBufferMem()+m_idxCurr+2)) = (WORD)2;		// UTF-8
#else // not win32
#ifdef WIN32
	*( (WORD*)( GetBufferMem() + m_idxCurr + 2 ) ) = (WORD)1;		// UTF-16
#else
#error "unknown platform"
#endif
#endif // win32
	m_idxCurr += sizeof( int );
	XBREAK( m_idxCurr + size >= m_idxEnd );
	memcpy_s( (void*)( GetBufferMem() + m_idxCurr ), leftsize
					, (void*)szStr, len * sizeof( TCHAR ) );
	m_idxCurr += size;
	XBREAK( m_idxCurr >= m_idxEnd );
	AddPacketLength( size + 4 );
}

#ifdef WIN32
void XArchive::WriteString( const char *cStr ) 
{
	//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
	const int len = strlen( cStr ) + 1;
	int size = len * sizeof( char );		// 널 포함
	// 4byte 정렬
	if( size & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
		size = ( size & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
	XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사
	XBREAK( size > 0xffff );		// 일단 64k이상의 스트링은 못보내는걸로 하자.
	CheckReallocArchive( size );
	const int leftsize = GetLeftSize();
	XBREAK( leftsize <= size + 4 );
	*( (WORD*)( GetBufferMem() + m_idxCurr ) ) = (WORD)size;		// 스트링의 메모리사이즈(널 포함)
	*( (WORD*)( GetBufferMem() + m_idxCurr + 2 ) ) = (WORD)3;		// 텍스트 인코딩(EUCKR)
	m_idxCurr += sizeof( int );		// 스트링패킷의 맨앞 사이즈 부분을 건너뜀
	XBREAK( m_idxCurr + size >= m_idxEnd );
	BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
	memcpy_s( (void*)( pBuff + m_idxCurr ), leftsize
					, (void*)cStr, len * sizeof( char ) );	// 스트링 길이부분만 정확하게 메모리 카피함
	m_idxCurr += size;	// 스트링 사이즈(정렬된) 건너뜀
	XBREAK( m_idxCurr >= m_idxEnd );
	AddPacketLength( size + 4 );		// 총 패킷길이에 더함.
}
#endif

BOOL XArchive::ReadString( TCHAR *szBuffer, int lenBuffer )
{
	//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
	CheckMode();
	szBuffer[ 0 ] = 0;		// 실패하더라도 버퍼는 클리어 되게
	XBREAK( m_idxCurr & 0x03 );		// 4byte정렬 검사
	WORD sizeBytes = *( (WORD*)( GetBufferMem() + m_idxCurr ) );		// 정렬된 스트링 메모리 byte 사이즈. 널 포함
	XBREAK( sizeBytes & 0x03 );		// 4byte정렬 검사
	WORD encode = *( (WORD*)( GetBufferMem() + m_idxCurr + 2 ) );
	m_idxCurr += sizeof( int );
	XBREAK( m_idxCurr >= m_idxEnd );
	if( encode == 1 )		// utf16
	{
#ifdef WIN32
		//윈32는 utf16으로 준것을 UTF16으로 그대로 받는다.
		XBREAK( sizeof( TCHAR ) != 2 );
		if( XBREAKF( sizeBytes >= lenBuffer * sizeof( WCHAR ), "sizeBytes(%d) >= szBuffer SizeBytes(%d)", sizeBytes, lenBuffer * sizeof( WCHAR ) ) )
			return FALSE;
		memcpy_s( (void*)szBuffer, lenBuffer * sizeof( WCHAR )
						, (void*)( GetBufferMem() + m_idxCurr ), sizeBytes );
		szBuffer[ sizeBytes * sizeof(TCHAR) ] = 0;
#else
		//기기에서는 UTF16으로 받은것을 UTF8로 변환해야 한다.
		UNICHAR uszUtf16[0x8000];
		if( XBREAK(sizeBytes >= sizeof(uszUtf16)) )
			return FALSE;
		memcpy_s( (void*)uszUtf16, sizeof(uszUtf16), (void*)(GetBufferMem()+m_idxCurr), sizeBytes);
		char buff[0x8000];
		LPCTSTR szUTF8 = _ConvertUTF16ToUTF8( buff, uszUtf16 );
		// utf8로 변환한 길이가 받을 버퍼보다 크면 에러
		if( XBREAK(strlen(szUTF8) >= lenBuffer) )
			return FALSE;
		strcpy_s( szBuffer, lenBuffer, szUTF8 );
#endif
	}	else 
	if( encode == 2 ) {	// utf8
#ifdef WIN32
		if( XBREAKF( sizeBytes >= lenBuffer, "sizebytes(%d) >= lenBuffer(%d)", sizeBytes, lenBuffer ) )
			return FALSE;
		char utf8Buff[ 0x10000 ];
		memcpy_s( (void*)utf8Buff, 0x10000
						, (void*)( GetBufferMem() + m_idxCurr ), sizeBytes );
		utf8Buff[ sizeBytes ] = 0;
		// UTF8을 UTF16으로 변환시켜야 함
		ConvertUTF8ToUTF16( szBuffer, lenBuffer, utf8Buff );
#else
		//        XBREAKF(1, "can not recv utf-8" );`
		memcpy_s( (void*)szBuffer, lenBuffer, (void*)(GetBufferMem()+m_idxCurr), sizeBytes);
#endif
	} else
	if( encode == 3 ) {	// euckr
		char euckrBuff[ 0x10000 ];
		if( XBREAKF( sizeBytes >= sizeof( euckrBuff ), "size(%d) >= sizeof(euckrBuff)(%d)", sizeBytes, sizeof( euckrBuff ) ) )
			return FALSE;
		memcpy_s( (void*)euckrBuff, 0x10000
						, (void*)( GetBufferMem() + m_idxCurr ), sizeBytes );
		euckrBuff[ sizeBytes ] = 0;
		// EUCKR을 UTF16으로 변환
		Convert_char_To_TCHAR( szBuffer, lenBuffer, euckrBuff );
//		_tcscpy_s( szBuffer, lenBuffer, Convert_char_To_TCHAR( euckrBuff ) );
	}
	else
		XBREAKF( 1, "unknown encoding type:%d", encode );
	m_idxCurr += sizeBytes;
	XBREAK( m_idxCurr >= m_idxEnd );
	return TRUE;
}
/**
 @brief pBuffer의 바이트크기
 2015.6.26 : 일단 코딩은 완성했지만 테스트는 못해봄.
*/
// bool XArchive::ReadString( void *pBuffer, int sizeBuff )
// {
// 	//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
// 	CheckMode();
// 	::memset( pBuffer, 0, sizeBuff );	// 실패하더라도 버퍼는 클리어 되게
//     XBREAK( m_idxCurr & 0x03 );		// 4byte정렬 검사
//     WORD sizeBytes = *((WORD*)(GetBufferMem()+m_idxCurr));		// 정렬된 스트링 메모리 byte 사이즈. 널 포함
//     WORD encode = *((WORD*)(GetBufferMem()+m_idxCurr+2));
//     m_idxCurr += sizeof(int);
// 	XBREAK( m_idxCurr >= m_idxEnd );
//     if( encode == 1) {		// utf16
// #ifdef WIN32
// 		//윈32는 utf16으로 준것을 UTF16으로 그대로 받는다.
// 		XBREAK( sizeof(TCHAR) != 2 );
// 		if( XBREAKF( sizeBytes >= sizeBuff, "sizeBytes(%d) >= sizeBuff(%d)", sizeBytes, sizeBuff ) )
// 			return false;
//         memcpy_s( pBuffer, sizeBuff, (void*)(GetBufferMem()+m_idxCurr), sizeBytes);
// #else
// 		//기기에서는 UTF16으로 받은것을 UTF8로 변환해야 한다.
// 		UNICHAR uszUtf16[0x8000];
// 		if( XBREAK(sizeBytes >= sizeof(uszUtf16)) )
// 			return false;
//         memcpy_s( (void*)uszUtf16, sizeof(uszUtf16), (void*)(GetBufferMem()+m_idxCurr), sizeBytes);
//         char buff[0x8000];
//         const char* cUTF8 = _ConvertUTF16ToUTF8( buff, uszUtf16 );
// 		// utf8로 변환한 길이가 받을 버퍼보다 크면 에러
// 		if( XBREAK(strlen(cUTF8) >= sizeBuff) )
// 			return false;
// 		memcpy_s( pBuffer, sizeBuff, cUTF8, sizeof(buff) )
// //        strcpy_s( pBuffer, sizeBuff, cUTF8 );
// #endif
//     } else
// 	if( encode == 2 ) {	// utf8
// #ifdef WIN32
// 		if( XBREAKF( sizeBytes >= sizeBuff, "sizebytes(%d) >= sizeBuff(%d)", sizeBytes, sizeBuff ) )
// 			return false;
// 		char utf8Buff[ 0x10000 ];
// 		memcpy_s( (void*)utf8Buff, 0x10000, (void*)(GetBufferMem()+m_idxCurr), sizeBytes );
// 		// UTF8을 UTF16으로 변환시켜야 함
// 		ConvertUTF8ToUTF16( (TCHAR*)pBuffer, sizeBuff / sizeof(WCHAR), utf8Buff );
// #else
// //        XBREAKF(1, "can not recv utf-8" );`
// 		memcpy_s( (void*)pBuffer, sizeBuff, (void*)(GetBufferMem()+m_idxCurr), sizeBytes);
// #endif
// 	} else
//     if( encode == 3 ) {	// euckr
// 		if( XBREAKF( sizeBytes >= sizeBuff, "size(%d) >= sizeBuff(%d)", sizeBytes, sizeBuff ) )
// 			return false;
//         memcpy_s( (void*)pBuffer, sizeBuff, (void*)(GetBufferMem()+m_idxCurr), sizeBytes );
//     } else
//         XBREAKF(1, "unknown encoding type:%d", encode );
//     m_idxCurr += sizeBytes;
// 	XBREAK( m_idxCurr >= m_idxEnd );
// 	return true;
// }
int XArchive::ReadBufferSize( void )
{
	CheckMode();
	XBREAK( m_idxCurr & 0x03 );		// 4byte정렬 검사
	int size = *((int*)(GetBufferMem()+m_idxCurr));		// 버퍼 크기 구함
	return size;
}
BOOL XArchive::ReadBuffer( void *pDst, int dstSize, int *pOutSizeBuffer )
{
	//XBREAK( m_WriteMode == 1 );		// 쓰기전용인데 읽기를 시도한경우
	CheckMode();
	XBREAK( m_idxCurr & 0x03 );		// 4byte정렬 검사
	int size = *((int*)(GetBufferMem()+m_idxCurr));		// 버퍼 크기 구함(align 안된크기)
	m_idxCurr += sizeof(int);
	XBREAK( m_idxCurr >= m_idxEnd );
	if( XBREAKF( size > dstSize, "size(%d) > dstSize(%d)", size, dstSize ) )
		return FALSE;
	if( size > 0 )
		memcpy_s( pDst, dstSize, (BYTE*)GetBufferMem()+m_idxCurr, size );
	if( pOutSizeBuffer )
		*pOutSizeBuffer = size;
	if( size & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
		size = (size & ~(0x03)) + 4;		// 비트 절삭하고 4바이트 더 더해줌
	m_idxCurr += size;
	XBREAK( m_idxCurr >= m_idxEnd );
	return TRUE;
}

void XArchive::WriteBuffer( void *pSrc, int _size ) 
{
	//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
	if( _size > 0 ) {
		// 4byte 정렬
		int sizeAligned = _size;
		if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
			sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
		XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사
		CheckReallocArchive( sizeAligned );
		const int leftsize = GetLeftSize();
		XBREAK( leftsize <= sizeAligned + 4 );
		*( (int*)( GetBufferMem() + m_idxCurr ) ) = _size;			// 이거 아닌가?
		m_idxCurr += sizeof( int );
		XBREAK( m_idxCurr + sizeAligned >= m_idxEnd );
		BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
		memcpy_s( pBuff + m_idxCurr, leftsize, pSrc, _size );	// sizeAligned로 카피하면 안됨. 메모리오버해서 읽음.
		m_idxCurr += sizeAligned;
		XBREAK( m_idxCurr >= m_idxEnd );
		AddPacketLength( sizeAligned + 4 );
	} else {
		*( (int*)( GetBufferMem() + m_idxCurr ) ) = _size;			// 이거 아닌가?
		AddPacketLength( sizeof(int) );		// size
	}
}
// 버퍼사이즈를 앞에 저장하지 않는 버전
void XArchive::WriteBufferNoHeader( void *pSrc, int _size ) 
{
	//XBREAK( m_WriteMode == 2 );		// 읽기전용인데 쓰기를 시도한경우
	if( _size == 0 )
		return;
	// 4byte 정렬
	int sizeAligned = _size;
	if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
		sizeAligned = ( sizeAligned & ~( 0x03 ) ) + 4;		// 비트 절삭하고 4바이트 더 더해줌
	XBREAK( m_idxCurr & 0x03 ); // 4바이트 정렬		// 4byte정렬 검사
	CheckReallocArchive( sizeAligned );
	XBREAK( m_idxCurr + sizeAligned >= m_idxEnd );
	const int leftsize = GetLeftSize();
	BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
	memcpy_s( pBuff + m_idxCurr, leftsize, pSrc, _size );	// sizeAligned로 카피하면 안됨. 메모리오버해서 읽음.
	m_idxCurr += sizeAligned;
	XBREAK( m_idxCurr >= m_idxEnd );
	AddPacketLength( sizeAligned );
}

void XArchive::IsCryptThenSetFlag()
{
	DWORD dwIdentifier = *((DWORD*)(GetBufferMem()+4));
	if( dwIdentifier == XCrypto::CRYPTO_OBJ_IDENTIFIER )
		m_bEncryption = TRUE;
	m_bEncryption = FALSE;
}
// 아카이브 메모리를 pCryter로 암호화 한다.
BOOL XArchive::DoCrypto( XCrypto *pCrypter, DWORD dwParam )
{
	XBREAK( GetMaxSizeArchive() > 0x10000 );	// 이경우는 스택을 지나치게 많이 먹기때문에 문제없는지 고려해봐야함
	const int sizeBuff = 0x10000;
	BYTE bufCrypto[ sizeBuff ];	// 아카이브 동적할당가능으로 바뀌면서 최대치로 잡음.
	const int sizeArchive = size();	// 아카이브 길이헤더를 포함한 버퍼 전체 사이즈
	BYTE* pBuffPacket = GetBufferPacket();
	const int sizePacket = GetPacketLength();
	const int sizeEncrypt = pCrypter->EncryptMem( (BYTE*)bufCrypto, sizeBuff,
																								pBuffPacket, sizePacket, dwParam );
	if( sizeEncrypt == 0 )
		return FALSE;
	CurrMoveToStart();
	CheckReallocArchive( sizeEncrypt );
#ifdef _XDEBUG
	BYTE* pBuff = const_cast<BYTE*>( GetBufferMem() );
	memset( pBuff, 0, GetMaxSizeArchive() );
#endif
	// 암호화된 패킷을 다시 아카이브에 넣는다.
	SetPacketLength( sizeEncrypt );
	{
		BYTE* pBuffPacket = GetBufferPacket();
		const int sizeArchive = GetMaxSizeArchive();
		memcpy_s( pBuffPacket, sizeArchive
						, bufCrypto, sizeEncrypt );
	}
	m_bEncryption = TRUE;
	return TRUE;
}

BOOL XArchive::DoDecrypto( XCrypto *pCrypter )
{
	XBREAK( GetMaxSizeArchive() > 0xffff );	// 이경우는 스택을 지나치게 많이 먹기때문에 문제없는지 고려해봐야함
//	if( m_bEncryption )
		// 암호화된 아카이브인데 첫버퍼 식별자가 없으면 안됨.
		if( *((DWORD*)GetBufferPacket()) != XCrypto::CRYPTO_OBJ_IDENTIFIER )
			return FALSE;
	const int sizeBuff = 0x10000;
	BYTE _bufCrypto[ sizeBuff ];
	BYTE *bufCrypto = (BYTE*)_bufCrypto;
	BYTE *pCurr = bufCrypto;
	const int size = GetPacketLength();
	XBREAK( size <= 0 );
	// m_Buffer의 암호화된 내용을 임시 버퍼로 옮김
	XBREAK( size >= sizeBuff );
	memcpy_s( bufCrypto, sizeBuff, GetBufferPacket(), size );
	CheckReallocArchive( size );
	auto pBuffDst = GetBufferPacket();
	auto sizeBuffDst = GetMaxBufferSize();
	const int sizeDat = pCrypter->DecryptMem( pBuffDst, sizeBuff
																					, bufCrypto, size );
	XBREAK( sizeDat <= 0 );
	SetPacketLength( sizeDat );
	m_bEncryption = FALSE;
	return TRUE;
}

/**
 @brief this아카이브를 압축포맷으로 만든다.
 암호화 되어있지 않은 아카이브여야 한다.
*/
int XArchive::DoCompress()
{
	BYTE* pBuffComp = nullptr;
	DWORD sizeBuffComp = 0;
	BYTE* pBuffPacket = GetBufferPacket();
	const auto sizePacket = GetPacketLength();
	XSYSTEM::CreateCompressMem( &pBuffComp			// 새로 할당받을 압축된 데이터
														, &sizeBuffComp		// 압축된 데이터의 크기
														, pBuffPacket				// 압축할 데이터의 시작
														, sizePacket );			// 압축할 데이터의 크기
	if( XASSERT(pBuffComp) ) {
#ifdef _DEBUG
		CONSOLE("DoCompress: sizePacket:%d => sizeComp=%d", sizePacket, sizeBuffComp );
#endif // _DEBUG
		// 압축된 데이터를 this아카이브에 옮김.
		CurrMoveToStart();
		int sizeBuffCompAligned = sizeBuffComp;
		if( sizeBuffCompAligned & 0x03 )
			sizeBuffCompAligned= (sizeBuffCompAligned & (~0x03)) + 4;		// 비트절삭하고 4바이트에 맞춤.
		SetPacketLength( sizeBuffCompAligned + 12 ); // 압축크기(정렬된) + 헤더들
		const int sizeBuff = GetLeftSize();
		*( ( DWORD* )pBuffPacket ) = (DWORD)COMPRESS_OBJ_IDENTIFIER;
		pBuffPacket += 4;
		*( ( DWORD* )pBuffPacket ) = sizePacket;		// 압축전 크기
		pBuffPacket += 4;
		*( ( DWORD* )pBuffPacket ) = sizeBuffComp;	// 압축후 크기
		pBuffPacket += 4;
		XBREAK( (int)sizeBuffComp >= sizeBuff - 12 );		// 안전을 위해서 >=를 써서 4바이트여유를 둠.
		memcpy_s( pBuffPacket, sizeBuff - 12, pBuffComp, sizeBuffComp );
	}
	return sizeBuffComp;
}

/**
 @brief 
 4byte: 헤더 + 압축된 데이타의 크기
 4byte: 식별자
 4byte: 압축되기전 크기
 4byte: 압축된 크기
 ~ : 압축된 데이터 덩어리
*/
bool XArchive::DoUnCompress()
{
	// 압축된 this데이터를 카피뜬다.
	BYTE* pBuffThis = GetBufferPacket();		// 길이 헤더를 뺀 시작버퍼
	const DWORD identifier = *( (DWORD*)pBuffThis );
	// 압축되지 않은 아카이브를 이걸로 호출해선 안됨.
	if( XBREAK(identifier != COMPRESS_OBJ_IDENTIFIER) )
		return false;
	pBuffThis += 4;
	int sizeUnCompressed = *((int*)pBuffThis);
	pBuffThis += 4;
	int sizeComp = *((int*)pBuffThis);
	pBuffThis += 4;
	//
	BYTE* pBuffComp = pBuffThis;
	BYTE* pBuffUncompressed = nullptr;	// 새로 할당받을 메모리 포인터
	XSYSTEM::UnCompressMem( &pBuffUncompressed
												, sizeUnCompressed		// 할당받을 메모리크기
												, pBuffComp					// 압축데이터의 시작
												, sizeComp );				// 압축데이터의 크기
	// 압축푼게 현재 버퍼보다 크다면 버퍼크기 재조정.
	CheckReallocArchive( sizeUnCompressed + 4 );
	// 압축푼 데이터를 다시 this로 카피
	CurrMoveToStart();
	SetPacketLength( sizeUnCompressed );
	BYTE* pBuffDst = GetBufferPacket();
	const int sizeLeft = GetLeftSize();
	memcpy_s( pBuffDst, sizeLeft, pBuffUncompressed, sizeUnCompressed );
	SAFE_DELETE_ARRAY( pBuffUncompressed );
	return true;
}




//////////////////////////////////////////////////////////////////////////
XPacketMem::XPacketMem( XArchive& arSrc ) 
{ 
	Init(); 
	// 소스 아카이브의 크기를 구해 메모리를 할당해서
	int size = arSrc.size();
	m_Buffer = new BYTE[ size ];
	const BYTE *pSrc = arSrc.GetBuffer();
	// 소스에서 this로 메모리 내용을 카피해온다.
	memcpy_s( (void*)m_Buffer, size, (void*)pSrc, size );
	m_Size = size;
}

