﻿#ifndef	__OBJECT3D_H__
#define	__OBJECT3D_H__
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include "Mathematics.h"
#if 1
#include "Material.h"
#include "Bone.h"
/*
#define D3DFVF_SKINVERTEX_BUMP (D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_SKINVERTEX (D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_NORMALVERTEX_BUMP (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_NORMALVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_SHADOWMASKVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
 */
struct SHADOWMASK_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

// Mesh Type
enum GMTYPE
{
	GMT_ERROR = -1,
	GMT_NORMAL,
	GMT_SKIN,
	GMT_BONE
};

// Vertex Buffer Type
enum VBTYPE
{
	VBT_ERROR = -1,
	VBT_NORMAL,
	VBT_BUMP,
};


struct	INDEX_BUFFER
{
	int		nIdx;				// 버텍스 리스트의 인덱스
	Vec3		normal;
};

struct SKINVERTEX
{
    Vec3 position;	// The 3D position for the vertex
	FLOAT		w1, w2;		// vertex weight
	GLubyte		matIdx1, matIdx2;
	GLshort		wReserved;
    Vec3 normal;		// The surface normal for the vertex
    FLOAT       tu, tv;		// The texture coordinates
};

struct NORMALVERTEX
{
    Vec3 position;	// The 3D position for the vertex
    Vec3 normal;		// The surface normal for the vertex
    FLOAT       tu, tv;		// The texture coordinates
};

struct	MATERIAL_BLOCK
{
	WORD	*m_pIB;
	GLuint		m_nGL_IB;		// Indexed용 IB
//	int		m_nStartVertex;			// 버텍스 버퍼에서의 시작 인덱스
	int		m_nPrimitiveCount;		// 프리미티브(삼각형)갯수
	int		m_nTextureID;			// 텍스쳐 번호
	DWORD	m_dwEffect;
	int		m_nAmount;		// 투명도
	int		m_nMaxUseBone;
	int		m_UseBone[MAX_VS_BONE];
};

struct GMOBJECT;
//
// 버텍스와 면정보를 갖는 메쉬형태의 데이타 구조
// Geometry Object
//
struct GMOBJECT		// base object
{
	GMTYPE	m_Type;					// GMOBJECT Type
	int			m_nVertexSize;		// sizeof(FVF)
//	DWORD		m_dwFVF;			// FVF 포맷
	int			m_nMaxUseBone;		
	int			m_UseBone[ MAX_VS_BONE ];
	int			m_nID;
	int			m_bOpacity;
	int			m_bBump;
	int			m_bRigid;
	int			m_bLight;
	int			m_nMaxVertexList;			// read
	int			m_nMaxFaceList;				// read
	int			m_nMaxMtrlBlk;				// read
	int			m_nMaxMaterial;				// read
	GMTYPE		m_ParentType;				// 부모의 타입
	int			m_nParentIdx;				// read		// 스킨일땐 본의 인덱스가 되고 일반일땐 GMOBJECT의 인덱스
	GMOBJECT*	m_pParent;					// 부모, 바이페드를 부모로 가진 링크메시는 이걸안쓴다.
	MATRIX		m_mLocalTM;			// read
	MATRIX		m_mInverseTM;			// read 미리 변환된 인버스TM 

	Vec3		*m_pVertexList;			// read 각 관절에 대해 Local좌표가 들어감
	int				*m_pPhysiqueVertex;		// read 피지크, 각 버텍스 피지x크들이 참조하는 본 ID
	MATERIAL_BLOCK	*m_pMtrlBlk;			// read
//	LPDIRECT3DTEXTURE9	*m_pMtrlBlkTexture;		// 매터리얼 블럭내 텍스쳐포인터
	GLuint	*m_pGL_MtrlBlkTexture;		// 매터리얼 블럭내 텍스쳐포인터
	
	TM_ANIMATION	*m_pFrame;				// 애니메이션이 있으면 프레임수만큼 있다

	Vec3		m_vBBMin, m_vBBMax;			// read bound box min, max
	Vec3		m_vBBVList[8];				// read bound box vlist

	int				m_bMaterial;				// MES파일에 매터리얼이 있었는지?.  저장을 위해서 백업 받아둠
	MATERIAL		m_MaterialAry[16];			// MES에 저장되어 있던 Material정보.  Save때나 다시 매터리얼을 셋팅 해야할 필요가 있을때 사용

	// void*의 VB를 쓸땐 항상 캐스팅을 할것.
	void			*m_pVB;			// Indexed용 버텍스 버퍼 - 버텍스 리스트와 다른점은 좌표는 같으나 TU,TV가 다른 버텍스들을 따로 빼낸것이다. matIdx는 가지고 있지 않다.

