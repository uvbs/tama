#ifndef	__GLOBAL_H__
#define	__GLOBAL_H__

#ifndef TRUE
#define TRUE	1
#endif
//#ifndef FALSE
#ifdef FALSE
#undef FALSE
#endif
#define FALSE	((int)0)
//#endif

#ifndef YES
#define YES	1
#endif
#ifndef NO
#define NO		0
#endif

#ifndef ON
#define ON	1
#endif
#ifndef OFF
#define OFF	0
#endif


#define XNORMAL	0
#define XHIGH		1
#define XLOW		2

typedef enum tagxRESULT {
	xFAIL = 0,
	xSUCCESS
} xRESULT;

//#ifdef SAFE_DELETE
#undef SAFE_DELETE
//#endif
//#ifdef SAFE_DELETE_ARRAY
#undef SAFE_DELETE_ARRAY
//#endif
//#ifdef SAFE_RELEASE
#undef SAFE_RELEASE
//#endif

/*
	if(1) 
		SAFE_DELTE( p );      <- 이런식으로 쓸때 { }; else 형태로 바껴서 매크로를 좀 바꿨음
	else
		p = 111;
*/
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(p) { if(p) { free((p));	(p)=NULL; } }
#endif
#ifndef SAFE_RELEASE2
#define SAFE_RELEASE2( MNG, p) { if(p) { MNG->Release(p); (p)=NULL; } }
#endif

#ifdef WIN32
template<class Interface>
inline void
SafeRelease(
    Interface **ppInterfaceToRelease
    )
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}
#endif // WIN32
#define SET_ACCESSOR( x, y )       inline void Set##y( x t )   { m_##y = t; };
#define GET_ACCESSOR( x, y )       inline x Get##y()           { return m_##y; };
#define GET_SET_ACCESSOR( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR( x, y )

#define IS_FLOAT_SAME(F,F2)	( ((F2)-0.01f) < (F) && ((F2)+0.01f) > (F) )
#define ROUND_OFF(F)		(((F)>0)? (float)((int)((F)+0.5f)) : (float)((int)((F)-0.5f)))	// 소숫점아래 반올림. 반올림맞는거 같은데 왜 '버림'이라고 했지?
#define ROUND_OFF2(F,U)		(((int)(F * (1.f/U))) / (1.f/U))		// 소수점단위로 잘라낼때... ROUND_OFF2( 1.7, 0.5 ) == 1.5	// 0.5단위로 잘라냄
#define ROUND_UP(F)		ROUND_OFF(F)		// 소숫점 아래 반올림

#define xmax(a,b)            (((a) > (b)) ? (a) : (b))
#define xmin(a,b)            (((a) < (b)) ? (a) : (b))
#define XCLEAR_ARRAY(BUFF)	memset( BUFF, 0, sizeof(BUFF) )

#define LIST_DESTROY( LIST, NODE_TYPE )		{			\
									list<NODE_TYPE>::iterator itor; \
									for( itor = LIST.begin(); itor != LIST.end(); ) \
									{	\
										NODE_TYPE pNode = (*itor);		\
										SAFE_DELETE( pNode );		\
										LIST.erase( itor++ );		\
									}	\
								}	\

#define LIST_CLEAR( LIST, NODE_TYPE )		{			\
									list<NODE_TYPE>::iterator itor; \
									for( itor = LIST.begin(); itor != LIST.end(); ) \
									{	\
										LIST.erase( itor++ );		\
									}	\
								}	\

#define LIST_LOOP( LIST, NODE_TYPE, I, E )	{			\
								list<NODE_TYPE>::iterator I; \
								int li;	\
								for( li=0, I = (LIST).begin(); I != (LIST).end(); li++, ++I ) \
								{	\
									NODE_TYPE E = (*I);		\

#define LIST_LOOP_REVERSE( LIST, NODE_TYPE, I, E )	{			\
								list<NODE_TYPE>::reverse_iterator I; \
								for( I = (LIST).rbegin(); I != (LIST).rend(); ++I ) \
								{	\
									NODE_TYPE E = (*I);		\


#define LIST_MANUAL_LOOP( LIST, NODE_TYPE, I, E )	{			\
								list<NODE_TYPE>::iterator I; \
								for( I = LIST.begin(); I != LIST.end(); ) \
								{	\
									NODE_TYPE E = (*I);		\
	
#define END_LOOP		} }

#define GETNEXT_LOOP( OBJ, TYPE, ELEM ) \
{ \
	int _i; \
	OBJ->GetNextClear( &_i ); \
	TYPE ELEM; \
	while( ELEM = OBJ->GetNext( &_i ) ) \
	{ 

#define GETNEXT_LOOP2( OBJ, TYPE, ELEM ) \
{ \
	OBJ->GetNextClear(); \
	TYPE ELEM; \
	while( ELEM = OBJ->GetNext() ) \
	{ 

#define MAP_LOOP( MAP, FIRST_TYPE, SECOND_TYPE, FIRST_ELEM, SECOND_ELEM ) \
{	\
	map<FIRST_TYPE, SECOND_TYPE>::iterator itor;	\
	for( itor = MAP.begin(); itor != MAP.end(); itor++ )	\
	{	\
		FIRST_TYPE FIRST_ELEM = itor->first;	\
		SECOND_TYPE SECOND_ELEM = itor->second; \

#include "debug.h"
//template <typename NEWT, typename OLDT>
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

// 파일 오픈류 루틴에서 버전체크하는 일반적인 매크로.
#define VER_CHECK( FILENAME, FILE_VER, CURR_VER ) \
	if( FILE_VER < CURR_VER )	{	\
		CONSOLE( "경고: %s 파일의 버전(%d)이 최신버전인 %d보다 낮다.", FILENAME, FILE_VER, CURR_VER );	\
	}	\
	if( FILE_VER > CURR_VER )	{	\
		XALERT( "에러: %s 파일의 버전(%d)이 최신버전인 %d보다 높아 열수가 없습니다.", FILENAME, FILE_VER, CURR_VER );	\
		return FALSE;	\
	}


#endif // __GLOBAL_H__
