#ifndef		__MODELOBJECT_H__
#define		__MODELOBJECT_H__

#include "Model.h"
#include "Object3D.h"
#include "Bone.h"
//#include "xuzhu.h"
#include "xUtil3D.h"
#include "ModelGlobal.h"
#include "xColor.h"
 
#define		MAX_SF_SLERP		3		// 키와키 사이를 몇단계로 보간할 것인가
#define		MAX_SF_SWDFORCE		512
#define		MAX_SF_SPLINE		4096

struct FVF_SWDFORCE
{
    Vec3 position;	// The 3D position for the vertex
    XCOLOR		color;		// The vertex color
};
#define	D3DFVF_SWDFORCE			(D3DFVF_XYZ | D3DFVF_DIFFUSE)

class CSwordForce
{
public:
	int		m_nMaxVertex;
	int		m_nMaxSpline;
	int		m_nMaxDraw;			// 스플라인 리스트중에 그릴 리스트
	XCOLOR	m_dwColor;			// 검광색.
	FVF_SWDFORCE	m_aList[ MAX_SF_SWDFORCE ];
	FVF_SWDFORCE	m_aList2[ MAX_SF_SWDFORCE ];
	FVF_SWDFORCE	m_aSpline[ MAX_SF_SPLINE ];

	CSwordForce() { Init(); }
	~CSwordForce() { Destroy(); }

	void	Init( void )
	{
		m_nMaxVertex = 0;
		m_nMaxSpline = 0;
		m_nMaxDraw = 0;
		m_dwColor = XCOLOR_ARGB( 255, 120, 120, 230 );
		memset( m_aList, 0, sizeof(FVF_SWDFORCE) * MAX_SF_SWDFORCE );
		memset( m_aSpline, 0, sizeof(FVF_SWDFORCE) * MAX_SF_SPLINE );
	}
	void	Destroy( void )
	{
		Init();
	}

	void	Clear( void )
	{
		m_nMaxVertex = 0;
		m_nMaxSpline = 0;
		m_nMaxDraw = 0;
	}
	BOOL	IsEmpty( void ) { return (m_nMaxSpline == 0) ? TRUE : FALSE; }

	void	Add( Vec3 v1, Vec3 v2 );
	void	Process( void );
	void	Draw( const MATRIX *mWorld );
	void	MakeSpline( void );

};

//
// 객체를 구성하는 o3d들.  Element라 칭하자.
//
struct O3D_ELEMENT
{
	int				m_nParentIdx;
	CObject3D		*m_pObject3D;
//	Vec3		*m_pVertexListWorld;	// r/w 영향받는 본에 의해 실시간으로 월드로 변환될 벡터(바이페드에만 사용)
//	MATRIX		*m_mUpdate;				// 각 오브젝트의 애니메이션 계산후 변환된 매트릭스 - 계층 개수만큼 생성됨 단일오브젝이면 디폴트로 LocalTM을 카피시킴.
	MATRIX		m_mLocalTM;				// m_pParent로부터의 LocalTM. 부모가 임의로 바뀔수 있기땜에 따로 가짐.
	MATRIX		m_mLocalRH;
	MATRIX		m_mLocalLH;
	MATRIX		m_mLocalShield;
	MATRIX		m_mLocalKnuckle;
	//	CUSTOMVERTEX	*m_pVB;					// indexed용 VertexBuffer 스킨은 버텍스버퍼가 계속 갱신되어야 하므로 직접 가지고 있는다.
	GLuint		*m_pnGL_VB;	// 버퍼 풀 - 지울땐 이것만 날리면 된다 .
	GLuint		*m_pnGL_VB1;	// 스킨오브젝트를 위한 독자적인 버텍스버퍼 m_pObject3D->m_nMaxObject만큼 생성된다. - 직접 지워선 안된다.
	GLuint		*m_pnGL_VB2;	// LOD2
	GLuint		*m_pnGL_VB3;	// LOD3
	GLuint		*m_pnGL_VBSel;	// 현재 선택된 버퍼
	//	LPDIRECT3DINDEXBUFFER8		m_nGL_IB;	// AGP에 생성되는 인덱스버퍼(역시 위와 같다)
	GLuint      m_nGL_TextureEx;	// 추가 텍스쳐.(멀티텍스쳐로 옷을 덧입힐때 쓴다.) 텍스쳐스테이지1번에 들어간다.
	MATERIAL		m_Mtrl;					// mes에 포함된 텍스쳐이외에 텍스쳐를 따로 쓸때.  여기에 값이 없다면 디폴트것을 쓴다.
	int		m_nEffect;						// 각 파츠의 외부지정 이펙트 설정.
};

