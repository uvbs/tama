#include "stdafx.h"
#include "CrashHandler.h"

#ifdef _VER_ANDROID
#include <signal.h>
#include <dlfcn.h>
namespace XE
{
	static FILE *s_fp = nullptr;
//////////////////////////////////////////////////////////////////////////
void __backtrace( FILE *pFile, void* pFunc, char* trace )
{
	//XTRACE("__backtrace");
	if (pFunc == 0) {
//		XLOG("%s", "fp == 0");
		//XTRACE("fp == 0");
		return;
	}
	static int s_cnt = 0;
	++s_cnt;
// 	XLOG("__backtrace(%d):0x%08x", s_cnt, (DWORD)fp);
	void* funcAddr = (void*) *((int*)pFunc);	// NDK 컴파일러에 의한 코드는 fp의 위치에 호출된 함수의 주소 저장

	Dl_info info;
// 	XLOG("dladdr() funcAddr=0x%08x", (DWORD)funcAddr);
	if( dladdr( funcAddr, &info ) ) {
// 		XLOG("dladdr()in:%d(%s)", info.dli_fbase, info.dli_fname );
		funcAddr = (void*) ( (unsigned int)funcAddr - (unsigned int)info.dli_fbase );	// funcAddr은 프로세스의 메모리 상의 주소이므로, 라이브러리상의 함수 주소로 변환
		char buffer[1024];
		sprintf( buffer, "%s %p\r\n", info.dli_fname, funcAddr);
		strcat( trace, buffer );
		fprintf_s( pFile, "%s", buffer );
		fflush( pFile );
// 		XLOG("call line:%s", buffer );
	}
//	XLOG("%s", "dladdr()out" );
	__backtrace( pFile, (void*)(*((int*)pFunc - 1)), trace );	// fp 위치의 다음 필드가 이전의 fp
}

// 현재 context의 stack dump를 trace에 텍스트로 저장
void GetBacktraces( FILE *fp, char* trace )
{
	__backtrace( fp, __builtin_frame_address(0), trace );
}
// 크래쉬(signal) 발생시 호출됨
void SignalHandler(int signal, siginfo_t *info, void *reserved)
{
	XLOG("%s","crash! write dump.bin2");
	//
	static int s_cnt = 0;
	if( ++s_cnt > 2 ) {
		if( s_fp ) {
			fprintf_s( s_fp, "exit\r\n" );
			fflush( s_fp );
			fclose(s_fp);
		}
		exit(1);
		return;
	}
	{
// 		FILE *fp = NULL;
		const char *cPath = XE::MakeDocFullPath( "", "dump.bin");
		if( !s_fp ) {
			fopen_s( &s_fp, cPath, "wt" );
		}
		if( s_fp ) {
			char backtraces[8191]={0,};
			GetBacktraces( s_fp, backtraces );	// 위에서 구현했던 콜스택 덤프 기능
			fprintf_s( s_fp, "callstack end\r\n" );
			// 콜스택 뽑다가 다시 죽는경우도 있으므로 이곳으로 반드시 나오지는 않음.
			fclose(s_fp);
			XLOG("%s", "write dump success");
		} else {
			XLOG("%s","crash: write failed.[%s]", cPath );
		}
	}
}
// void __backtrace( void* fp, char* trace )
// {
// 	//XTRACE("__backtrace");
// 	if (fp == 0) {
// 		XLOG("%s", "fp == 0");
// 		//XTRACE("fp == 0");
// 		return;
// 	}
// 	static int s_cnt = 0;
// 	++s_cnt;
// 	XLOG("__backtrace(%d):0x%08x", s_cnt, (DWORD)fp);
// 	void* funcAddr = (void*) *((int*)fp);	// NDK 컴파일러에 의한 코드는 fp의 위치에 호출된 함수의 주소 저장
// 
// 	Dl_info info;
// 	XLOG("dladdr() funcAddr=0x%08x", (DWORD)funcAddr);
// 	if( dladdr( funcAddr, &info ) ) {
// 		XLOG("dladdr()in:%d(%s)", info.dli_fbase, info.dli_fname );
// 		funcAddr = (void*) ( (unsigned int)funcAddr - (unsigned int)info.dli_fbase );	// funcAddr은 프로세스의 메모리 상의 주소이므로, 라이브러리상의 함수 주소로 변환
// 		char buffer[1024];
// 		sprintf( buffer, "%s %p\r\n", info.dli_fname, funcAddr);
// 		strcat( trace, buffer );
// 		XLOG("call line:%s", buffer );
// 	}
// 	XLOG("%s", "dladdr()out" );
// 	__backtrace( (void*)(*((int*)fp - 1)), trace );	// fp 위치의 다음 필드가 이전의 fp
// }
// 
// // 현재 context의 stack dump를 trace에 텍스트로 저장
// void GetBacktraces( FILE *fp, char* trace )
// {
// 	__backtrace( __builtin_frame_address(0), trace );
// }
// // 크래쉬(signal) 발생시 호출됨
// void SignalHandler(int signal, siginfo_t *info, void *reserved)
// {
// 	XLOG("%s","crash! write dump.bin");
// 	char backtraces[8191]={0,};
// 	GetBacktraces( backtraces );	// 위에서 구현했던 콜스택 덤프 기능
// 	//
// 	XLOG("%s", "call stack");
// 	XLOG("%s", backtraces);
// 	{
// 		FILE *fp = NULL;
// 		const char *cPath = XE::MakeDocFullPath( "", "dump.bin");
// 		fopen_s( &fp, cPath, "wt" );
// 		if( fp ) {
// 			XLOG("write dump. [%s]", cPath);
// 			XLOG("%s", backtraces );
// 			fprintf_s( fp, "%s", backtraces );
// 			fclose(fp);
// 		} else {
// 			XLOG("%s","crash: write failed.[%s]", cPath );
// 		}
// 	}
// }

// 크래쉬 핸들러 설정
void AddSignalHandlers()
{
	__xLog( XLOGTYPE_LOG, _T("setting crash handler") );
// 	XLOG("%s","setting crash handler");
	struct sigaction handler;
	struct sigaction old_sigactions[NSIG];
	memset(&handler, 0, sizeof(struct sigaction));

	handler.sa_sigaction = SignalHandler;
	handler.sa_flags = SA_RESETHAND;
#define CATCHSIG(X) { \
		int _r = sigaction(X, &handler, &old_sigactions[X]); \
		if( _r == -1 ) {\
			__xLogf( XLOGTYPE_ERROR, "CATCHSIG failed:%d", X); \
		} \
	}

	CATCHSIG(SIGILL);
	CATCHSIG(SIGABRT);
	CATCHSIG(SIGBUS);
	CATCHSIG(SIGFPE);
	CATCHSIG(SIGSEGV);
	CATCHSIG(SIGSTKFLT);
	CATCHSIG(SIGPIPE);
}
} // namespace XE
#endif // android