	WORD			*m_pIB;			// 인덱스 버퍼, m_pIIndexBuffer의 복사본.
	WORD			*m_pIIB;		// m_pVB가 어느 m_pVertexList에 대응되는지에 대한 인덱스, m_nMaxIVB, 스키닝에 쓰임.  버텍스 버퍼와 버텍스리스트의 크기가 다르기때문에 이런게 필요하다.
	int				m_nMaxVB;
	int				m_nMaxIB;

	// 스킨형 오브젝트일때는 VB는 NULL이다. 외부지정VB를 따로 써야 한다.
	GLuint		m_nGL_VB;		// Indexed용 VB
//	GLuint		m_nGL_IB;		// Indexed용 IB
//	LPDIRECT3DVERTEXBUFFER9		m_pd3d_VB;		// Indexed용 VB
//	LPDIRECT3DINDEXBUFFER9		m_pd3d_IB;		// Indexed용 IB	

};

//
// 3D 오브젝트
// 인간, 몬스터, 나무, 회전관람차 등.....
// Object3D는 GMOBJECT들로써 구성된다.  클래스내에선 편의상 Object로 부른다.
// o3d파일 이다.
// 3D 오브젝트 데이타의 최소단위.
// 
//
/*
여러가지 유형
* 스킨,스킨,스킨,일반,일반,일반
. 스킨들은 한셋트의 본에만 영향을 받음.
. 일반들은 각자 최종업데이트용 매트릭스를 가짐.

스킨
. 본에만 영향 받음.

일반,일반,일반,일반,일반
. 각자 매트릭스를 가짐.

*/

#define		MAX_GROUP		3

#define		LOD_HIGH	0
#define		LOD_MEDIUM	1
#define		LOD_LOW		2

struct LOD_GROUP
{
	int			m_nMaxObject;
	GMOBJECT*	m_pObject;		// sizeof(GMOBJECT) * m_nMaxObject
	// 계층구조 애니메이션후 갱신된 매트릭스들이 담길 임시버퍼 m_nMaxObject만큼 생성. 
	// 만약 스킨만 있는 오브젝트라면 이것은 생성되지 않는다.
	MATRIX	*_mUpdate;
};

class CObject3D
{
private:
	LOD_GROUP		*m_pGroup;		// 현재 선택된 LOD그룹
	LOD_GROUP		m_Group[ MAX_GROUP ];		// LOD 그룹
	MATRIX	*m_pmExternBone;	// 외부에서 지정하는 본 포인터.
	MATRIX	*m_pmExternBoneInv;	// 외부에서 지정하는 본 Inv 포인터.
	
	int			m_nMaxBone;			// 이오브젝트가 사용하는 뼈대의 개수. 외부에서 뼈대를 지정할때도 이 개수와 맞는지 확인하자.
	MATRIX	*m_pBaseBone;		// o3d내에 들어있는 디폴트 뼈대 셋트. 이미계산 끝난 matWorld리스트가 들어있다. 뼈대를 쓰지 않는다면 이것은 NULL이다.
	MATRIX	*m_pBaseBoneInv;	// BaseBone의 Inverse. 이것은 destroy하지 앟는다.
	CMotion		*m_pMotion;			// o3d에 본애니메이션이 포함되어 있다면 할당됨.
	
	int		m_nMaxEvent;
	Vec3	m_vEvent[MAX_MDL_EVENT];

	int		m_nTextureEx;			// 몇번 텍스쳐 세트로 렌더 해야하는가.
	MOTION_ATTR		*m_pAttr;			// 프레임크기만큼 할당.  각 프레임속성.
	
public:
//	LPDIRECT3DDEVICE9		m_pd3dDevice;
	int		m_bSendVS;
	int		m_bLOD;						// LOD데이타가 있는가?
	int		m_nID;						// 오브젝트의 고유 아이디
	int		m_nHavePhysique;			// 피지크 오브젝트를 가지고 있는가.
	char	m_szFileName[64];			// 오브젝트 파일 명
	GMOBJECT	m_CollObject;	// 충돌용 메시
	
	// 검광지점은 인덱스로는 안되고 실제 버텍스 위치를 기록해야 할것이다.
	Vec3		m_vBBMin, m_vBBMax;			// read bound box min, max
	Vec3	m_vForce1, m_vForce2;			// 검광 시작점,끝점
	float	m_fScrlU, m_fScrlV;
	int		m_nMaxFace;					// 오브젝트들의 폴리곤 갯수 총합
	int		m_nNoTexture;				// 텍스쳐를 사용하지 않음.
	int		m_nNoEffect;				// 상태셋팅을 건드리지 않음.
	
