#include "stdafx.h"
#include "XCrypto.h"

BYTE XCrypto::s_keyGlobal = 0;

// 객체를 암호화 시키기를 원한다. key값은 byte짜리 원하는 암호키
void XCrypto::SetEncrypt( BOOL bEncryption, BYTE key ) 
{
	m_bEncryption = bEncryption;
//	m_Key = key;
	// 랜덤키 테이블 생성.
	MakeRandomKeyTable();
	// 초기화
	GetNextRandomKeyClear();
}

// 암호화에 필요한 정보(식별자, 랜덤키테이블, 키 테이블 등)를 저장한다.
void XCrypto::WriteHeader( FILE *fp )
{
	XBREAK( s_keyGlobal == 0 );
	DWORD dwIdentifier = 7041214;												// 암호화된 파일이라는 표시
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
	XBREAK( s_keyGlobal == 0 );
	DWORD dwIdentifier;
	fread( &dwIdentifier, 4, 1, fp );		// identifier
	DWORD ver;
	fread( &ver, 4, 1, fp );
	fread( m_tableRandomKey, xKEY_TABLE_SIZE, 1, fp );		// 랜덤키 테이블 읽음.
	// 초기화
	GetNextRandomKeyClear();
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
int XCrypto::Encrypt( BYTE *pOutBuffer, int sizeBuffer, BYTE *pSrc, int sizeSrc ) 
{
	XBREAK( pOutBuffer == nullptr );
	XBREAK( pSrc == nullptr );
	XBREAK( sizeSrc > sizeBuffer );
	XBREAK( sizeSrc <= 0 );
	XBREAK( sizeBuffer <= 0 );
	BYTE key = s_keyGlobal;
	for( int i = 0; i < sizeSrc; ++i )
	{
		BYTE byteDat = *pSrc++;
		BYTE keyRandom = GetNextRandomKey();
		*pOutBuffer++ = byteDat ^ keyRandom ^ key;
	}
	return 1;
}

// size길이의 buffer의 내용을 해독하여 다시 buffer에 넣는다.,
int XCrypto::Decrypt( BYTE *buffer, int size )
{
	XBREAK( buffer == nullptr );
	XBREAK( size <= 0 );
	BYTE key = s_keyGlobal;
	for( int i = 0; i < size; ++i )
	{
		BYTE dat = *buffer;			// 한바이트 읽음
		*buffer++ = dat ^ key ^ GetNextRandomKey();		// 해독
	}
	return 1;
}

void XCrypto::WriteCheckSum( DWORD dwCheckSum, FILE *fp )
{
	XBREAK( s_keyGlobal == 0 );
	XBREAK( dwCheckSum == 0 );
	// 체크섬값 암호화
	BYTE *byteBuffer =(BYTE *)&dwCheckSum;
	BYTE encrypt[4];
	for( int i = 0; i < 4; ++i )
	{
		BYTE byteDat = *byteBuffer++;
		encrypt[i] = byteDat ^ m_tableRandomKey[i] ^ s_keyGlobal;		// 랜덤키테이블[0,1,2,3] 으로 암호화 함.
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
	GetNextRandomKeyClear();
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
	GetNextRandomKeyClear();
	Decrypt( (BYTE *)&checkSumFile, 4 );		// 암호화된 체크섬값 해독
	//
	fseek( fp, 0, SEEK_SET );		// 끝나면 다시 파일포인터를 첨으로 돌림.
	//
	if( checkSumFile == dwCheckSum )
		return TRUE;
	return FALSE;
}
