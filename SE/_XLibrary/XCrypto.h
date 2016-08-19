#pragma once
#include <stdio.h>
#include <stdlib.h>

class XCrypto
{
public:
	enum { xKEY_TABLE_SIZE = 28 };
	enum { xHEADER_SIZE = 4 + 4 + xKEY_TABLE_SIZE };
	static BYTE s_keyGlobal;
private:
	BOOL m_bEncryption;			// 암호화 된 혹은 암호화 시켜야할 객체인가.
//	BYTE m_Key;
	int m_idxTable;
	BYTE m_tableRandomKey[ xKEY_TABLE_SIZE ];		// 랜덤키 테이블
	void Init() {
		m_bEncryption = FALSE;
//		m_Key = 0;
		m_idxTable = 0;
		XCLEAR_ARRAY( m_tableRandomKey );
	}
	void Destroy() {}
public:
	XCrypto() { Init(); }
	virtual ~XCrypto() { Destroy(); }
	//
	GET_ACCESSOR( BOOL, bEncryption );
	//
	void SetEncrypt( BOOL bEncryption, BYTE key );
	void WriteHeader( FILE *fp );
	void WriteTailer( FILE *fp );
	void ReadHeader( FILE *fp );
	BOOL IsEncryption( FILE *fp );
	// 랜덤 키 테이블을 생성한다.
	void MakeRandomKeyTable() {
		for( int i = 0; i < xKEY_TABLE_SIZE; ++i )
			m_tableRandomKey[ i ] = rand() % 256;			// 0에서 255까지의 난수를 생성
	}
	void GetNextRandomKeyClear() { m_idxTable = 0; }
	// 랜덤키를 순환하면서 하나씩 꺼내옴.
	BYTE GetNextRandomKey() {
		if( m_idxTable >= sizeof( m_tableRandomKey ) )
			m_idxTable = 0;
		return m_tableRandomKey[ m_idxTable++ ];
	}
	int Encrypt( BYTE *pOutBuffer, int sizeBuffer, BYTE *pSrc, int sizeSrc );
	int Decrypt( BYTE *buffer, int size );
	BOOL CheckCheckSum( FILE *fp );
	void WriteCheckSum( DWORD dwCheckSum, FILE *fp );
};