	// 만약 자체 키프레임 애니메이션이 있을경우 사용된다.
//	double		m_dKPS;
	float		m_fPerSlerp;
	int			m_nMaxFrame;
	Vec3		m_vBBVList[8];				// read bound box vlist

	int		m_nUseCnt;		// 몇번 공유되어 사용되었는가.	

	FLOAT	m_fAmbient[3];
	
private:
	void	Init( void );
	void	Destroy( void );

	int		LoadGMObject( FILE *fp, GMOBJECT *pObject );
	int		LoadTMAni	( FILE *fp, GMOBJECT *pObject );
	int		SaveGMObject( FILE *fp, GMOBJECT *pObject );
	int		SaveTMAni	( FILE *fp, GMOBJECT *pObject );

public:
	
	CObject3D();
	~CObject3D();

	GMOBJECT*	GetGMOBJECT( int nIdx = 0 ) { return &m_pGroup->m_pObject[ nIdx ]; }
	int		GetMaxObject( void ) { return m_pGroup->m_nMaxObject; }
	int		GetMaxFrame( void ) { return m_nMaxFrame; }
	int		GetMaxBone( void )	{ return m_nMaxBone; }
	MATRIX *GetBaseBone( void ) { return m_pBaseBone; }
	void	SetExternBone( MATRIX *pmBone, MATRIX *pmBoneInv ) { if(m_pBaseBone == NULL) { m_pmExternBone = pmBone; m_pmExternBoneInv = pmBoneInv; } }	// 스킨만이 m_pBaseBone을 이미 할당하고 있다.
//	int		Find( int nID );
	void	SetTexture( LPCTSTR szTexture );		// 수동으로 텍스쳐를 지정함.
	void	SetTexture( GLuint nGLTexture );
	void	SetTextureEx( GMOBJECT *pObj, int nNumEx );
	void	SetTextureEx( int nNumEx )
	{
		for( int i = 0; i < m_pGroup->m_nMaxObject; i ++ )
			SetTextureEx( &(m_pGroup->m_pObject[i]), nNumEx );		
	}
	void	LoadTextureEx( int nNumEx, GMOBJECT *pObj, MATERIAL *pmMaterial[16] );
	void	ChangeTexture( LPCTSTR szSrc, LPCTSTR szDest );
	void	SetBone( MATRIX *pmBone ) { m_pmExternBone = pmBone; }
	int		GetMaxVertex( void );
	int		GetMaxFace( void );
	int		GetMaxMtrlBlk( void );
	BOOL	IsHaveCollObj( void ) { return (m_CollObject.m_Type != GMT_ERROR)? TRUE : FALSE; }
	BOOL	IsUseExternBone( void ) { if( m_nHavePhysique && m_pBaseBone == NULL ) return TRUE; else return FALSE; }	// 스킨오브젝트이고 자체 본을 가지고 있지 않다. 이것은 외장본을 사용하는 스킨이다
	void	SetLOD( int nLevel ) { 
#ifdef __XDEBUG
		if( nLevel < 0 || nLevel >= MAX_GROUP )
		{
			XERROR( "SetLOD : lv=%d, %s", nLevel, m_szFileName );
			nLevel = 0;
		}
#endif		
		if( m_bLOD == 0 ) nLevel = 0;	// LOD데이타가 없다면 항상 High레벨로만..
		m_pGroup = &m_Group[nLevel]; 
	}	// 0:high,  1:medium,   2:low
	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pAttr + nNumFrm; }	
	DWORD	GetAttr( int nNumFrm ) 
	{ 
		if( m_pAttr == NULL )	return 0;
		return m_pAttr[ nNumFrm ].m_dwAttr; 
	}
	DWORD	IsAttrHit( float fNumFrm ) 
	{ 
		if( m_pAttr == NULL )	return 0;
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_HIT )
			if( pAttr->m_fFrame == fNumFrm )		// 속성은 8.0에 있는데 fNumFrm==8.5 로 들어왔을때 정확하게 비교하기 위해 필요.
				return pAttr->m_dwAttr;
			return 0;
	}
	MOTION_ATTR *IsAttrSound( float fNumFrm )
	{ 
		if( m_pAttr == NULL )	return 0;
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_SOUND )
			if( pAttr->m_fFrame == fNumFrm )
				return pAttr;
			return NULL;
	}
	DWORD	IsAttrQuake( float fNumFrm ) 
	{ 
		if( m_pAttr == NULL )	return 0;
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_QUAKE )
			if( pAttr->m_fFrame == fNumFrm )		// 속성은 8.0에 있는데 fNumFrm==8.5 로 들어왔을때 정확하게 비교하기 위해 필요.
				return pAttr->m_dwAttr;
			return 0;
	}
	
	
	xRESULT	CreateDeviceBuffer( GMOBJECT *pObject, GLuint *pnGL_VB );
	xRESULT SendVertexBuffer( GMOBJECT *pObj, GLuint nGL_VB );
	xRESULT SendIndexBuffer( MATERIAL_BLOCK *pBlock );
	int		LoadObject( LPCTSTR szFileName );
