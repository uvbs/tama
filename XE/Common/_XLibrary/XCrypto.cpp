#include "stdafx.h"
#include "XCrypto.h"
#ifdef _SERVER
#include "XFramework/server/XServerMain.h"
#endif // _SERVER

#ifdef _XSUPPORT_ARCHIVE
#include "XArchive.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//BYTE XCrypto::s_keyGlobal = 0;

// 객체를 암호화 시키기를 원한다. key값은 byte짜리 원하는 암호키
void XCrypto::SetEncrypt( BOOL bEncryption ) 
{
#ifdef _SERVER
	const ID idThread = ::GetCurrentThreadId();
	const ID idThreadMain = XEServerMain::sGet()->GetidThreadMain();
	// 이곳은 메인스레드에서만 불려야 함.
	XASSERT( idThread == idThreadMain );
#endif // _XSERVER
	m_bEncryption = bEncryption;
//	m_Key = key;
	if( bEncryption ) {
		// 랜덤키 테이블 생성.
		MakeRandomKeyTable();
		// 초기화
//		GetNextRandomKeyClear();
	} else
		XCLEAR_ARRAY( m_tableRandomKey );
}

// 암호화에 필요한 정보(식별자, 랜덤키테이블, 키 테이블 등)를 저장한다.
void XCrypto::WriteHeader( FILE *fp )
{
	XBREAK( m_keyPrivate == 0 );
	DWORD dwIdentifier = CRYPTO_OBJ_IDENTIFIER;												// 암호화된 파일이라는 표시
	fwrite( &dwIdentifier, 4, 1, fp );
	DWORD ver = 1;
	fwrite( &ver, 4, 1, fp );
	fwrite( m_tableRandomKey, xKEY_TABLE_SIZE, 1, fp );		// 키 테이블 저장
}

// 암호화 파일의 끝부분에 기록할 데이타.(체크섬)
void XCrypto::WriteTailer( FILE *fp )
{
}

// 암호화 파일의 헤더를 읽는다.
void XCrypto::ReadHeader( FILE *fp )
{
	XBREAK( m_keyPrivate == 0 );
	DWORD dwIdentifier;
	fread( &dwIdentifier, 4, 1, fp );		// identifier
	DWORD ver;
	fread( &ver, 4, 1, fp );
	fread( m_tableRandomKey, xKEY_TABLE_SIZE, 1, fp );		// 랜덤키 테이블 읽음.
	// 초기화
//	GetNextRandomKeyClear();
	m_bEncryption = TRUE;
}

// fp파일이 암호화된 파일인가.
BOOL XCrypto::IsEncryption( FILE *fp )
{
	DWORD dwIdentifier;
	fread( &dwIdentifier, 4, 1, fp );		// 앞에 4바이트 함 읽어보고
	fseek( fp, 0, SEEK_SET );
	return (dwIdentifier == 7041214);	// 식별자가 맍으면 암호화 파일임.
}

// pSrc(sizeSrc)를 암호화해서 pOutBuffer에 담는다. sizeBuffer는 pOutBuffer의 크기
// 성공하면 0이 아닌 체크섬값을 리턴한다. 
long long XCrypto::Encrypt( BYTE *pOutBuffer, int sizeBuffer, BYTE *_pSrc, int sizeSrc ) 
{
	XBREAK( pOutBuffer == NULL );
	XBREAK( _pSrc == NULL );
	XBREAK( sizeSrc > sizeBuffer );
	XBREAK( sizeSrc <= 0 );
	XBREAK( sizeBuffer <= 0 );
	XBREAK( m_keyPrivate == 0 );
	BYTE* pSrc = _pSrc;
	BYTE key = m_keyPrivate;
	long long llCheckSum = 0;
	int idx = 0;
// 	암호화 코드 시작부분과 끝부분 식별자 넣어서 디버깅
// 	Encrypt한다음 서버에서 Decrypt한번 돌려서 체크섬값이 같은지 비교
// 	GetNextRandomKey 에서 내부변수 쓰지 않도록.
// 	인크립 디크립할때 락이 제대로 걸리는지 확인
	for( int i = 0; i < sizeSrc; ++i )
	{
		BYTE byteDat = *pSrc++;
		BYTE keyRandom = GetNextRandomKey( &idx );
		BYTE dat = byteDat ^ keyRandom ^ key;
		llCheckSum += (long long)byteDat;	// 압축되기전 값을 체크섬시켜야 함.
		*pOutBuffer++ = dat;
	}
	++llCheckSum;	// 0값이 안나오도록
	return llCheckSum;
}

