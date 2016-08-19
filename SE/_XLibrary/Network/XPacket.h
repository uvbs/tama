#pragma once

class XPacket
{
private:
	enum { xBUFFER_SIZE=0xffff };

	BYTE m_Buffer[ xBUFFER_SIZE ];		// 한번에 주고받을 수 있는 패킷 최대크기. 이것보다 큰 데이타는 버퍼단위로 쪼개서 보내야 한다. 그래야 전송중 진행율 같은것도 만들수 있고 하기땜시..
	BYTE *m_pCurr;		// 현재 포인터
	BYTE *m_pEnd;		// 버퍼의 마지막부분의 포인터
public:
	XPacket() {
		XCLEAR_ARRAY( m_Buffer );
		m_pCurr = m_Buffer + 4;		// 맨앞 4바이트는 패킷의 총 길이다
		m_pEnd = m_Buffer + xBUFFER_SIZE - 1;
	}
	virtual ~XPacket() {}

	//
	// 헤더까지 포함한 순수 메모리 버퍼
	const BYTE* GetBufferMem( void ) { 
		return m_Buffer;		
	}
	// 패킷길이 헤더4바이트를 제외한 순수 패킷포인터
	BYTE* GetBufferPacket( void ) { 
		return m_Buffer + 4;		
	}

	int GetMaxBufferSize( void ) {	
		return xBUFFER_SIZE - 4;	// 사용가능한 버퍼사이즈는 최대크기에서 맨앞 4바이트를 빼줘야 함
	}
	// 패킷길이를 포함한 버퍼전체 사이즈
	int size( void ) {
		int s = (DWORD)m_pCurr - (DWORD)m_Buffer;
        DWORD n = (DWORD)m_Buffer;
//		XBREAK( s != *((int*)m_Buffer) );
		return s;
	}
	void SetPacketLength( DWORD byteLen ) {
		*((DWORD*)m_Buffer) = byteLen;
		m_pCurr = m_Buffer + 4;		// 포인터를 시작위치로 돌림
	}
	DWORD GetPacketLength( void ) {
		return *((DWORD*)m_Buffer);
	}
	void Clear( void ) {
		SetPacketLength( 0 );
		m_pCurr = m_Buffer + 4;;
	}
	//
	XPacket& operator << ( int d ) {
		return *this << (long)d;
	}
	XPacket& operator << ( unsigned long d ) {
		return *this << (long)d;
	}
	XPacket& operator << ( unsigned char d ) {
		return *this << (char)d;
	}
	XPacket& operator << ( unsigned short d ) {
		return *this << (short)d;
	}