#define		MAX_ELEMENT		16

// 애니메이션 개체. 게임내 오브젝트의 최소단위.
// 모션이 여기에 포함되어 있기때문에 바이페드든 일반이든 애니메이션을 시킨다면
// 반드시 이것으로 생성시켜서 쓸것.
// 독자적인 월드좌표를 가진다.
class CModelObject : public CModel
{
	BOOL	m_bMotionBlending;		// 모션 블렌딩 플래그
	float	m_fBlendWeight;			// 모션트랜지션의 가중치 0.0에서 1.0까지 되면 끝난다.
//	MATRIX	*m_pBaseBone;		// 본이 있는 오브젝을 읽었을때 그 본의 포인터를 받아둔다. 참조용.	X0617들어가면서 빠짐,
	MATRIX	*m_pBaseBoneInv;	// InverseTM - destroy하지 말것
//	int		m_bSendVS;
	BOOL	m_bCollObj;				// 충돌메시를 포함하고 있는가?
	GLuint	m_nGL_CloakTexture;		// 외부 지정 망토 텍스쳐 

public:
	int			m_nCnt;

	// 객체를 구성하는 o3d들.  Element라 칭하자.
	O3D_ELEMENT	m_Element[ MAX_ELEMENT ];
	
	// Bone / animation
	// 원래 Motion은 Element안에 있어야 맞는것이지만 문제를 간략화 하기 위해서 
	// CModelObject로 빼냈다.  1뼈대 n메쉬 구조가 더 이해하기가 쉽기 때문이다.
	CBones	*m_pBone;			// 뼈대 클래스 - 이것을 직접 파괴시키면 안된다.
	CMotion	*m_pMotionOld;		// 이전 모션
	CMotion	*m_pMotion;			// 현재 로딩된 모션 포인터 - 직접 파괴시키면 안된다.
	CSwordForce	*m_pForce;		// 검광
	CSwordForce	*m_pForce2;		// 검광-왼손
	int		m_nFrameOld;		// 이전동작의 프레임번호
//	int		m_nMaxBone;			// 뼈대개수.	X0617들어가면서 빠짐,
	MATRIX	*m_mUpdateBone;		// 뼈대의 애니메이션 계산후 변환된 매트릭스 - 뼈대 갯수만큼 생성됨 - 뼈대가 없는 오브젝트는 이것을 사용하지 않음.
	Vec3	m_vForce1, m_vForce2;

//	BOOL		m_bLocal;			// 로컬 애니메이션 모드
//	Vec3	m_vLastKeyPos;		// 마지막으로 계산됐던 키포지션값
//	Vec3	m_vLastDelta;	
//	Vec3	m_vLastPath;		// path처리에 사용. 이전에 읽었던 m_pPath->m_vPos값.
	
	// 바운딩 박스 관련....
//	float	m_fBBWidth, m_fBBHeight, m_fBBDepth;		// bound box size(사용하지 말것)

private:
	void Init( int nParts );
	void Init( void );
	void Destroy( void );
	BOOL Destroy( int nParts );

public:
	CModelObject();
	~CModelObject();

	void CreateForce( int nParts );		// 검광클래스 생성

	int		GetRHandIdx( void ) { return m_pBone->m_nRHandIdx; }
	int		GetLHandIdx( void ) { return m_pBone->m_nLHandIdx; }
	int		GetRArmIdx( void ) { return m_pBone->m_nRArmIdx; }
	int		GetLArmIdx( void ) { return m_pBone->m_nLArmIdx; }
//	void	ResetSpeed( void ) { m_fKeySpeed = (float)m_pMotion->m_dKps; }
	O3D_ELEMENT	*GetParts( int nParts ) { return &m_Element[ nParts ]; }
	CObject3D	*GetObject3D( void ) { return m_Element[0].m_pObject3D; }		// 이것은 단일메쉬에만 쓴다.

	CObject3D	*GetObject3D( int nNum ) { return m_Element[nNum].m_pObject3D; }		// 이것은 상이만 쓴다..ㅡㅡ^;
	void	SetExtTexture( GLuint nGL_Texture ) { m_nGL_CloakTexture = nGL_Texture; }
	void	GetForcePos( Vec3 *vOut, int nIdx, int nParts, const MATRIX &mWorld );
	void	GetHandPos( Vec3 *vOut, int nParts, const MATRIX &mWorld );
		
