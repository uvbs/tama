#pragma once
#include <stdio.h>
#include <stdlib.h>

#define _XSUPPORT_ARCHIVE		// XArchive의 의존성을 떼내고 싶을때 사용

/*
	스레드 세이프하게 만들것.
	만들어서 Send()함수도 멀티스레드로 사용할수 있게 할것.
*/
class XCrypto
{
public:
	enum { xKEY_TABLE_SIZE = 28 };
	enum { xHEADER_SIZE = 4 + 4 + xKEY_TABLE_SIZE };
	enum { CRYPTO_OBJ_IDENTIFIER = 0xf2341234 };	// 암호화 객체 식별자		
	// 압축 아카이브 식별자
	/*static void sSetGlobalKey( BYTE key ) {
		s_keyGlobal = key;
	}
	static BYTE sGetGlobalKey( void ) {
		return s_keyGlobal;
	}*/
private:
	BYTE m_keyPrivate;
	BOOL m_bEncryption;			// 암호화 된 혹은 암호화 시켜야할 객체인가.
//	BYTE m_Key;
//	int m_idxTable;
	BYTE m_tableRandomKey[ xKEY_TABLE_SIZE ];		// 랜덤키 테이블
	bool m_bLoadedRandomKey = false;
	void Init() {
		m_bEncryption = FALSE;
		m_keyPrivate = 0;
//		m_idxTable = 0;
		XCLEAR_ARRAY( m_tableRandomKey );
	}
	void Destroy() {}
// protected:
// 	SET_ACCESSOR( BYTE, keyPrivate );
public:
	XCrypto() { 
		Init(); 
		if( m_keyPrivate == 0 )
			m_keyPrivate = (BYTE)(1 + xRandom(255));
	}
	virtual ~XCrypto() { Destroy(); }
	//
	GET_ACCESSOR_CONST( BOOL, bEncryption );
	GET_SET_ACCESSOR_CONST( BYTE, keyPrivate );
	GET_BOOL_ACCESSOR( bLoadedRandomKey );
	//
	void SetEncrypt( BOOL bEncryption );
	void WriteHeader( FILE *fp );
	void WriteTailer( FILE *fp );
	void ReadHeader( FILE *fp );
	BOOL IsEncryption( FILE *fp );
	// 랜덤 키 테이블을 생성한다.
	void MakeRandomKeyTable( void ) {
		for( int i = 0; i < xKEY_TABLE_SIZE; ++i )
			m_tableRandomKey[ i ] = xRand() % 256;			// 0에서 255까지의 난수를 생성
		m_bLoadedRandomKey = true;
	}
//	void GetNextRandomKeyClear( void ) { m_idxTable = 0; }
	// 랜덤키를 순환하면서 하나씩 꺼내옴.
	inline BYTE GetNextRandomKey( int* pOutIdx ) {
		if( (*pOutIdx) >= sizeof( m_tableRandomKey ) )
			(*pOutIdx) = 0;
		return m_tableRandomKey[ (*pOutIdx)++ ];
	}
	long long Encrypt( BYTE *pOutBuffer, int sizeBuffer, BYTE *pSrc, int sizeSrc );
	int Decrypt( BYTE *buffer, int size );
	int DecryptMem( BYTE *pDst, int sizeDst, BYTE *pSrc, int sizeSrc );
	int EncryptMem( BYTE *pOutBuffer, int sizeBuffer, BYTE *pSrc, int sizeSrc, DWORD dwParam );
	BOOL CheckCheckSum( FILE *fp );
	void WriteCheckSum( DWORD dwCheckSum, FILE *fp );
	void SerializeEncryptWithString( const std::string& strc, XArchive& ar, int sizeDummy );
#ifdef WIN32
	void SerializeEncryptWithString( const _tstring& strt, XArchive& ar, int sizeDummy );
#endif // WIN32
	std::string DeSerializeDecryptString( XArchive& ar );
#ifdef _XSUPPORT_ARCHIVE
	BOOL SerializeKeyTable( XArchive& ar ) const;
	BOOL DeSerializeKeyTable( XArchive& ar );
#endif
private:
	long long Decrypt2( BYTE *pDst, int sizeDst, BYTE *pSrc, int sizeSrc );
};