static 	BOOL IsAnimateFile( LPCTSTR szFileName );		// szFileName이 애니메이션이 있냐?
	int		SaveObject( LPCTSTR szFileName );
	void	ClearNormal( void );

	// 충돌/교차 루틴들.
	Vec3 *IntersecRayVertex( Vec3 *pOut, const Vec3 &vRayOrig, const Vec3 &vRayDir );
	int		SlideVectorXZ( Vec3 *pOut, const Vec3 &vPos, const Vec3 &vEnd , const MATRIX &mWorld, BOOL bCollObj = TRUE );
	int		SlideVectorXZ2( Vec3 *pOut, Vec3 *pIntersect, const Vec3 &vPos, const Vec3 &vEnd , const MATRIX &mWorld, BOOL bCollObj = TRUE);
	int		SlideVectorUnder( Vec3 *pOut, const Vec3 &vPos, const Vec3 &vEnd , const MATRIX &mWorld, Vec3 *pIntersect );
	void	FindTouchTriLine( Vec3 **pTriOut, const Vec3 &vPos, const Vec3 &vEnd , const MATRIX &mWorld, FLOAT *pDist, BOOL bCollObj = TRUE );
	Vec3 *IntersectRayTri( const Vec3 &vRayOrig, const Vec3 &vRayDir, const MATRIX &mWorld, Vec3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE );
	BOOL	IsTouchOBB_Line( const Vec3 &vStart, const Vec3 &vEnd, const MATRIX &mWorld, Vec3* pvIntersect );

	void	Skining( GMOBJECT *pObj, const MATRIX *mBones );
	xRESULT	SetVertexBuffer( GMOBJECT *pObj );
	void	Animate( FLOAT fFrameCurrent, int nNextFrame );
	void    SetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor );
	void    ResetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor );
	void	SetShader( const MATRIX *mWorld );
	void	Render( GLuint *pnGL_VB, FLOAT fFrameCurrent, int nNextFrame, const MATRIX *mWorld, int nEffect = 0, DWORD dwBlendFactor = 0xff000000 );
	void	RenderSkin( GLuint nGL_VB, GMOBJECT *pObj, const MATRIX *mWorld, int nEffect, DWORD dwBlendFactor = 0xff000000 );
	void	RenderNormal( GMOBJECT *pObj, const MATRIX *mWorld, int nEffect, int nBlendFactor = 255 );
	
	void	RenderBB( const MATRIX *mWorld );
	
//	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice;  return S_OK; }
	xRESULT RestoreDeviceObjects( GLuint *pnGL_VB );
//	HRESULT InvalidateDeviceObjects();
	xRESULT DeleteDeviceObjects();
};

#define		MAX_OBJECT3D			2048
#define		MAX_OBJECT3D_CACHE		32

class	CObject3DMng
{
private:
	int			m_nCachePos;
	int			m_nSize;
	int			m_nMax;

	DWORD		m_tmTimer;

	CObject3D		*m_pCache[ MAX_OBJECT3D_CACHE ];				// 메쉬 캐쉬
	CObject3D		*m_pObject3DAry[ MAX_OBJECT3D ];						// 메모리상에 일괄적으로 저장되는 메쉬 데이타

	void	Init( void );

public:
	void	Destroy( void );
	xRESULT DeleteDeviceObjects();
//	xRESULT InvalidateDeviceObjects();
		
	CObject3DMng();
	~CObject3DMng();

	int			DeleteObject3D( CObject3D *pObject3D );
	CObject3D	*LoadObject3D( LPCTSTR szFileName );
	void	Process( void );

	//HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice; }

};

extern CObject3DMng		g_Object3DMng;
extern MATRIX	g_mReflect;
/*
HRESULT		CreateShadowVS( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
HRESULT		CreateSkinningVS( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
void		DeleteVertexShader( LPDIRECT3DDEVICE9 pd3dDevice );


HRESULT		CreateShadowMask( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight );
void	RenderShadowMask( LPDIRECT3DDEVICE9 pd3dDevice );
void	DeleteShadowMask( void );
*/
void	SetTransformViewProj( const MATRIX &mViewProj );

Vec3 *GetLastPickTri( void );		// 마지막으로 피킹한 삼각형버텍스의 시작포인터.


#endif // 0
#endif // OBJECT3D
