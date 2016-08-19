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
	// �������� ���ػ� ���ҽ��� �����Ǹ� �ܺο��� �� ���ҽ��� ũ��� ����ũ���� 1/2�� �ν��Ѵ�.
	// �ٸ� ���θ޸𸮴� ����ũ���� ���� �Ҵ�Ǿ� �ִ�.
	// ��) 64x64 png�� ���ػ� ����������
	// GetWidth() == 32
	// GetMemWidth() == 64
	BOOL m_bHighReso;		// �������� ���ػ� ���ҽ�����?
	float __fResoScale;		// �ٸ������ ���� ���ػ󵵿� �����ϰ��� �޾Ƶд�
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
	// ������ ������ Load�Ҷ��ܿ� �ܺο��� ������ �����Ƿ� protected�� ����
	void SetvSize( const XE::VEC2& vSize ) { SetvSize( vSize.x, vSize.y ); }
	template<typename T1>
	void SetSize( T1 w, T1 h ) {		// ����� ���õɶ��� ���ػ� �÷� �����ؼ� ����ǰ�
	#ifdef DEBUG
		if( m_bHighReso && ((int)w & 1) )	// ���ػ� ���ҽ��� ����ũ�Ⱑ /2�� �������� �ʴ°�� ���
			XALERT( "���:%s�� ����ũ�Ⱑ 2�� ������ �������� �ʽ��ϴ�" );
	#endif
		m_vSize.x = w * __fResoScale; 
		m_vSize.y = h * __fResoScale; 
	}		
public:
	XImage( BOOL bHighReso ) {		// ���ػ󵵸��ҽ��� �����Ǹ� ȭ�鿡 ������ ����ũ���� ������ ������
		Init(); 
		m_bHighReso = bHighReso; 
		if( bHighReso )		__fResoScale = 0.5f;
	}
	virtual ~XImage() { Destroy(); }
	// get/set
	GET_ACCESSOR( const XE::VEC2&, vSize );		// ���� ���ʹ� �׳� ������
	int GetWidth( void ) { return (int)m_vSize.x; }
	int GetHeight( void ) { return (int)m_vSize.y; }
	int GetMemWidth( void ) { return (int)(m_vSize.x / __fResoScale); }	// �����޸�ũ�⸦ �˾Ƴ���
	int GetMemHeight( void ) { return (int)(m_vSize.y / __fResoScale); }
	T *GetTextureData( void ) { return m_pTextureData; }
	void SetTextureData( T *pTextureData ) { m_pTextureData = pTextureData; }
	void MovePtr( T **ptr )	{	// m_pTextureData�� �η� ����� �� �����͸� ptr�� �ű��
		*ptr = m_pTextureData;
		m_pTextureData = NULL;
	}
	//
	int ConvertToMemSize( float num ) { return (int)(num / __fResoScale); }
	float ConvertToSurfaceSize( int num ) { return num * __fResoScale; }
	// virtual 
	virtual BOOL Load( LPCTSTR szFilename ) { _tcscpy_s( m_szFilename, XE::GetFileName(szFilename) ); return TRUE; }
};
