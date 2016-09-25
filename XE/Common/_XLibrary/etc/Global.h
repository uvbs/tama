#pragma once
//#ifndef	__GLOBAL_H__
//#define	__GLOBAL_H__

//#include "debug.h"

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
#define ON	TRUE
#endif
#ifndef OFF
#define OFF	FALSE
#endif

#ifndef NULL
#define NULL    0
#endif

#define XNORMAL	0
#define XHIGH		1
#define XLOW		2
//#define		SUCCESS		1
//#define		FAIL			-1
#ifndef XFPS		// 프리컴파일헤더나 version.h등에서 XFPS가 정의되었다면 자체 디파인을 쓰지 않는다
#define XFPS		(60.0f)
#endif
#define XSPF		(1.0f/XFPS)		// sec per frame(프레임당 sec)

// XE::것을 쓸것.
enum xRESULT {
	xFAIL = 0,
	xSUCCESS
};
namespace XE {
	enum xRESULT {
		xFAIL = 0,
		xSUCCESS
	};
};
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#undef SAFE_RELEASE
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
#define SAFE_RELEASE2( MNG, p) { if(p && MNG) { MNG->Release(p); (p)=NULL; } }
#endif
#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(h)  { \
	if(h != INVALID_HANDLE_VALUE && h != nullptr) { \
		CloseHandle(h);     \
		(h)=0; } } 
#endif

#ifdef WIN32
template<class Interface>
inline void SafeRelease( Interface **ppInterfaceToRelease ) {
    if (*ppInterfaceToRelease != nullptr) {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = nullptr;
    }
}
#endif // WIN32
#define SET_ACCESSOR( x, y )       inline void Set##y( x t )   { m_##y = t; };
#define GET_ACCESSOR( x, y )       inline x Get##y()           { return m_##y; };
#define GET_ACCESSOR_CONST( x, y )	inline x Get##y() const { return m_##y; };
#define GET_ACCESSOR_MUTABLE( x, y )	inline x Get##y##Mutable() { return m_##y; };
#define GET_SET_ACCESSOR( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR( x, y )
#define GET_SET_ACCESSOR_CONST( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR_CONST( x, y )
#define GET_SHARED_ACCESSOR( x, y )       inline x Get##y() const { return m_##y.lock(); };
#define GET_SET_SHARED_ACCESSOR( x, y )       SET_ACCESSOR( x, y ) GET_SHARED_ACCESSOR( x, y )
// shared_ptr전용
#define GET_ACCESSOR2( x, y ) \
	inline x##Const Get##y() const { return m_##y; }; \
	inline x Get##y##Mutable() { return m_##y; };

#define GET_SET_ACCESSOR2( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR2( x, y )

// 일반 포인터용
#define GET_ACCESSOR_PTR( x, y ) \
	inline const x Get##y() const { return m_##y; }; \
	inline x Get##y##Mutable() { return m_##y; };