	int		MovePart( int nDstPart, int nSrcPart );
	void	SetMotionBlending( BOOL bFlag = TRUE );
	void	TakeOffParts( int nParts );		// 장착해제할때 쓴다.
	void	SetGroup( int nNum ) 
	{
#ifdef __XDEBUG
		if( nNum < 0 || nNum >= MAX_GROUP )
		{
			XERROR( "SetGroup : lv=%d", nNum );
			nNum = 0;
		}
#endif		
		for( int i = 0; i < MAX_ELEMENT; i ++ )
		{
			if( m_Element[i].m_pObject3D )	
			{
				if( m_Element[i].m_pObject3D->m_bLOD == 0 )		
					m_Element[i].m_pObject3D->SetLOD( 0 );		// LOD데이타가 없다면 항상 High레벨로만..
				else
				{
					m_Element[i].m_pObject3D->SetLOD( nNum );
					switch( nNum )
					{
					case 0:		m_Element[i].m_pnGL_VBSel = m_Element[i].m_pnGL_VB1;	break;		// lod가 선택될때마다 버퍼 포인터를 스위칭 해줌.
					case 1:		m_Element[i].m_pnGL_VBSel = m_Element[i].m_pnGL_VB2;	break;
					case 2:		m_Element[i].m_pnGL_VBSel = m_Element[i].m_pnGL_VB3;	break;
					}
				}
				
			}
		}
	}		// Set LOD Group
	// 각 파츠의 렌더링 이펙트를 외부에서 지정할때 쓴다.	XE_??? 시리즈를 조합해서 쓰자.
	void	SetEffect( int nParts, int nEffect ) { m_Element[nParts].m_nEffect |= nEffect; } 
	void	SetEffectOff( int nParts, int nEffect ) { m_Element[nParts].m_nEffect &= (~nEffect); } 
	void	SetTextureEx( int nNumEx )		// 몇번 텍스쳐 셋을 쓰느냐
	{
		m_Element[0].m_pObject3D->SetTextureEx( nNumEx );		// 몬스터는 [0]하나만 쓴다고 가정하고 하자.
	}

	
	BOOL	IsLoadAttr( void ) { if(m_pMotion) return TRUE; else return FALSE; }
	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pMotion->GetMotionAttr(nNumFrm); }	
	DWORD	IsAttrHit( void ) 
	{ 
		if( m_pMotion )
			return m_pMotion->IsAttrHit( m_fFrameOld, m_fFrameCurrent ); 
		else
			return GetObject3D()->IsAttrHit( m_fFrameCurrent );
	}	// 현재 프레임에 타격속성이 있는가?
	DWORD	IsAttrHit( float fNumFrm ) { return m_pMotion->IsAttrHit( m_fFrameOld, fNumFrm ); }	// nNumFrm프레임에 타격속성이 있는가?
	MOTION_ATTR *IsAttrSound( void ) 
	{ 
		if( m_pMotion == NULL )
		{
			XLOG( "IsAttrSound : pMotion==NULL %d %d", m_fFrameOld, m_fFrameCurrent );
		}
		return m_pMotion->IsAttrSound( m_fFrameOld, m_fFrameCurrent ); 
	}	// 현재 프레임에 사운드속성이 있는가?
	MOTION_ATTR *IsAttrSound( float fNumFrm ) { return m_pMotion->IsAttrSound( m_fFrameOld, fNumFrm ); }	// nNumFrm프레임에 사운드속성이 있는가?
	DWORD	IsAttrQuake( float fNumFrm ) { return m_pMotion->IsAttrQuake( m_fFrameOld, fNumFrm ); }
	DWORD	IsAttrQuake( void ) { return m_pMotion->IsAttrQuake( m_fFrameOld, m_fFrameCurrent ); }
	void	SetAttr( float fNumFrm, DWORD dwAttr ) { m_pMotion->SetAttr( fNumFrm, dwAttr ); }
	void	UpdateMatrixBone( void )
	{
		if( m_pMotion )		// 현재 프레임의 모션에 대한 매트릭스를 갱신시킴.
			m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );	
	}
	void	GetEventPos( Vec3 *pOut, int nIdx = 0 )		// 업데이트된 본의 기준으로 계산된 이벤트좌표.
	{ 
		if( m_pBone == NULL )	return;
		if( m_mUpdateBone == NULL )	return;
		MatrixVec3Multiply( *pOut, m_pBone->m_vEvent[nIdx], m_mUpdateBone[ m_pBone->m_nEventParentIdx[nIdx] ] );
		// 곱한 결과인 pOut은 로컬기준이므로 최종 m_mWorld와 곱해줘야 한다.
		// vFinal = pOut * m_mWorld;
	}
	MATRIX *GetMatrixBone( int nBoneIdx )
	{
		if( m_pBone == NULL )	return NULL;
		if( m_mUpdateBone == NULL )	return NULL;
		return &m_mUpdateBone[ nBoneIdx ];
	}
	BOOL	IsEmptyElement( void )
	{
		for( int i = 0; i < MAX_ELEMENT; i ++ )
			if( m_Element[i].m_pObject3D )	return FALSE;
		return TRUE;
	}
	Vec3 *GetPath( void ) { return ( m_pMotion ) ? m_pMotion->m_pPath : NULL; }
