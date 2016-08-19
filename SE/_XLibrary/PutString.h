#pragma once 

#if 0 
#include "XGraphics.h"
#include "SprDat.h"
#include "SprMng.h"

enum xAlign 
{
	xALIGN_NONE = 0,
	xALIGN_LEFT = 0x01,
	xALIGN_RIGHT = 0x02,
	xALIGN_HCENTER = 0x04,
	xALIGN_VCENTER = 0x08,
	xALIGN_CENTER = xALIGN_HCENTER | xALIGN_VCENTER,
};

class XPutString
{
	float _m_fScaleX, _m_fScaleY;	// PushScale()에서 사용
	float m_Alpha;
	void Init( void ) {
		m_fDist = 0;		// 
		m_pSprDat = NULL;
		SetScale( 1.0f, 1.0f );
		_m_fScaleX = _m_fScaleY = 1.0f;
		m_Alpha = 1.0f;
	}
	void Destroy( void ) {
		SPRMNG->Destroy( m_pSprDat );
	}
protected:
	XSprDat *m_pSprDat;
	float m_fDist;			// 글자사이 간격
	float m_fScaleX, m_fScaleY;

public:
	XDrawString() { Init(); }
	XDrawString( LPCTSTR szFilename ) {
		Init();
		m_pSprDat = SPRMNG->Load( szFilename, TRUE );
		if( m_pSprDat == NULL )
			XERROR( "%s읽기 실패", szFilename );
			
	}
	virtual ~XDrawString() { Destroy(); }

	GET_SET_ACCESSOR( float, fDist );
	GET_SET_ACCESSOR( float, Alpha );
//	float GetfScaleX() { return m_fScaleX / GRAPHICS->GetGScale().x; }
//	float GetfScaleY() { return m_fScaleY / GRAPHICS->GetGScale().y; }
	float GetfScaleX() { return m_fScaleX; }
	float GetfScaleY() { return m_fScaleY; }
//	void _SetScale( float sx, float sy ) { m_fScaleX = GRAPHICS->GetGScale().x * sx; m_fScaleY = GRAPHICS->GetGScale().y * sy; }	// 고해상도 리소스를 쓰면 0.5를 곱해줘야 한다.
	void SetScale( float sx, float sy ) { m_fScaleX = sx; m_fScaleY = sy; }	
	void SetScale( float scalexy ) { SetScale( scalexy, scalexy ); }
	void SetScale( const XE::VEC2& vScale ) { SetScale( vScale.x, vScale.y ); }

	void PushScale( void ) { _m_fScaleX = GetfScaleX(); _m_fScaleY = GetfScaleY(); }
	void PopScale( void ) { SetScale( _m_fScaleX, _m_fScaleY ); }
	float GetFontHeight( void );
	float GetLengthPixel( LPCTSTR str );

	virtual int GetCharToIdx( TCHAR c );
	float DrawNumber( const XE::VEC2& vPos, int num, xAlign align = xALIGN_LEFT ) { return DrawNumber( vPos.x, vPos.y, num, align ); }
	float DrawNumber( float x, float y, int num, xAlign align = xALIGN_LEFT );
	float DrawFloat( float x, float y, float num, xAlign align = xALIGN_LEFT );
//	float DrawString( float x, float y, LPCTSTR format, ... ) { return DrawString(x, y, xALIGN_LEFT, format, __VA_ARGS__ ); }	
	float DrawString( float x, float y, xAlign align, LPCTSTR format, ... );
	virtual float DrawString( float x, float y, LPCTSTR str, xAlign align = xALIGN_LEFT );
	float PutStringFit(  float x, float y, float wFit, float hFit, LPCTSTR str, xAlign align=xALIGN_LEFT  );
}; // class XPutString

// 숫자폰트만 있는버전
class XPutStringNumber : public XPutString
{
public:
	XPutStringNumber( LPCTSTR szFilename ) : XDrawString( szFilename ) {
	}
	virtual ~XPutStringNumber() {}

	virtual int GetCharToIdx( TCHAR c );
};

// 숫자폰트 + : m s
class XPutStringTime : public XPutString
{
public:
	XPutStringTime( LPCTSTR szFilename ) : XDrawString( szFilename ) {
		m_fDist = 1;
	}
	virtual ~XPutStringTime() {}

	virtual int GetCharToIdx( TCHAR c );
};

// 알파벳과 숫자 특수문자 폰트 모두 있는 버전
class XPutStringAll : public XPutString
{
	void Init( void ) {
	}
	void Destroy( void ) {
	}
public:
	XPutStringAll() { Init(); }
	XPutStringAll( LPCTSTR szFilename ) : XDrawString( szFilename )		// ww:글자사이 간격(기본이 0)
	{
		Init();
	}
	virtual ~XPutStringAll() { Destroy(); }

	virtual int GetCharToIdx( TCHAR c );
};

#endif // 0