#define GET_SET_ACCESSOR_PTR( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR_PTR( x, y )
///< 
/**
 @brief type형 m_name변수의 add macro(템플릿 객체형태로 만들면 더 좋을듯.)
 IsOverAddXXXX: m_name변수에 add를 더하거나 빼려고할때 값이 오버나 언더되는지 검사
 AddXXXX: m_name변수에 add값을 더한다. add값은 마이너스가 될수도 있다.
 unsinged형은 아직 사용할 수 없다.
*/
#define ADD_SIGNED_ACCESSOR( type, name ) \
	inline bool IsOverAdd##name( type add ) const { \
		if( add < 0 ) { \
			return ( m_##name < -add ); \
		} else { \
			int maxVal = 0x7fffffff; \
			if( sizeof( type ) == 2 ) \
				maxVal = 0x7fff; \
			else if( sizeof( type ) == 1 ) \
				maxVal = 0x7f; \
			return ( maxVal - m_##name < add ); \
		} \
		return false; \
	} \
	inline type Add##name( type add ) { \
		if( add < 0 ) { \
			if( m_##name < -add ) \
				m_##name = 0; \
			else \
			m_##name += add; \
		} else { \
			int maxVal = 0x7fffffff; \
			if( sizeof(type) == 2 ) \
				maxVal = 0x7fff; \
			else if( sizeof(type) == 1 ) \
				maxVal = 0x7f; \
			if( maxVal - m_##name < add ) \
				m_##name = maxVal; \
			else \
				m_##name += add; \
		} \
		return m_##name; \
	}
#define GET_ADD_ACCESSOR( type, name )	GET_ACCESSOR_CONST( type, name ) ADD_SIGNED_ACCESSOR( type, name )
#define GET_SET_ADD_ACCESSOR( type, name )	GET_SET_ACCESSOR_CONST( type, name ) ADD_SIGNED_ACCESSOR( type, name )

/**
 @brief 스태틱 멤버 전용 억세서
*/
#define sSET_ACCESSOR( x, y )       inline static void sSet##y( x t )   { s_##y = t; };
#define sGET_ACCESSOR( x, y )       inline static x sGet##y()           { return s_##y; };
#define sGET_SET_ACCESSOR( x, y )   sSET_ACCESSOR( x, y ) sGET_ACCESSOR( x, y )

#define GET_BOOL_ACCESSOR( y )       inline bool Get##y() const { return m_##y; }  inline bool Is##y() const { return m_##y; }   
#define SET_BOOL_ACCESSOR( y )       inline void Set##y( bool flag ) { m_##y = flag; } inline bool Toggle##y() { m_##y = !m_##y; return m_##y; }
#define GET_SET_BOOL_ACCESSOR( y )   SET_BOOL_ACCESSOR( y ) GET_BOOL_ACCESSOR( y )

/**
 ex: GET_SET_TSTRING_ACCESSOR( strName );
 는 아래와 같다.
 LPCTSTR GetstrName( void ) {
	return m_strName.c_str();
 }
 void SetstrName( LPCTSTR strName ) {
	m_strName = strName;
 }
*/
#define SET_TSTRING_ACCESSOR( y )	inline void Set##y( LPCTSTR t ) { m_##y = t; };\
																	inline void Set##y( const _tstring& t ) { m_##y = t; };
#define GET_TSTRING_ACCESSOR( y )	inline LPCTSTR Get##y() const { return m_##y.c_str(); };\
																	inline const _tstring& Get_##y() const { return m_##y; };
#define GET_SET_TSTRING_ACCESSOR( y )   SET_TSTRING_ACCESSOR( y ) GET_TSTRING_ACCESSOR( y )
#ifdef WIN32
#define SET_STRING_ACCESSOR( y )		inline void Set##y( const char* t )   { m_##y = t; };
#define GET_STRING_ACCESSOR( y )		inline const char* Get##y()           { return m_##y.c_str(); };
#else
#define SET_STRING_ACCESSOR( y )		SET_TSTRING_ACCESSOR( y )
#define GET_STRING_ACCESSOR( y )		GET_TSTRING_ACCESSOR( y )
#endif
#define GET_SET_STRING_ACCESSOR( y )   SET_STRING_ACCESSOR( y ) GET_STRING_ACCESSOR( y )
#define GET_SET_TSTRING_ACCESSOR( y )   SET_TSTRING_ACCESSOR( y ) GET_TSTRING_ACCESSOR( y )


// 초기화때 이외에는 이 변수에 대해 읽기만 한다는걸 보장한다.
#define GET_READONLY_ACCESSOR( x, y )       inline x Get##y() const           { return m_##y; };

#define IS_FLOAT_SAME(F,F2)	( ((F2)-0.01f) < (F) && ((F2)+0.01f) > (F) )
#define ROUND_OFF(F)		(((F)>0)? (float)((int)((F)+0.5f)) : (float)((int)((F)-0.5f)))	// 소숫점아래 반올림. 반올림맞는거 같은데 왜 '버림'이라고 했지?
#define ROUND_OFF2(F,U)		(((int)(F * (1.f/U))) / (1.f/U))		// 소수점단위로 잘라낼때... ROUND_OFF2( 1.7, 0.5 ) == 1.5	// 0.5단위로 잘라냄
#define ROUND_UP(F)		ROUND_OFF(F)		// 소숫점 아래 반올림
/**
 @brief 소숫점 반올림
 @param x 반올림하려는 실수
 @param dig 반올림 하려는 소숫점 자리수
 ex) ROUND_FLOAT( 10.6f, 0 ) == 11.0f
*/
//#define ROUND_FLOAT(x, dig)    (floor((x) * pow(10, dig) + 0.5f) / pow(10, dig))
inline float ROUND_FLOAT( float x, float dig ) {
	return (float)(floor((x) * std::pow(10, dig) + 0.5f) / std::pow(10, dig));
}
// 소숫점 아래 올림
inline float ROUND_AUP( float num ) {
	if( num - (int)num > 0 )		// 소숫점 아래에 값이 있으면 1 올림
		return ((int)num) + 1.f;
	return (float)((int)num);		// 아니면 소숫점 잘라냄
}

// namespace XGAME {
// };

// 배열 요소 개수세기
#define XNUM_ARRAY(ARY)		(sizeof(ARY) / sizeof(ARY[0]))
#define XARRAY_LENGTH(ARY)		XNUM_ARRAY(ARY)

#define xmax(a,b)            (((a) > (b)) ? (a) : (b))
#define xmin(a,b)            (((a) < (b)) ? (a) : (b))
#define XCLEAR_ARRAY(BUFF)	memset( BUFF, 0, sizeof(BUFF) )
#define XCLEAR_ARRAY_WITH_VAL(BUFF,VAL)	memset( BUFF, VAL, sizeof(BUFF) )
#define LOOP_ARRAY( IDX, ARY ) \
	for( int IDX = 0; IDX < XNUM_ARRAY(ARY); ++IDX )
		

#define LIST_DESTROY( LIST, NODE_TYPE )		{			\
	for( auto itor = LIST.begin(); itor != LIST.end(); ) \
	{	\
		NODE_TYPE pNode = (*itor);		\
		SAFE_DELETE( pNode );		\
		LIST.erase( itor++ );		\
	}	\
	LIST.clear(); \
}	\

#define LIST_RELEASE( LIST, NODE_TYPE )		{			\
							for( auto itor = LIST.begin(); itor != LIST.end(); ) \
							{	\
								NODE_TYPE pNode = (*itor);		\
								SAFE_RELEASE_REF( pNode );		\
								LIST.erase( itor++ );		\
							}	\
						}	\