/**
 pSrc메모리를 암호화 해서 pOutBuffer에 담아준다.
 암호화 메모리 구조
 4byte 식별자
 8byte 체크섬값
 4byte 암호화된 메모리 크기
 나머지 암호화된 데이타
*/
// 
int XCrypto::EncryptMem( BYTE *pOutBuffer, int sizeBuffer, BYTE *pSrc, int sizeSrc, DWORD dwParam ) 
{
#ifdef _SERVER
	const ID idThread = ::GetCurrentThreadId();
	const ID idThreadMain = XEServerMain::sGet()->GetidThreadMain();
	// 이곳은 메인스레드에서만 불려야 함.
	XASSERT( idThread == idThreadMain );
#endif // _XSERVER
	XBREAKF( (DWORD)pOutBuffer & 3, "pOutBuffer=%08x", (DWORD)pOutBuffer );
// 	const int sizeHeader = sizeof(DWORD) + sizeof(long long) + sizeof(DWORD);
// 	XBREAK( sizeBuffer < sizeSrc + sizeHeader );	// 식별자와 체크섬값 길이 포함
	BYTE *pCurr = pOutBuffer;
	// 암호화된 메모리 임을 알리는 식별자
	DWORD dwIdentifier = XCrypto::CRYPTO_OBJ_IDENTIFIER;
	*((DWORD*)pCurr) = dwIdentifier;
	pCurr += sizeof(DWORD);
	sizeBuffer -= sizeof(DWORD);	// 버퍼 4바이트 사용했으므로 빼줌
	//
	*((long long*)pCurr) = 0;	// 체크섬값 위치 비워둠
	pCurr += sizeof(long long);
	sizeBuffer -= sizeof(long long);	// 버퍼 4바이트 사용했으므로 빼줌
	//
	*((DWORD*)pCurr) = sizeSrc;		// 실 데이타 크기
	pCurr += sizeof(DWORD);
	sizeBuffer -= sizeof(DWORD);	// 버퍼 4바이트 사용했으므로 빼줌
	//
	*( (DWORD*)pCurr ) = m_keyPrivate;		// 확인용
	pCurr += sizeof( DWORD );
	sizeBuffer -= sizeof( DWORD );	// 버퍼 4바이트 사용했으므로 빼줌
	*( (DWORD*)pCurr ) = dwParam;		// 확인용(보통 패킷아이디)
	pCurr += sizeof( DWORD );
	sizeBuffer -= sizeof( DWORD );	// 버퍼 4바이트 사용했으므로 빼줌
	*( (DWORD*)pCurr ) = 0;		// 확인용(체크섬 원본)
	pCurr += sizeof( DWORD );
	sizeBuffer -= sizeof( DWORD );	// 버퍼 4바이트 사용했으므로 빼줌

	int sizeHeader = (int)((DWORD)pCurr - (DWORD)pOutBuffer);
	XBREAK( sizeBuffer < sizeSrc + sizeHeader );	// 식별자와 체크섬값 길이 포함
	//
//	GetNextRandomKeyClear();
	*pCurr++ = 0x12;		sizeBuffer -= sizeof(BYTE);
	// Encrypt buff
	long long llCheckSum = Encrypt( pCurr, sizeBuffer, pSrc, sizeSrc );
	XBREAK( llCheckSum > 0xffffffff );	// 이런경우 헤더에 넣는 체크섬값 크기 늘여야함.
	// 잘 풀리는지 검증
	BYTE buffDecrypt[ 0x10000 ];
//	GetNextRandomKeyClear();
	XINT64 llCheckSumDecrypt = Decrypt2( buffDecrypt, 0x10000, pCurr, sizeSrc );
 	XBREAK( llCheckSumDecrypt != llCheckSum );
	if( llCheckSumDecrypt != llCheckSum ) {
		for( int i = 0; i < sizeSrc; ++i ) {
			const BYTE datEnc = pSrc[i];
			const BYTE datDec = buffDecrypt[i];
			if( datEnc != datDec ) {
				int a = 0;
			}
		}
	}

// 	break걸렸을때 다시 실행해보면 같은값이 제대로 나오는걸로 보아. 멀티스레드 문제인듯 싶다.
// 
// 		192번 리부팅








	pCurr += sizeSrc;
	*pCurr++ = 0x34;
//	pCurr += sizeSrc;
	// 체크섬값 암호화
//	GetNextRandomKeyClear();
	long long llCryptCheckSumEncrypt = 0;
	Encrypt( (BYTE*)(&llCryptCheckSumEncrypt), sizeof(long long), (BYTE*)(&llCheckSum), sizeof(long long) );
	// 암호화된 체크섬값 넣을 포인터
	pCurr = pOutBuffer + sizeof(DWORD);
	XBREAKF( (DWORD)pCurr & 3, "%08x", (DWORD)pCurr );
	//
	*((long long*)pCurr) = llCryptCheckSumEncrypt;	// 헤더에 암호화된 체크섬값 써넣음
	pCurr += sizeof( long long );
	pCurr += sizeof( DWORD );		// sizeSrc
	pCurr += sizeof( DWORD );		// keyPrivate
	pCurr += sizeof( DWORD );		// dwParam
	*((DWORD*)pCurr) = (DWORD)llCheckSum;

	// +2: 0x12, 0x34
	return sizeSrc + 2 + sizeHeader;
}

