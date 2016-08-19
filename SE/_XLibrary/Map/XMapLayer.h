#pragma once

#include "xe.h"
#include "global.h"

class XMap;
// �ʷ��̾� �⺻��
#ifdef _XTOOL
#include "Select.h"
class XMapLayer : public XBaseElem
#else
class XMapLayer
#endif
{
public:
	enum xtType { xTYPE_NONE=0, xTYPE_TILE, xTYPE_IMAGE };
private:
	void Init() {
		m_idLayer = 0;
		m_Type = xTYPE_NONE;
		m_prefMap = NULL;
		m_bVisible = TRUE;
		m_Alpha = 1.0f;
		m_bShowMask = TRUE;
//		XCLEAR_ARRAY( m_szName );
	}
	void Destroy() {}
protected:
	ID m_idLayer;
	xtType m_Type;
//	TCHAR m_szName[ 32 ];
	XMap *m_prefMap;		// �� ���̾ �����ϰ� �ִ� ���� ������
	BOOL m_bVisible;		// ���̾� ����/�Ⱥ���.
	BOOL m_bShowMask;	// �浹����ũ ���� �Ⱥ���.
	float m_Alpha;			
	XE::VEC2 m_vDrawOffset;		// �ణ�� ������ �� �ְ� ��.
public:
	XMapLayer( XMap *pMap, xtType type, ID idLayer=0, LPCTSTR szName=NULL ) { 
		Init(); 
		if( idLayer == 0 )		// ������ ��ȣ�� ������ ��ȣ����.
			m_idLayer = XE::GenerateID();
		else
			m_idLayer = idLayer;
//		if( szName )
//			_tcscpy_s( m_szName, szName );
		m_Type = type;
		m_prefMap = pMap;
	}
	virtual ~XMapLayer() { Destroy(); }
	//
//	GET_ACCESSOR( LPCTSTR, szName );
//	void SetszName( LPCTSTR szName ) {
//		if( szName )
//			_tcscpy_s( m_szName, szName );
//	}
	GET_ACCESSOR( xtType, Type );
	GET_ACCESSOR( ID, idLayer );
	GET_SET_ACCESSOR( BOOL, bVisible );
	GET_SET_ACCESSOR( float, Alpha );
	GET_SET_ACCESSOR( XE::VEC2&, vDrawOffset );
	GET_SET_ACCESSOR( BOOL, bShowMask );
	//
	virtual void FrameMove( float dt ) {}
	virtual void Draw( void ) {}
	virtual void Load( XResFile *pRes, LPCTSTR szPath );
	virtual BYTE GetCollision( float wx, float wy ) { return 0; }
	// tool ����. ���߿� �ٸ� Ŭ������ ������.
	virtual void DrawLayer( int y, int cw, int ch, BYTE alpha=255 ) {}
//	virtual void Save( XResFile *pRes );
//	virtual void PaintBrush( int wx, int wy, XTileBrush *pBrush );
//	virtual void EraseBrush( int wx, int wy, XTileBrush *pBrush );
	friend class XMap;
};