	XPacket& operator << ( long d ) {
		// 버퍼 오버플로우 검사할것
		*((long *)m_pCurr) = d;
		m_pCurr += sizeof(long);
		// 버퍼 맨앞에 패킷의 총길이를 넣는다
		AddPacketLength( sizeof(long) );
		return *this;
	}
	XPacket& operator << ( short d ) {
		*((short *)m_pCurr) = d;
		m_pCurr += sizeof(short);
		AddPacketLength( sizeof(short) );
		return *this;
	}
	XPacket& operator << ( char d ) {
		*m_pCurr = d;
		m_pCurr += sizeof(char);
		AddPacketLength( sizeof(char) );
		return *this;
	}
	XPacket& operator << ( float d ) {
		*((float *)m_pCurr) = d;
		m_pCurr += sizeof(float);
		AddPacketLength( sizeof(float) );
		return *this;
	}
	XPacket& operator << ( long long d ) {
		// 버퍼 오버플로우 검사할것
		*((long long*)m_pCurr) = d;
		m_pCurr += sizeof(long long);
		// 버퍼 맨앞에 패킷의 총길이를 넣는다
		AddPacketLength( sizeof(long long) );
		return *this;
	}
	XPacket& operator << ( LPCTSTR szStr ) {
		WriteString( szStr );
		return *this;
	}
#ifdef WIN32
	XPacket& operator << ( const char *cStr ) {
		WriteString( cStr );
		return *this;
	}
#endif
	void AddPacketLength( int size ) {
		int *pSize = (int*)m_Buffer;
		*pSize = *pSize + size;
	}
	void WriteBuffer( void *pSrc, int _size ) {
		int leftsize = (int)m_pEnd - (int)m_pCurr;
        // 4byte 정렬
		int sizeAligned = _size;
		if( sizeAligned & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
			sizeAligned = (sizeAligned & ~(0x03)) + 4;		// 비트 절삭하고 4바이트 더 더해줌
		XBREAK( (int)m_pCurr & 0x03 );		// 4byte정렬 검사
		*((int*)m_pCurr) = sizeAligned;
		m_pCurr += sizeof(int);
		memcpy_s( m_pCurr, leftsize, pSrc, _size );	// sizeAligned로 카피하면 안됨. 메모리오버해서 읽음.
		m_pCurr += sizeAligned;
		AddPacketLength( sizeAligned + 4 );
	}
	// 서버에서 보낼땐 utf16로. iOS에서 받을땐 utf16를 받아 UTF8로 변환해서 사용
	// 클라에서 보낼땐 UTF8로 보내서 서버에서 utf16로 변환. iOS에서 utf8->utf16변환방법을 아직 모르니 일단 이렇게 해야함.
	void WriteString( LPCTSTR szStr );
#ifdef WIN32
	void WriteString( const char *cStr ) {
		int leftsize = (int)m_pEnd - (int)m_pCurr;
        int len = strlen( cStr ) + 1;
		int size = len * sizeof(char);		// 널 포함
        // 4byte 정렬
		if( size & 0x03 )                // 하위에 찌끄래기 비트가 남아있으면
			size = (size & ~(0x03)) + 4;		// 비트 절삭하고 4바이트 더 더해줌
		XBREAK( (int)m_pCurr & 0x03 );		// 4byte정렬 검사
		XBREAK( size > 0xffff );		// 일단 64k이상의 스트링은 못보내는걸로 하자.
		*((WORD*)m_pCurr) = (WORD)size;		// 스트링의 메모리사이즈(널 포함)
		*((WORD*)(m_pCurr+2)) = (WORD)3;		// 텍스트 인코딩(EUCKR)
		m_pCurr += sizeof(int);		// 스트링패킷의 맨앞 사이즈 부분을 건너뜀
		memcpy_s( (void*)m_pCurr, leftsize, (void*)cStr, len * sizeof(char) );	// 스트링 길이부분만 정확하게 메모리 카피함
		m_pCurr += size;	// 스트링 사이즈(정렬된) 건너뜀
		AddPacketLength( size + 4 );		// 총 패킷길이에 더함.
	}
#endif
	template<size_t _Size>
	void ReadString( TCHAR (&szBuffer)[_Size] ) {
		ReadString( szBuffer, _Size );
	}
	void ReadBuffer( void *pDst, int dstSize, int sizeRead ) {
		XBREAK( (int)m_pCurr & 0x03 );		// 4byte정렬 검사
		int size = *((int*)m_pCurr);		// 버퍼 크기 구함
		m_pCurr += sizeof(int);
		memcpy_s( pDst, dstSize, m_pCurr, sizeRead );
		m_pCurr += size;
	}
	void ReadString( TCHAR *szBuffer, int lenBuffer ) {
		XBREAK( (int)m_pCurr & 0x03 );		// 4byte정렬 검사
		WORD size = *((WORD*)m_pCurr);		// 정렬된 스트링 메모리 사이즈. 널 포함
		WORD encode = *((WORD*)(m_pCurr+2));
		m_pCurr += sizeof(int);
		if( encode == 1)		// utf16
		{
			memcpy_s( (void*)szBuffer, lenBuffer, (void*)m_pCurr, size );
		} else
		if( encode == 2 )	// utf8
		{
#ifdef WIN32
			char utf8Buff[ 0xffff ];
			memcpy_s( (void*)utf8Buff, 0xffff, (void*)m_pCurr, size );
			// UTF8을 UTF16으로 변환시켜야 함
			ConvertUTF8ToUTF16( szBuffer, lenBuffer, utf8Buff );
#else
            XBREAK(1);
#endif
		} else
		if( encode == 3 )	// euckr
		{
			char euckrBuff[ 0xffff ];
			memcpy_s( (void*)euckrBuff, 0xffff, (void*)m_pCurr, size );
			// EUCKR을 UTF16으로 변환
			_tcscpy_s( szBuffer, lenBuffer, Convert_char_To_TCHAR( euckrBuff ) );
		} else
			XBREAK(1);
		m_pCurr += size;
	}
	///////////////////////////////////////////////////////
	XPacket& operator >> ( long long& d ) {
		XBREAK( (int)m_pCurr & 0x03 );		// 4byte정렬 검사. 이건 8바이트 정렬해야 하는거 아닌가?
		d = *((long long*)m_pCurr);
		m_pCurr += sizeof(long long);
		return *this;
	}
	XPacket& operator >> ( float& d ) {
		XBREAK( (int)m_pCurr & 0x03 );		// 4byte정렬 검사
		d = *((float*)m_pCurr);
		m_pCurr += sizeof(float);
		return *this;
	}
	XPacket& operator >> ( long& d ) {
		XBREAK( (int)m_pCurr & 0x03 );		// 4byte정렬 검사
		d = *((long*)m_pCurr);
		m_pCurr += sizeof(long);
		return *this;
	}
	XPacket& operator >> ( short& d ) {
		XBREAK( (int)m_pCurr & 0x01 );		// 2byte정렬 검사
		d = *((short*)m_pCurr);
		m_pCurr += sizeof(short);
		return *this;
	}
	XPacket& operator >> ( char& d ) {
		d = *m_pCurr;
		m_pCurr += sizeof(char);
		return *this;
	}
	// 스트링은 >> 가 없음. 버퍼 오버플로우 처리를 해줄수가 없어서...
	//
	XPacket& operator >> ( int& d ) {
		return *this >> ((long&)d);
	}
	XPacket& operator >> ( unsigned long& d ) {
		return *this >> ((long&)d);
	}
	XPacket& operator >> ( unsigned char& d ) {
		return *this >> ((char&)d);
	}
	XPacket& operator >> ( unsigned short& d ) {
		return *this >> ((short&)d);
	}

};