// size길이의 buffer의 내용을 해독하여 다시 buffer에 넣는다.,
int XCrypto::Decrypt( BYTE *buffer, int size )
{
	XBREAK( buffer == NULL );
	XBREAK( size <= 0 );
	BYTE key = m_keyPrivate;
	int idx = 0;
	for( int i = 0; i < size; ++i ) {
		BYTE dat = *buffer;			// 한바이트 읽음
		*buffer++ = dat ^ key ^ GetNextRandomKey( &idx );		// 해독
	}
	return 1;
}

long long XCrypto::Decrypt2( BYTE *pDst, int sizeDst
													, BYTE *pSrc, int sizeSrc )
{
	XBREAK( pDst == NULL );
	XBREAK( pSrc == NULL );
	XBREAK( sizeDst <= 0 );
	XBREAK( sizeSrc <= 0 );
	XBREAK( sizeSrc > 0xffff );	// 아직 64k이상은 지원하지 않음.
	XBREAK( sizeSrc > sizeDst );
	BYTE key = m_keyPrivate;
	long long llCheckSum = 0;
	int idx = 0;
	//
	for( int i = 0; i < sizeSrc; ++i ) {
		BYTE dat = *pSrc++;			// 한바이트 읽음
		BYTE decrypDat = dat ^ key ^ GetNextRandomKey( &idx );		// 해독
		*pDst++ = decrypDat;
		llCheckSum += decrypDat;
	}
	++llCheckSum;	// 0 나오지 않게
	return llCheckSum;
}

int XCrypto::DecryptMem( BYTE *pDst, int sizeDst, BYTE *pSrc, int sizeSrc )
{
#ifdef _SERVER
	const ID idThread = ::GetCurrentThreadId();
	const ID idThreadMain = XEServerMain::sGet()->GetidThreadMain();
	// 이곳은 메인스레드에서만 불려야 함.
	XASSERT( idThread == idThreadMain );
#endif // _XSERVER
	BYTE *pCurr = pSrc;
	DWORD dwIdentifier = *((DWORD*)pCurr);			pCurr += sizeof(DWORD);
	long long llCheckSumEncrypt = *((long long*)pCurr);	pCurr += sizeof(long long);
	DWORD sizeData = *((DWORD*)pCurr);			pCurr += sizeof(DWORD);	// 암호화된 실 데이타 크기
	DWORD keyPrivate = *( (DWORD*)pCurr );			pCurr += sizeof( DWORD );	
	DWORD dwdParam = *( (DWORD*)pCurr );			pCurr += sizeof( DWORD ); // idPacket
	DWORD llCheckSumOrig = *( (DWORD*)pCurr );			pCurr += sizeof( DWORD );
//	GetNextRandomKeyClear();
	// 암호화되어 저장되어있던 체크섬값을 해독한다.
	XUINT64 llCheckSumDecrypt = 0;
	Decrypt2( (BYTE*)(&llCheckSumDecrypt), sizeof(long long)
					, (BYTE*)(&llCheckSumEncrypt), sizeof(long long) );
//	GetNextRandomKeyClear();
	// Decrypt buff
	BYTE byteIden = *pCurr++;
	XBREAK( byteIden != 0x12 );
	long long llCheckSumByCalc = Decrypt2( pDst, sizeDst, pCurr, sizeData );
	pCurr += sizeData;
	byteIden = *pCurr++;
	XBREAK( byteIden != 0x34 );
	if( ( llCheckSumByCalc != llCheckSumDecrypt) )
		return 0;
	return sizeData;
}