#define LIST_CLEAR( LIST, NODE_TYPE )		{			\
									for( auto itor = LIST.begin(); itor != LIST.end(); ) \
									{	\
										LIST.erase( itor++ );		\
									}	\
								}	\

#define LIST_LOOP( LIST, NODE_TYPE, I, E )	{ \
	for( auto& E : LIST ) {
// 	std::list<NODE_TYPE>::iterator I; \
// 	int li;	\
// 	for( li=0, I = (LIST).begin(); I != (LIST).end(); li++, ++I ) \
// 	{	\
// 		NODE_TYPE E = (*I);		\

#define LIST_LOOP_REVERSE( LIST, NODE_TYPE, I, E )	{			\
								std::list<NODE_TYPE>::reverse_iterator I; \
								for( I = (LIST).rbegin(); I != (LIST).rend(); ++I ) {	\
									NODE_TYPE E = (*I);


#define LIST_MANUAL_LOOP( LIST, NODE_TYPE, I, E )	{			\
								for( auto I = (LIST).begin(); I != (LIST).end(); ) {	\
									NODE_TYPE E = (*I);

#define LIST_MANUAL_LOOP_AUTO( LIST, I, E )	{			\
								for( auto I = (LIST).begin(); I != (LIST).end(); ) {	\
									auto E = (*I);

#define LISTREF_LOOP( LIST, NODE_TYPE, I )	{			\
	int li;	\
	for( auto li=0, I = (LIST).begin(); I != (LIST).end(); li++, ++I ) {

/**
 @brief iterator I를 수동으로 증가시켜주는 버전
*/
#define LISTREF_MANUAL_LOOP( LIST, NODE_TYPE, I )	{			\
	for( auto I = (LIST).begin(); I != (LIST).end(); ) {

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


// 파일 오픈류 루틴에서 버전체크하는 일반적인 매크로.
#define VER_CHECK( FILENAME, FILE_VER, CURR_VER ) \
	if( FILE_VER < CURR_VER )	{	\
		CONSOLE( "경고: %s 파일의 버전(%d)이 최신버전인 %d보다 낮다.", FILENAME, FILE_VER, CURR_VER );	\
	}	\
	if( FILE_VER > CURR_VER )	{	\
		XALERT( "에러: %s 파일의 버전(%d)이 최신버전인 %d보다 높아 열수가 없습니다.", FILENAME, FILE_VER, CURR_VER );	\
		return FALSE;	\
	}

#define XE_NAMESPACE_START(NAME)    namespace NAME {
#define XE_NAMESPACE_END    }

XE_NAMESPACE_START( XE )
/**
	val를 valDiv로 나눠 나머지가 있으면 valDiv단위로 올림을 한다
	ex) 32->32  33->64
*/
inline int RoundUpDivVal( int val, int valDiv ) {
	int div = (int)val / valDiv;
	int rem = (int)val % valDiv;
	if( rem > 0 )
		++div;
	int ret = div * valDiv;
	return ret;
}
// inline XE::VEC2 RoundUpDivVal( const XE::VEC2& v, const XE::VEC2& vDiv ) {
// 	auto ptDiv = v.ToPoint() / vDiv.ToPoint();
// 	auto ptRem = v.ToPoint() % vDiv.ToPoint();
// 	if( ptRem.x > 0 )
// 		++ptDiv.x;
// 	if( ptRem.y > 0 )
// 		++ptDiv.y;
// 	return ptDiv.ToVec2() * vDiv;
// }
//
XE_NAMESPACE_END; // XE


//#endif // __GLOBAL_H__
