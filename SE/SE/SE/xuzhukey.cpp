#include "stdafx.h"
void GetPathA( char *szBuff, const char *strFullPath, int nBuffSize )
{
	strcpy_s( szBuff, nBuffSize, strFullPath );
	char *p = strrchr( szBuff, '\\' );
	*p = 0;
}

void GetCwdA( char *szBuff, int nSize )
{
	char szFull[MAX_PATH];
	GetModuleFileNameA( nullptr, szFull, MAX_PATH );		// 실행파일의 폴더를 얻어옴
	GetPathA( szBuff, szFull, MAX_PATH );						// 패스부분만 떼냄.
}

DWORD _InitApp( HWND hWnd ) 
{ 
     DWORD SerialNum; 
     DWORD A,B; 
     char   C[256]; 
     char   Buffer[256]; 

     if(GetVolumeInformationA("C:\\", Buffer, 256, &SerialNum, &A, &B, C, 256)) { 
		/*
		소스
		. 만약 툴소스를 공개한다면 이부분은 dll로 빼고 이곳에서 실패한상황에선 exit()뿐만아니라 전역상수들(SPRVER같은)값을 바꾸자
		배포
		1. SE.cfg 바이너리 파일을 만들고 초기값 어떤암호화 스트링을 넣어 배포한다.
		실행
		1. SE.cfg를 읽는다. 없으면 파일없음을 알리고 끝낸다.
		2. SE.cfg를 읽어서 암호스트링을 읽는다. 
		3-1. 초기값이면 볼륨시리얼을 구해서 암호화시킨다음 다시 SE.cfg에 저장한다.
		3-2. 초기값이 아니면 시스템의 볼륨시리얼과 비교해서 다르면 조용히 끝낸다.
		*/
		const unsigned char n = 0x64;
		// ini check
		unsigned char fnc[] = { '\\'+n, 's'+n, 'e'+n, '.'+n, 'c'+n, 'f'+n, 'g'+n, 0+n };
		char fn[8];
		for( int i = 0; i < 8; i ++ )	fn[i] = fnc[i] - n;
		char szFull[MAX_PATH];
		GetCwdA( szFull, MAX_PATH );
		strcat_s( szFull, fn );
		FILE *fp = fopen( szFull, "rb" );
		if( fp == nullptr ) {
			unsigned char ec[] = { 's'+n, 'e'+n, '.'+n, 'c'+n, 'f'+n, 'g'+n, ' '+n, 'n'+n, 'o'+n, 't'+n, ' '+n, 'f'+n, 'o'+n, 'u'+n, 'n'+n, 'd'+n, 0+n };
			for( int i = 0; i < (int)strlen((const char *)ec)+1; i ++ )		szFull[i] = ec[i] - n;
			::MessageBoxA( hWnd, szFull, "", MB_OK );
			exit(1);
		}
		// init num check
//		unsigned char isc[256] = { '1'+n, '7'+n, '1'+n, '8'+n, '0'+n, '0'+n, '0'+n, '2'+n, '4'+n, '1'+n, '1'+n, '0'+n, '5'+n }
		long long in = 1718000241105;
//		char is[256] = { 0, };
		char szsn[64] = { 0, };
		char szhc[64] = { 0, };
		_i64toa( (__int64)SerialNum, szsn, 10 );
		fseek( fp, 0, SEEK_END );
		int size = ftell( fp );			// file size
		fseek( fp, 0, SEEK_SET );
		fread(  szhc, size, 1, fp );
		for( int i = 0; i < (int)strlen(szhc); i ++ )		szhc[i] -= n;
		szhc[size] = 0;
		__int64 hc = _atoi64( szhc );
		if( in == (long long)hc )
		{
			fclose(fp);
			FILE *fp2 = fopen( szFull, "wb" );
			for( int i = 0; i < (int)strlen(szsn); i ++ )		szsn[i] += n;
			fwrite( szsn, strlen(szsn), 1, fp );
			fclose(fp2);
			return 1;
		}
		// incorrect check
		if( SerialNum != (DWORD)hc )
		{
			unsigned char ec[13] = { 'D'+n, 'i'+n, 'r'+n, 'e'+n, 'c'+n, 't'+n, 'X'+n, 'E'+n, 'r'+n, 'r'+n, 'o'+n, 'r'+n, 0+n };
			char e[13] = { 0, };
			for( int i = 0; i < 13; i ++ )	e[i] = ec[i] - n;
			::MessageBoxA( hWnd, e, "", MB_OK );
			exit(1);
		}
		fclose(fp);
        return 1;
     } 
	 return 0;
} 