void XCrypto::WriteCheckSum( DWORD dwCheckSum, FILE *fp )
{
	XBREAK( m_keyPrivate == 0 );
	XBREAK( dwCheckSum == 0 );
	// 체크섬값 암호화
	BYTE *byteBuffer =(BYTE *)&dwCheckSum;
	BYTE encrypt[4];
	for( int i = 0; i < 4; ++i )
	{
		BYTE byteDat = *byteBuffer++;
		encrypt[i] = byteDat ^ m_tableRandomKey[i] ^ m_keyPrivate;		// 랜덤키테이블[0,1,2,3] 으로 암호화 함.
	}
	fwrite( encrypt, 4, 1, fp );
}

// 체크섬을 검사한다. fp는 파일의 시작이어야 한다.
BOOL XCrypto::CheckCheckSum( FILE *fp )
{
	//
	fseek( fp, 0, SEEK_END );		// 파일포인터를 끝으로
	int sizeFile = ftell( fp );
	sizeFile -= xHEADER_SIZE + 4;		// 헤더와 체크섬을 뺀 순수 원본파일사이즈
//	fseek( fp, xHEADER_SIZE, SEEK_SET );		// 파일포인터의 처음에서 헤더는 건너뜀.
	fseek( fp, 0, SEEK_SET );		// 파일포인터의 처음에서 헤더는 건너뜀.
	ReadHeader( fp );
	int num = sizeFile / 4096;
	int remain = sizeFile % 4096;
	BYTE buffer[ 4096 ];
	DWORD dwCheckSum = 0;
//	GetNextRandomKeyClear();
	for( int i = 0; i < num; ++i )
	{
		fread( buffer, 4096, 1, fp );			// 4k단위로 읽음.
		Decrypt( buffer, 4096 );				// 읽은 블럭을 해독함
		for( int k = 0; k < 4096; ++k )
			dwCheckSum += buffer[k];		// 바이트들을 모두 더함.
	}
	if( remain > 0 )
	{
		fread( buffer, remain, 1, fp );
		Decrypt( buffer, remain );				// 읽은 블럭을 해독함
		for( int k = 0; k < remain; ++k )
			dwCheckSum += buffer[k];		// 바이트들을 모두 더함.
	}
	DWORD checkSumFile;
	fread( &checkSumFile, 4, 1, fp );			// 파일에 쓰여있는 체크섬 값을 읽음.
//	GetNextRandomKeyClear();
	Decrypt( (BYTE *)&checkSumFile, 4 );		// 암호화된 체크섬값 해독
	//
	fseek( fp, 0, SEEK_SET );		// 끝나면 다시 파일포인터를 첨으로 돌림.
	//
	if( checkSumFile == dwCheckSum )
		return TRUE;
	return FALSE;
}

#ifdef _XSUPPORT_ARCHIVE
BOOL XCrypto::SerializeKeyTable( XArchive& ar ) const
{
#ifdef _SERVER
	const ID idThread = ::GetCurrentThreadId();
	const ID idThreadMain = XEServerMain::sGet()->GetidThreadMain();
	// 이곳은 메인스레드에서만 불려야 함.
	XASSERT( idThread == idThreadMain );
#endif // _XSERVER
	// 멀티스레드땜에 복사해서 사용.
	BYTE table[ xKEY_TABLE_SIZE ];
	memcpy_s( table, sizeof(table), m_tableRandomKey, sizeof(m_tableRandomKey) );
	std::swap( table[0], table[1] );		// 해킹 속이기 위해 값을 바꿔넣는다.
	ar.WriteBuffer( table, sizeof(BYTE) * xKEY_TABLE_SIZE );
// 	std::swap( table[0], m_tableRandomKey[1] );		// 다시 원래대로 돌려놓는다.
	return TRUE;
}