//	void GetPathDelta( Vec3 *pOut );		// // m_fFrameCurrent의 Path Delta값을 보간하여 리턴
	int		GetMaxObject( void );			// GMOBJECT 수
	int		GetMaxVertex( void );			// 총 버텍스 개수
	int		GetMaxFace( void );				// 총 면수
	int		GetMaxMtrlBlk( void );			// 총 매터리얼 블럭 수
	int		GetMaxBone( void ) { return ( m_pBone ) ? m_pBone->m_nMaxBone : 0; }
	char	*GetBonesName( char *pBuff ) { return NULL; }
	int		IsHaveCollObj( void ) { return m_bCollObj; }
	
	xRESULT	CreateDeviceBuffer( O3D_ELEMENT *pElem );
	int		LoadBone( LPCTSTR szFileName );
	int		LoadMotion( LPCTSTR szFileName );
	int		LoadElement( LPCTSTR szFileName, int nParts = 0 );
	void	SetParent( int nParts, int nBoneIdx );
	void	SetTextureMulti( LPCTSTR szBitmap, int nParts );
	void	ClearTextureEx( int nParts );
	void	ChangeTexture( int nParts, LPCTSTR szSrc, LPCTSTR szDest );
	
	BOOL	Render( const MATRIX *mWorld );
//	void	RenderEffect( const MATRIX *mWorld );
	BOOL	RenderBB( const MATRIX *mWorld );


	void	MakeSWDForce( int nParts, DWORD dwItemKind3, BOOL bSlow = FALSE, DWORD dwColor = XCOLOR_ARGB(255,120,120,230), float fSpeed = 1.0f );		// m_pMotion의 검광을 생성

	void	FrameMove( float fSpeed = 1.0f );

	// collision detect
	BOOL	Intersect( const Vec3 &vRayOrig, const Vec3 &vRayDir, const MATRIX &mWorld, Vec3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE  ) { if( IntersectRayTri( vRayOrig, vRayDir, mWorld, pvIntersect, pfDist, bColl ) ) return TRUE; else return FALSE; }
	Vec3 *IntersectRayVertex( Vec3 *pOut, const Vec3 &vRayOrig, const Vec3 &vRayDir );
	Vec3 *IntersectRayTri( const Vec3 &vRayOrig, const Vec3 &vRayDir, const MATRIX &mWorld, Vec3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE ) 
	{	
		O3D_ELEMENT *pElem = m_Element;
		Vec3 *pTri;
		for( int i = 0; i < MAX_ELEMENT; i ++ )
		{
			if( pElem->m_pObject3D )
			{
				if( m_pBone )
					pElem->m_pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 외장본이 있다면 그것을 넘겨준다.
				if( pTri = pElem->m_pObject3D->IntersectRayTri( vRayOrig, vRayDir, mWorld, pvIntersect, pfDist, bColl ) )
					return pTri;
			}
			pElem ++;
		}
		return NULL;
	}
	BOOL	IsTouchOBB_Line( const Vec3 &vStart, const Vec3 &vEnd, const MATRIX &mWorld, Vec3* pvIntersect, BOOL bNeedCollObject = TRUE )
	{ 
		CObject3D *pObj = m_Element[0].m_pObject3D;
		if( pObj->m_CollObject.m_Type == GMT_ERROR && bNeedCollObject )	// 충돌메시가 없으면 검사하지 않음.
			return FALSE;
		if( ::IsTouchOBB_Line( m_vMin, m_vMax, mWorld, vStart, vEnd, pvIntersect ) == TRUE )
			return TRUE;

		return FALSE;
//		return pObj->IsTouchOBB_Line( vStart, vEnd, mWorld, pvIntersect ); 
	}

	// virtual
	int LoadModel( LPCTSTR szFileName ) { return LoadElement( szFileName, 0 ); }
	
	// 디바이스 초기화 및 삭제 
	xRESULT RestoreDeviceObjects();
	xRESULT DeleteDeviceObjects();
};



#endif // MODELOBJECT
