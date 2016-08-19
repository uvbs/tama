/*
 *  XImage.h
 *
 *  Created by xuzhu on 11. 5. 19..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once
#include "xe.h"

template<typename T>
class XImage
{
	// 아이폰용 고해상도 리소스로 지정되면 외부에선 이 리소스의 크기는 원래크기의 1/2로 인식한다.
	// 다만 내부메모리는 원래크기의 양이 할당되어 있다.
	// 예) 64x64 png를 고해상도 지정했을때
	// GetWidth() == 32
	// GetMemWidth() == 64
	BOOL m_bHighReso;		// 아이폰용 고해상도 리소스인지?
	float __fResoScale;		// 바른계산을 위해 고해상도용 스케일값을 받아둔다
	XE::VEC2 m_vSize;
	T *m_pTextureData;
	int m_bpp;	// byte per pixel
	TCHAR m_szFilename[1024];
	
	void Init( void ) {
		m_bHighReso = FALSE;
		__fResoScale = 1.0f;
		m_pTextureData = NULL;
		m_bpp = sizeof(T);
		XCLEAR_ARRAY( m_szFilename );
	}
	void Destroy( void ) {
		SAFE_DELETE_ARRAY( m_pTextureData );
	}
protected:
	// 사이즈 지정은 Load할때외엔 외부에서 쓸일이 없으므로 protected로 숨김
	void SetvSize( const XE::VEC2& vSize ) { SetvSize( vSize.x, vSize.y ); }
	template<typename T1>
	void SetSize( T1 w, T1 h ) {		// 사이즈가 세팅될때만 고해상도 플랙 적용해서 저장되고
	#ifdef DEBUG
		if( m_bHighReso && ((int)w & 1) )	// 고해상도 리소스의 가로크기가 /2로 떨어지지 않는경우 경고
			XALERT( "경고:%s의 가로크기가 2로 나누어 떨어지지 않습니다" );
	#endif
		m_vSize.x = w * __fResoScale; 
		m_vSize.y = h * __fResoScale; 
	}		
public:
	XImage( BOOL bHighReso ) {		// 고해상도리소스로 지정되면 화면에 찍힐땐 실제크기의 반으로 찍힌다
		Init(); 
		m_bHighReso = bHighReso; 
		if( bHighReso )		__fResoScale = 0.5f;
	}
	virtual ~XImage() { Destroy(); }
	// get/set
	GET_ACCESSOR( const XE::VEC2&, vSize );		// 이후 부터는 그냥 꺼낸다
	int GetWidth( void ) { return (int)m_vSize.x; }
	int GetHeight( void ) { return (int)m_vSize.y; }
	int GetMemWidth( void ) { return (int)(m_vSize.x / __fResoScale); }	// 실제메모리크기를 알아내기
	int GetMemHeight( void ) { return (int)(m_vSize.y / __fResoScale); }
	T *GetTextureData( void ) { return m_pTextureData; }
	void SetTextureData( T *pTextureData ) { m_pTextureData = pTextureData; }
	void MovePtr( T **ptr )	{	// m_pTextureData를 널로 만들고 그 포인터를 ptr로 옮긴다
		*ptr = m_pTextureData;
		m_pTextureData = NULL;
	}
	//
	int ConvertToMemSize( float num ) { return (int)(num / __fResoScale); }
	float ConvertToSurfaceSize( int num ) { return num * __fResoScale; }
	// virtual 
	virtual BOOL Load( LPCTSTR szFilename ) { _tcscpy_s( m_szFilename, XE::GetFileName(szFilename) ); return TRUE; }
};