BOOL XCrypto::DeSerializeKeyTable( XArchive& ar )
{
#ifdef _SERVER
	const ID idThread = ::GetCurrentThreadId();
	const ID idThreadMain = XEServerMain::sGet()->GetidThreadMain();
	// 이곳은 메인스레드에서만 불려야 함.
	XASSERT( idThread == idThreadMain );
#endif // _XSERVER
	int sizeBuffer = 0;
	ar.ReadBuffer( m_tableRandomKey, &sizeBuffer );
	std::swap( m_tableRandomKey[0], m_tableRandomKey[1] );	// 해커를 위한 페이크
	XASSERT( sizeBuffer == xKEY_TABLE_SIZE );
	m_bEncryption = TRUE;
//	GetNextRandomKeyClear();
	m_bLoadedRandomKey = true;
	return TRUE;
}

/**
 @brief strc스트링을 암호화해서 ar에 넣는다.
 @param sizeDummy 해킹을 어렵게 하기 위해 더미데이터를 앞에 몇 바이트나 넣을건지.
*/
void XCrypto::SerializeEncryptWithString( const std::string& strc, XArchive& ar, int sizeDummy )
{
#ifdef _SERVER
	const ID idThread = ::GetCurrentThreadId();
	const ID idThreadMain = XEServerMain::sGet()->GetidThreadMain();
	// 이곳은 메인스레드에서만 불려야 함.
	XASSERT( idThread == idThreadMain );
#endif // _XSERVER
	if( strc.empty() ) {
		ar << 0;
		return;
	}
	const int sizeBuff = 4096;
	BYTE cStr[ sizeBuff ];
	BYTE cKeyBuff[ sizeBuff ];
	XBREAK( sizeDummy > sizeBuff );
 	for( int i = 0; i < sizeDummy; ++i )
 		cKeyBuff[i] = (BYTE)xRandom(256);
	ar.WriteBuffer( cKeyBuff, sizeDummy );		// fake용 dummy
	strcpy_s( (char*)cStr, sizeBuff, strc.c_str() );
	const int sizeStr = strc.length();
	const auto sizeEncrypt = EncryptMem( cKeyBuff, sizeBuff, cStr, sizeStr, 0 );
	ar.WriteBuffer( cKeyBuff, sizeEncrypt );
}

#ifdef WIN32
void XCrypto::SerializeEncryptWithString( const _tstring& strt, XArchive& ar, int sizeDummy )
{
	const std::string strc = SZ2C(strt);
	SerializeEncryptWithString( strc, ar, sizeDummy );
}
#endif // WIN32

/**
 @brief 암호화된 문자열을 풀어서 리턴한다.
*/
std::string XCrypto::DeSerializeDecryptString( XArchive& ar )
{
#ifdef _SERVER
	const ID idThread = ::GetCurrentThreadId();
	const ID idThreadMain = XEServerMain::sGet()->GetidThreadMain();
	// 이곳은 메인스레드에서만 불려야 함.
	XASSERT( idThread == idThreadMain );
#endif // _XSERVER
	const int sizeBuff = 4096;
 	BYTE cKeyBuff[ sizeBuff ];
	BYTE cKeyEncrypt[ sizeBuff ];
 	XBREAK( IsbLoadedRandomKey() == false );
	int sizeEncrypt = 0;
	ar.ReadBuffer( cKeyBuff, &sizeEncrypt );		// dummy
	if( sizeEncrypt == 0 )
		return std::string();
	sizeEncrypt = 0;
	ar.ReadBuffer( cKeyEncrypt, &sizeEncrypt );
	XBREAK( sizeEncrypt > 4096 );		// 비정상적으로 큼.
	XBREAK( sizeEncrypt <= 0 );
	const auto sizeDecrypt 
		= DecryptMem( cKeyBuff, sizeBuff, cKeyEncrypt, sizeEncrypt );
	char cKey[sizeBuff];
	if( sizeDecrypt > 0 ) {	// 빈 스트링을 보내면 0일수 있음.
		XBREAK( sizeDecrypt <= 0 );
		memcpy_s( cKey, sizeBuff, cKeyBuff, sizeDecrypt );
		cKey[sizeDecrypt] = 0;
	} else {
		XBREAK( sizeDecrypt < 0 );
		cKey[0] = 0;
	}
	return std::string(cKey);
}

#endif // _XSUPPORT_ARCHIVE