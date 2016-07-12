//
//
//
//
//
#include <stdio.h>
#include "path.h"
#include "Material.h"
#include "ModelGlobal.h"
#include "ModelObject.h"

#define PARTS_HAIR			1
#define PARTS_CLOAK			2
#define PARTS_RWEAPON		6
#define PARTS_LWEAPON		7

char   g_szLastFile[512];

/////////////////////////////////////////////////////////////////////////////////////
////////////
////////////
////////////
////////////
////////////
/////////////////////////////////////////////////////////////////////////////////////
CModelObject::CModelObject()
{
	Init();
}

CModelObject::~CModelObject()
{
	Destroy();
}

void CModelObject::Init( void )
{
	int		i;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		Init( i );
	}

	m_pBone = NULL;
	m_pForce = m_pForce2 = NULL;
	m_pMotion = m_pMotionOld = NULL;
	m_mUpdateBone = NULL;
	m_nModelType = MODELTYPE_NONE;//BIPED_MESH;
	m_bMotionBlending = FALSE;
	m_fBlendWeight = 0;
	m_bCollObj = 0;
	m_pBaseBoneInv = NULL;

	m_vMin.x = m_vMin.y = m_vMin.z = 65535.0;
	m_vMax.x = m_vMax.y = m_vMax.z = -65535.0;
	m_vForce1.x = m_vForce1.y = m_vForce1.z = 0;
	m_vForce2.x = m_vForce2.y = m_vForce2.z = 0;

	m_nGL_CloakTexture = NULL;
	
}

void CModelObject::Init( int nParts )
{

	memset( &m_Element[nParts], 0, sizeof(O3D_ELEMENT) );
	m_Element[nParts].m_nParentIdx = -1;
	MatrixIdentity( m_Element[nParts].m_mLocalRH );
	MatrixIdentity( m_Element[nParts].m_mLocalLH );
	MatrixIdentity( m_Element[nParts].m_mLocalShield );
	MatrixIdentity( m_Element[nParts].m_mLocalKnuckle );
	MatrixIdentity( m_Element[nParts].m_mLocalTM );
}

//
void CModelObject::Destroy( void )
{
	int		i;

	// 메쉬 삭제
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		Destroy( i );
	}
	SAFE_DELETE_ARRAY( m_mUpdateBone );

	SAFE_DELETE( m_pForce );
	SAFE_DELETE( m_pForce2 );
	Init();
	// m_pBone를 직접 삭제하면 안된다.
	// m_pMotion을 직접 삭제하면 안된다.
	// CObject3D::m_pObject3D를 직접 삭제하면 안된다.  CObject3DMng에서 삭제해 준다.
}

//
BOOL CModelObject::Destroy( int nParts )
{
	O3D_ELEMENT	*pElem = &m_Element[ nParts ];
	int		i;

	if( pElem->m_pObject3D )
	{
		if( pElem->m_pnGL_VB )
		{
			pElem->m_pObject3D->SetLOD(0);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )		// 
				glDeleteTextures( 1, &pElem->m_pnGL_VB1[i] );
			pElem->m_pObject3D->SetLOD(1);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )
				glDeleteTextures( 1, &pElem->m_pnGL_VB2[i] );
			pElem->m_pObject3D->SetLOD(2);
			for( i = 0; i < pElem->m_pObject3D->GetMaxObject(); i ++ )
				glDeleteTextures( 1, &pElem->m_pnGL_VB3[i] );
			pElem->m_pObject3D->SetLOD(0);
			
			SAFE_DELETE_ARRAY( pElem->m_pnGL_VB );		// 전체 메모리 풀을 한꺼번에 날림.
		}
		g_Object3DMng.DeleteObject3D( pElem->m_pObject3D );	// pObject3D가 공유된게 아니면 여기서 날려 버림.
		Init( nParts );
		return TRUE;
	}
	Init( nParts );
	return FALSE;
}

// 검광클래스 생성
void	CModelObject::CreateForce( int nParts )
{
	if( nParts == PARTS_RWEAPON )
	{
		if( m_pForce )	return;		// 이미 할당되어 있으면 다시 할당하지 않음.
		m_pForce = new CSwordForce;
	} else
	if( nParts == PARTS_LWEAPON )
	{
		if( m_pForce2 )	return;		// 이미 할당되어 있으면 다시 할당하지 않음.
		m_pForce2 = new CSwordForce;
	} else
	{
//		ASSERT(0);
	}
}


void	CModelObject::TakeOffParts( int nParts )
{
#ifdef __XDEBUG
	if( nParts < 0 )
		XERROR( "CModelObject::TakeOffParts : nParts 에러 - nParts = %d", nParts );
//		XERROR( "CModelObject::TakeOffParts : nParts 에러 - nParts = %d", nParts );
#endif
	if( nParts >= MAX_ELEMENT )
		return;
	// 파츠를 제거했는데, 아래 부분을 안해주면 old 파츠에서 매모리 릭 발생 - vampyre
	O3D_ELEMENT	*pElem;
	pElem = &m_Element[ nParts ];
	if( pElem->m_pnGL_VB )
	{
		pElem->m_pObject3D->SetLOD(0);
		int j;
		for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )		// 
			glDeleteTextures( 1, &pElem->m_pnGL_VB1[j] );
		pElem->m_pObject3D->SetLOD(1);
		for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
			glDeleteTextures( 1, &pElem->m_pnGL_VB2[j] );
		pElem->m_pObject3D->SetLOD(2);
		for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
			glDeleteTextures( 1, &pElem->m_pnGL_VB3[j] );
		pElem->m_pObject3D->SetLOD(0);
		SAFE_DELETE_ARRAY( pElem->m_pnGL_VB );		// 전체 메모리 풀을 한꺼번에 날림.
	}
	Init( nParts );
}
int		CModelObject::GetMaxObject( void )
{
	int		i, nMax = 0;
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
		{
			nMax += m_Element[i].m_pObject3D->GetMaxObject();
		}
	}

	return nMax;
}

int		CModelObject::GetMaxVertex( void )
{
	int		i, nMax = 0;
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
		{
			nMax += m_Element[i].m_pObject3D->GetMaxVertex();
		}
	}

	return nMax;
}

int		CModelObject::GetMaxFace( void )
{
	int		i, nMax = 0;
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
		{
			nMax += m_Element[i].m_pObject3D->GetMaxFace();
		}
	}

	return nMax;
}

int		CModelObject::GetMaxMtrlBlk( void )
{
	int		i, nMax = 0;
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
		{
			nMax += m_Element[i].m_pObject3D->GetMaxMtrlBlk();
		}
	}

	return nMax;
}

//
//		LoadBone
//
int		CModelObject::LoadBone( LPCTSTR szFileName )
{
#ifdef __XDEBUG
	if( m_pBone )
		XLOG( "CModelObject::LoadBone ; 이미 본이 로딩되어 있다. 0x%08x", (int)m_pBone );
#endif
	// Bone 관리자를 통해 본을 로딩한후 그 포인터를 받아온다.
	m_pBone = g_BonesMng.LoadBone( szFileName );

	// 본을 못읽었다.
	if( m_pBone == NULL )
	{
		XERROR( "%s : 찾을 수 없음", szFileName );
		return FAIL;
	}

	m_bSkin = TRUE;		// 스키닝 오브젝트라는 플래그
#ifdef _DEBUG
	if( m_mUpdateBone )
		XLOG( "CModelObject::LoadBone : %s 이미 읽었는데 또 읽은것 같다.", szFileName );
#endif

	// 오브젝트의 계층구조가 애니메이션되면서 실시간으로 변환되는 매트릭스 배열
//	if( m_mUpdateBone == NULL )		// 모션을 재로딩을 할 수 있으므로 이미 할당 받았으면 다시 받지 않음.
	m_mUpdateBone = new MATRIX[ m_pBone->m_nMaxBone * 2 ];	// Inv랑 같이 쓰려고 * 2로 잡는다,.
	m_pBaseBoneInv = m_mUpdateBone + m_pBone->m_nMaxBone;
	
	int		i;
	for( i = 0; i < m_pBone->m_nMaxBone; i ++ )
	{
		m_mUpdateBone[i] = m_pBone->m_pBones[i].m_mTM;		// 기본셋은 미리 카피해둠.
		m_pBaseBoneInv[i] = m_pBone->m_pBones[i].m_mInverseTM;	// Inv도 미리 받아둠.
	}
	return SUCCESS;
}


//
//	LoadMotion
// 오브젝트를 로딩하지않고도 독립적으로 동작해야한다.
// 
int		CModelObject::LoadMotion( LPCTSTR szFileName )
{
	int		i;
	// 본이 아직 할당 되어 있지 않으면 애니를 읽을수 없다.
	if( m_pBone == NULL )		
	{
#ifdef	__XDEBUG
		XERROR( "%s : 본을 로딩하지 않고 모션을 읽음", szFileName );
#endif
		return FAIL;
	}
	m_pMotionOld = m_pMotion;		// 예전 모션받아둠.
	m_nFrameOld = (int)m_fFrameCurrent;
	// Motion관리자를 통해 동작을 로딩한후 그 포인터를 받아온다.
	m_pMotion = g_MotionMng.LoadMotion( szFileName );
	if( m_pMotion == NULL )		// 모션을 못읽었다.
	{
		return FAIL;
	}
	if( m_pMotionOld == NULL )
		m_pMotionOld = m_pMotion;		// 이전프레임이 없으면 현재 프레임을 Old로 쓴다.  NULL이 되는상황이 없게 할것.

	InitAnimate();

	m_nFrameMax = m_pMotion->GetMaxFrame();
	m_fPerSlerp = (float)((int)(m_pMotion->m_fPerSlerp * 1000.0f)) / 1000.0f;		// 소숫점 3째 이하에서 오차가 생길수 있으므로 3째까지 에서 잘라버린다,.
//	m_vLastKeyPos = Vec3( 65535.0f, 65535.0f, 65535.0f );
//	m_vLastPath.x = m_vLastPath.y = m_vLastPath.z = 0;

  #ifdef __XDEBUG
	if( m_pMotionOld->m_nMaxBone != m_pMotion->m_nMaxBone )		// 이전에 읽은 모션의 본개수와 다르면 에러냄.
	{
		XERROR( "CModelObject::LoadElement - %s가 사용하는 뼈대개수(%d)가 이전에 읽은 뼈대개수(%d)와 다르다.", szFileName, m_pMotion->m_nMaxBone, m_pMotionOld->m_nMaxBone );
	}
  #endif

//	if( m_mUpdateBone == NULL )		// 모션을 재로딩을 할 수 있으므로 이미 할당 받았으면 다시 받지 않음.
//		m_mUpdateBone = new MATRIX[ m_pMotion->m_nMaxBone ];
	
	// 이미 스킨 오브젝트가 로딩되어 있는 상태면 m_mUpdateBone을 그 오브젝트에 등록시킴.
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( m_Element[i].m_pObject3D )
			m_Element[i].m_pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 함수내부에서 스킨인넘만 등록시킨다.
	}
	return SUCCESS;
}

// 파츠를 옮김.
// 왼손에 든걸 오른손에 옮기는등...
int	CModelObject::MovePart( int nDstPart, int nSrcPart )
{
	if( m_Element[ nDstPart ].m_pObject3D )
		XLOG( "CModelObject::MovePart : m_Element[%d].m_pObject3D == 0x%08x", nDstPart, (int)m_Element[ nDstPart ].m_pObject3D );
	memcpy( &m_Element[ nDstPart ], &m_Element[ nSrcPart ], sizeof(O3D_ELEMENT) );	// 카피하고
	memset( &m_Element[ nSrcPart ], 0, sizeof(O3D_ELEMENT) );		// 소스측은 클리어.
	return 1;
}

//
//	오브젝트 로딩.  모션을 읽지 않은 상태에서도 독립적으로 동작한다.
//
int		CModelObject::LoadElement( LPCTSTR szFileName, int nParts )
{
	CObject3D	*pObject3D;
	O3D_ELEMENT	*pElem;
#ifdef __XDEBUG
	if( nParts < 0 )
	{
		XERROR( "CModelObject::LoadElement : nParts 에러 - nParts = %d, %s", nParts, szFileName );
		return FAIL;
	}
	if( nParts >= MAX_ELEMENT )
		return FAIL;
//		XERROR( "CModelObject::LoadElement : nParts 에러 - nParts = %d, %s", nParts, szFileName );
#endif

	Destroy( nParts );

		
	pObject3D = g_Object3DMng.LoadObject3D( szFileName );
	//TRACE( "new Object 3D%p\n", pObject3D);
	
	if( pObject3D == NULL )
	{
		XERROR( "%s : 찾을 수 없음", MakePath( DIR_MODEL, szFileName ) );
		return FAIL;
	}

	// 스킨파츠의 경우 외부본을 읽었는지 검사.
	if( pObject3D->IsUseExternBone() )		// 외장본을 사용하는 오브젝이다.
	{
		if( m_pBone == NULL )			// 본을 아직 안읽은 상태다...
		{
#ifdef	__XDEBUG
			XERROR( "%s : 본을 로딩하지 않고 메쉬를 읽음", szFileName );		// 에러...
#endif
			return FAIL;
		}
	}
	
	pElem = &m_Element[ nParts ];
	pElem->m_pObject3D = pObject3D;
	

	memset( &g_szLastFile, 0, sizeof(char)*512 );
	strcpy( g_szLastFile, szFileName );

	if( pObject3D->GetGMOBJECT() == NULL )
	{
		XERROR( "CModelObject::LoadElement에서 튕김 : 읽은 파일명 %s", MakePath( DIR_MODEL, szFileName ) );
		return FAIL;
	}
	
	if( pObject3D->GetGMOBJECT()->m_bOpacity )
		m_dwRenderEffect = REFF_ALPHA;
//	if( pObject3D->GetGMOBJECT()->m_bReflection )
//		m_dwRenderEffect = REFF_REFLECT;
	m_bCollObj = pObject3D->IsHaveCollObj();
	if( pObject3D->m_nHavePhysique )	m_bSkin = TRUE;

	if( m_nFrameMax > 0 )	// 이미 모션이나 오브젝을 읽어서 maxframe이 있을때 이번에 읽은 오브젝틔 maxframe하고 비교. 틀리면 에러.
	{
		if( pObject3D->GetMaxFrame() )
			if( m_nFrameMax != pObject3D->GetMaxFrame() )
			{
				XERROR( "CModelObject::LoadElement - %s의 MaxFrame(%d)과 이전에 읽은 MaxFrame(%d)이 다르다.", szFileName, pObject3D->GetMaxFrame(), m_nFrameMax );
			}
	}
	if( m_nFrameMax == 0 )	// maxframe이 세팅된적이 없을때만 최초 세팅함.
		m_nFrameMax = pObject3D->GetMaxFrame();

	// 애니메이션 속도
	m_fPerSlerp = (float)((int)(pObject3D->m_fPerSlerp * 1000.0f)) / 1000.0f;		// 소숫점 3째 이하에서 오차가 생길수 있으므로 3째까지 에서 잘라버린다,.


	// 이미 본이 로딩되어 있는 상태면 m_pUpdateBone을 오브젝트의 외부뼈대로 등록함.
	if( m_pBone )
		pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );


	// 새로 읽은 Mesh의  BB를 기준으로 CModelObject의 BB를 갱신
	// 이부분계산은 현재 정확하지 않다.  Min,Max모두 Local기준인데다가
	// 사람이 창을 쥐고 있을경우 창까지 바운딩 박스에 포함되고 있다.
	if( pObject3D->m_vBBMin.x < m_vMin.x )	m_vMin.x = pObject3D->m_vBBMin.x;
	if( pObject3D->m_vBBMin.y < m_vMin.y )	m_vMin.y = pObject3D->m_vBBMin.y;
	if( pObject3D->m_vBBMin.z < m_vMin.z )	m_vMin.z = pObject3D->m_vBBMin.z;
	if( pObject3D->m_vBBMax.x > m_vMax.x )	m_vMax.x = pObject3D->m_vBBMax.x;
	if( pObject3D->m_vBBMax.y > m_vMax.y )	m_vMax.y = pObject3D->m_vBBMax.y;
	if( pObject3D->m_vBBMax.z > m_vMax.z )	m_vMax.z = pObject3D->m_vBBMax.z;
	SetBB( m_BB.m_vPos, m_vMin, m_vMax );


	// 읽어온 엘리먼트는 this의 뼈대에 부모지정이 안되어 있게 한다.
	// 만약 칼을 읽어왔다면 수동으로 SetParent()하도록 한다.
	pElem->m_nParentIdx = -1;
/*	
	// 읽은 오브젝트가 부모인덱스를 가진다면 그것을 지정함.
	// 일반적으로 오른손이 부모로 되어 있다.
	int nParentIdx = pObject3D->GetGMOBJECT()->m_nParentIdx;
	if( nParentIdx != -1 )
		SetParent( nParts, nParentIdx );
	else
		pElem->m_nParentIdx = -1;
*/
	if( pObject3D->m_nHavePhysique )	// 피지크를 가지고 있다면 스킨은 버텍스버퍼를 따로 가짐
	{
#ifdef __XDEBUG
		if( pElem->m_pnGL_VB )
			XLOG( "CModelObject::LoadElement: %s pElem->m_pnGL_VB가 남아 있다", szFileName );
#endif
		int nTotal = 0;
		pObject3D->SetLOD(0);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(0);
		
		if( nTotal <= 0 )
			XERROR( "%s LoadElement : nTotal=%d", pObject3D->m_szFileName, nTotal );

		pElem->m_pnGL_VB = new GLuint[ nTotal ];
		memset( pElem->m_pnGL_VB, 0, sizeof(GLuint) * nTotal );
		GLuint *pnGL_VB = pElem->m_pnGL_VB;
		pObject3D->SetLOD(0);		pElem->m_pnGL_VB1 = pnGL_VB;		pnGL_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);		pElem->m_pnGL_VB2 = pnGL_VB;		pnGL_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);		pElem->m_pnGL_VB3 = pnGL_VB;		
		pObject3D->SetLOD(0);
		pElem->m_pnGL_VBSel = pElem->m_pnGL_VB1;
	}
	
	// 버텍스 버퍼 생성
//	if( pObject3D->GetGMOBJECT()->m_Type == GMT_SKIN )
//		pObject3D->RestoreDeviceObjects( D3DPOOL_SYSTEMMEM );		// 이걸 여기서 하면 안되는 이유가 있어서 안한건데 그 이유가 기억이 안난다 -_-;;;
//	else
		pObject3D->RestoreDeviceObjects( pElem->m_pnGL_VB );		// 이걸 여기서 하면 안되는 이유가 있어서 안한건데 그 이유가 기억이 안난다 -_-;;;
	return SUCCESS;
}

// SetParent( PARTS_LWEAPON, BONE_RHAND );
// nParts의 오브젝트의 부모를 nBoneIdx로 세팅
// 부모를 바꿀때 쓴다.
void	CModelObject::SetParent( int nParts, int nBoneIdx )
{
	O3D_ELEMENT	*pElem = &m_Element[ nParts ];

	pElem->m_nParentIdx = nBoneIdx;
	pElem->m_mLocalTM = pElem->m_pObject3D->GetGMOBJECT()->m_mLocalTM;

	// 만약 본의 개수가 바뀌었다면 BONE_RHAND의 인덱스도 바껴야 한다. 인간형모델은 본 개수를 통일 시켜야 한다.
	// 본 인덱스를 쉽게 알수 있는 방법은 CModelObject->m_pMotion->m_pBoneInfo[]에 0~32까지를 입력시켜보면서 Bip01 R Hand를 찾는다,
	if( nBoneIdx == GetRHandIdx() )	
		pElem->m_mLocalRH = m_pBone->m_mLocalRH;

	if( nBoneIdx == GetLHandIdx() )	
		pElem->m_mLocalLH = m_pBone->m_mLocalLH;
	
	if( nBoneIdx == GetLArmIdx() )	
		pElem->m_mLocalShield = m_pBone->m_mLocalShield;

	if( nBoneIdx == GetRArmIdx() )
		pElem->m_mLocalKnuckle = m_pBone->m_mLocalKnuckle;
}

//
// nParts에 텍스쳐파일 szBitmap을 멀티텍스쳐로 등록
// 기존 텍스쳐를 파괴할 필요는 없다.
// 반드시 LoadMesh()이후에 되어야 한다.
//
void	CModelObject::SetTextureMulti( LPCTSTR szBitmap, int nParts )
{
	D3DMATERIAL9	d3dmtrl;
	MATERIAL		*mtrl;

	mtrl = g_TextureMng.AddMaterial( &d3dmtrl, szBitmap );
	m_Element[ nParts ].m_nGL_TextureEx = mtrl->m_nGL_Texture;		// m_nGL_TextureEx에 포인터가 있으면 렌더링할때 멀티로 셋팅
}

//
void	CModelObject::ClearTextureEx( int nParts )
{
	m_Element[ nParts ].m_nGL_TextureEx = 0;
}

//
void	CModelObject::ChangeTexture( int nParts, LPCTSTR szSrc, LPCTSTR szDest )
{
	m_Element[ nParts ].m_pObject3D->ChangeTexture( szSrc, szDest );
}

//---------------------------------------------------------------------------------------------
//
// Render()를 부를때 내부에서 SetTransform()을 해주고 외부에선 매트릭스만 넘기는것을 원칙으로 하자.
// 넘겨주는 매트릭스는 오브젝트의 원점을 넘겨준다.
// 가령 인간의 경우는 센터부분이다.
//
FLOAT   g_fDiffuse[4];
FLOAT   g_fAmbient[4];
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
만약에 Render했는데 화면에 안나온다!!! 그러면!
static VECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
m_pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
SetTransformView( matView );
SetTransformProj( matProj );
SetDiffuse( 1.0, 1.0, 1.0 );
SetAmbient( 1.0, 1.0, 1.0 );

이걸 렌더 하기전에 불러줬는지 확인해라!!!!!!!!!!!!!!!!!!!!! 크아아악!
좆도 고생했네!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
 */
BOOL		CModelObject::Render( const MATRIX *mWorld )
{
	CObject3D	*pObject3D;
	O3D_ELEMENT	*pElem;
	int		i;
	int		nNextFrame;
	MATRIX m1;

#ifdef	_DEBUG
	if( m_nLoop == 0 )
		XERROR( "경고 : %s : CModelObject::m_nLoop가 지정되지 않았다.", m_pMotion->m_szName );
#endif

	if( m_pMotion )		// m_pMotion이 널이라면 m_mUpdateBone도 널이다.
		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
//		AnimateBone( NULL );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴

	if( m_pBone )		// m_pBone이 있다면 뼈대가 있다는 얘기. VS를 써야 한다.
	{
		MATRIX *pmBones;
		MATRIX m1;
		MATRIX *pmBonesInv = m_pBaseBoneInv ;
		pmBones = m_mUpdateBone;

		if( m_pBone->m_bSendVS )	// 뼈대개수가 MAX_VS_BONE이하라서 한번에 다 전송한다.
		{
			int		nMaxBone = m_pBone->m_nMaxBone;
			const MATRIX *pmView = GetTransformView();
#ifdef	__XDEBUG
			if( nMaxBone > MAX_VS_BONE )	
				XERROR( "CModelObject::Render : 뼈대개수가 최대치를 초과했다. %d", nMaxBone );
#endif
			for( i = 0; i < nMaxBone; i ++ )	// MAX_VS_BONE개 이하	
			{
				MatrixMultiply( m1, pmBonesInv[i], pmBones[i] );
				
				glCurrentPaletteMatrixOES( i );
				MatrixMultiply( m1, m1, *mWorld );
				MatrixMultiply( m1, m1, *pmView );
				glLoadMatrixf( m1.f );
				
			}
		}
/*		
		MATRIX	mView, mProj;
		MATRIX	mViewProj, mViewProjTranspose, mInvWorld;

		VECTOR4 vLight = s_vLight;
		VECTOR4 vLightPos = s_vLightPos;

		mViewProj = *mWorld * s_mView * s_mProj;
		
		MATRIXTranspose( &mViewProjTranspose, &mViewProj );
		MATRIXTranspose( &mWorldTranspose, mWorld );

		MATRIXInverse( &mInvWorld, NULL, mWorld );
		Vec4Transform( &vLight, &vLight, &mInvWorld );
		Vec4Normalize( &vLight, &vLight );
		Vec4Transform( &vLightPos, &vLightPos, &mInvWorld );
//		Vec4Transform( &vEyePos, &vEyePos, &mInvWorld );

		m_pd3dDevice->SetVertexShaderConstantF( 84, (float*)&mViewProjTranspose, 4 );
//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&mWorldTranspose, 4 );
//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&vEyePos,  1 );		// specular use
//		m_pd3dDevice->SetVertexShaderConstantF( 89, (float*)&fSpecular, 1 );	// specular use
//		m_pd3dDevice->SetVertexShaderConstantF( 90, (float*)&fLightCol, 1 );	// specular use
		m_pd3dDevice->SetVertexShaderConstantF( 91, (float*)&vLightPos, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 92, (float*)&vLight,   1 );
		m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
//		m_pd3dDevice->SetVertexShaderConstant( 95, &vConst, 1 );
 */
	}
/*
*/
	if( m_nNoEffect == 0 )
	{
		// 기본 설정 
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		glAlphaFunc( GL_GEQUAL, 0 );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL   );		
//		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
//		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		
	}
	DWORD dwBlendFactor = m_dwColor | ( m_dwBlendFactor << 24 );
	//DWORD dwBlendFactor = 0xffff0000 ;//| ( m_dwBlendFactor << 24 );
	// 오브젝트의 반투명 효과 세팅 
	if( m_dwBlendFactor < 255 || m_dwColor )
	{
		const GLfloat fAmbient[] = { 0, 0, 0, 1.0f };
		const GLfloat fDiffuse[] = { 1.0f, 1.0f, 1.0f, (float)((dwBlendFactor>>24)&0xff)/255.0f };
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, fAmbient );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fDiffuse );
		glEnable( GL_BLEND );
		glEnable( GL_ALPHA_TEST );
	} else 
	{
		D3DMATERIAL9 *pMaterial = g_TextureMng.GetMaterial( 0 );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, pMaterial->Ambient );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, pMaterial->Diffuse );
	}

	nNextFrame = GetNextFrame();
//	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

	// 엘리먼트엔 스킨,일반,모핑 심지어는 파티클까지도 포함될수있다.
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if( pObject3D == NULL )		continue;
		if( pElem->m_nEffect & XE_HIDE )	continue;

		pObject3D->m_nNoTexture = m_nNoTexture;
		pObject3D->m_nNoEffect = m_nNoEffect;
		// 뼈대에 링크되어 있다.
		if( pElem->m_nParentIdx != -1 )		// 본의 인덱스를 뜻한다.
		{
			// 만약 본의 개수가 바뀌었다면 ParentIdx도 틀려졌기 때문에 바꿔줘야 한다.
			MatrixMultiply( m1, m_mUpdateBone[ pElem->m_nParentIdx ], *mWorld );	// 뼈대에 링크되어 있다면 뼈대위치가 센터가 된다.
			if( pElem->m_nParentIdx == GetRHandIdx() )
				MatrixMultiply( m1, pElem->m_mLocalRH, m1 );
			else if( pElem->m_nParentIdx == GetLHandIdx() )
				MatrixMultiply( m1, pElem->m_mLocalLH, m1 );
			else if( pElem->m_nParentIdx == GetLArmIdx() )
				MatrixMultiply( m1, pElem->m_mLocalShield, m1 );
			else if( pElem->m_nParentIdx == GetRArmIdx() )
				MatrixMultiply( m1, pElem->m_mLocalKnuckle, m1 );
		}
		else
			m1 = *mWorld;
		
		if( i == PARTS_CLOAK )
		{
			g_ModelGlobal.SetTexture( m_nGL_CloakTexture );		// 외부지정 망토텍스쳐가 있다면.
		}
		/**/
		if( m_pBone )
			pObject3D->SetExternBone( m_mUpdateBone, m_pBaseBoneInv );	// 외장본이 있다면 그것을 넘겨준다.
		pObject3D->Render( pElem->m_pnGL_VBSel, m_fFrameCurrent, nNextFrame, &m1, pElem->m_nEffect, dwBlendFactor );

		pObject3D->m_nNoEffect = 0;

		if( i == PARTS_CLOAK )
			g_ModelGlobal.SetTexture( 0 );
	}
	
	// 상태 해제
	if( m_dwBlendFactor < 255 || m_dwColor )
	{
		if( m_nNoEffect == 0 )
		{
			const GLfloat fAmbient[] = { 0, 0, 0, 1.0f };
			const GLfloat fDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, fAmbient );
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fDiffuse );
			glDisable( GL_BLEND );
			glDisable( GL_ALPHA_TEST );
		}
	}

	return 1;
}


BOOL	CModelObject::RenderBB( const MATRIX *mWorld )
{
	return 1;
}


xRESULT CModelObject::RestoreDeviceObjects()
{
	int		i;
	O3D_ELEMENT	*pElem;
	CObject3D	*pObject3D;

	SetGroup(0);
	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if( pObject3D )
		{
			// pElem이 스킨일땐 m_pnGL_VB[]에 버텍스버퍼가 담겨서 돌아온다.
			pObject3D->RestoreDeviceObjects( pElem->m_pnGL_VB );
		}
	}
	
	return xSUCCESS;
}	
xRESULT CModelObject::DeleteDeviceObjects()
{
	int		i, j;
	O3D_ELEMENT	*pElem;
	CObject3D				*pObject3D;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if( pObject3D )
		{
			if( pElem->m_pnGL_VB )
			{
//				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
//					SAFE_RELEASE( pElem->m_pnGL_VB[j] );
				pElem->m_pObject3D->SetLOD(0);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )		// 
					glDeleteTextures( 1, &pElem->m_pnGL_VB1[j] );
				pElem->m_pObject3D->SetLOD(1);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
					glDeleteTextures( 1, &pElem->m_pnGL_VB2[j] );
				pElem->m_pObject3D->SetLOD(2);
				for( j = 0; j < pElem->m_pObject3D->GetMaxObject(); j ++ )
					glDeleteTextures( 1, &pElem->m_pnGL_VB3[j] );
				pElem->m_pObject3D->SetLOD(0);
	//				SAFE_DELETE_ARRAY( pElem->m_pnGL_VB );	// 여기서 이걸 삭제하면 안된다. device 자원만 삭제.
			}
//			pObject3D->DeleteDeviceObjects();
		}
	}

	return  xSUCCESS;
}	

void	CModelObject::FrameMove( float fSpeed )
{
/*	if( m_pMotion )		// CModel::FrameMove에서 프레임이 증가되기전에 검사해봐야 한다.
	{
		MOTION_ATTR *pAttr = IsAttrSound();
		if( pAttr )
			if( pAttr->m_nSndID > 0 && m_nPause == 0 )		// 효과음 속성이 있다면 플레이, pause상태면 사운드 출력 안함
				PLAYSND( pAttr->m_nSndID, pvSndPos );		
	} else
	{
		CObject3D *pObject = GetObject3D();
		MOTION_ATTR *pAttr = pObject->IsAttrSound( m_fFrameCurrent );
		if( pAttr )
			if( pAttr->m_nSndID > 0 && m_nPause == 0 )		// 효과음 속성이 있다면 플레이, pause상태면 사운드 출력 안함
				PLAYSND( pAttr->m_nSndID, pvSndPos );
	} */
	
	CModel::FrameMove( fSpeed );

	if( m_pForce )
	{
		if( /*!m_bEndFrame &&*/ m_pForce->m_nMaxSpline && !m_nPause )
		{
			m_pForce->m_nMaxDraw += (int)((MAX_SF_SLERP*2+2) /** fSpeed*/);
			if( m_pForce->m_nMaxDraw > m_pForce->m_nMaxSpline )
			{
				if( m_nLoop & ANILOOP_LOOP )	// 루핑되는 애니일경우는 첨부터 다시 그린다.
					m_pForce->m_nMaxDraw = 0;
				else
					m_pForce->m_nMaxDraw = m_pForce->m_nMaxSpline;
			}
				
		}

		m_pForce->Process();
	}

	if( m_pForce2 )
	{
		if( m_pForce2->m_nMaxSpline && !m_nPause )
		{
			m_pForce2->m_nMaxDraw += (int)((MAX_SF_SLERP*2+2) /** fSpeed*/);
			if( m_pForce2->m_nMaxDraw > m_pForce2->m_nMaxSpline )
			{
				if( m_nLoop & ANILOOP_LOOP )	// 루핑되는 애니일경우는 첨부터 다시 그린다.
					m_pForce2->m_nMaxDraw = 0;
				else
					m_pForce2->m_nMaxDraw = m_pForce2->m_nMaxSpline;
			}
			
		}
		
		m_pForce2->Process();
	}
	
	if( m_bMotionBlending )
	{
		m_fBlendWeight += 0.05f;
//		if( fabs(m_fBlendWeight - 1.0f) < 0.001f )			
		if( m_fBlendWeight >= 1.0f )		// 블렌딩이 1.0까지 다 진행됐으면 블렌딩 해제
			SetMotionBlending( FALSE );
	}

}

//
//	검광을 생성
//	m_pMotion의 첫프레임~마지막프레임까지의 검광을 생성한다.
void	CModelObject::MakeSWDForce( int nParts, DWORD dwItemKind3, BOOL bSlow, DWORD dwColor, float fSpeed )
{
	Vec3		v1, v2;
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
//	int		n1, n2;
	MATRIX	m1;

	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;

	// FrameCurrent를 0으로
	m_fFrameCurrent = 0.0f;
	m_nPause = 0;
	m_bEndFrame = FALSE;
	int nLoop = m_nLoop;
	m_nLoop = ANILOOP_1PLAY;
	
	CreateForce( nParts );	// 검광 오브젝트 생성
	CSwordForce *pForce;
	if( nParts == PARTS_RWEAPON )
		pForce = m_pForce;
	else
		pForce = m_pForce2;
	pForce->Clear();
	pForce->m_dwColor = dwColor;

	MATRIX *pmLocal;
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else if( pElem->m_nParentIdx == GetLArmIdx() )
		pmLocal = &pElem->m_mLocalShield;
	else if( pElem->m_nParentIdx == GetRArmIdx() )
		pmLocal = &pElem->m_mLocalKnuckle;
	

	while(1)
	{
		if( m_pMotion )
			m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
//		AnimateBone( FALSE );		// 뼈대를 애니메이션 시킴. 스키닝은 하지 않음

		// 무기 WorldTM = 무기LocalTM X 무기부모WorldTM  
		MatrixMultiply( m1, *pmLocal, m_mUpdateBone[ pElem->m_nParentIdx ] );
		
		// 칼끝 버텍스의 월드 좌표 계산
		MatrixVec3Multiply( v1, (pElem->m_pObject3D->m_vForce1), m1 );
		MatrixVec3Multiply( v2, (pElem->m_pObject3D->m_vForce2), m1 );
		pForce->Add( v1, v2 );

		CModel::FrameMove( fSpeed );
		if( bSlow && IsAttrHit( m_fFrameCurrent ) )
			m_bSlow = TRUE;
		if( m_bEndFrame )
			break;
	}

	pForce->MakeSpline();		// 스플라인 생성
	

	// rewind
	m_fFrameCurrent = 0.0f;
	m_nPause = 0;
	m_bEndFrame = FALSE;
	m_nLoop = nLoop;
	m_bSlow = FALSE;
}

//
//
//
void	CModelObject::SetMotionBlending( BOOL bFlag )
{ 
	if( bFlag == m_bMotionBlending )	return;
	m_bMotionBlending = bFlag;
	m_fBlendWeight = 0.0f;
}

//
// 교차/충돌테스트 관련
//
//
// 레이에 교차하는 버텍스를 찾아서 리턴.
//
Vec3 *CModelObject::IntersectRayVertex( Vec3 *pOut, const Vec3 &vRayOrig, const Vec3 &vRayDir )
{
	int		i;
	O3D_ELEMENT		*pElem = m_Element;
	Vec3	*pV;

	for( i = 0; i < MAX_ELEMENT; i ++ )
	{
		if( pElem->m_pObject3D )
		{
			if( pV = pElem->m_pObject3D->IntersecRayVertex( pOut, vRayOrig, vRayDir ) )
				return pV;		// 찾았으면 *pOut엔 좌표, 리턴포인터는 실제버텍스.
		}
		pElem ++;
	}

	return NULL;
}


void	CModelObject::GetForcePos( Vec3 *vOut, int nIdx, int nParts, const MATRIX &mWorld )
{
	MATRIX *pmLocal;
	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
	MATRIX	m1;
	Vec3		v1;
	
	if( pElem->m_pObject3D == NULL )		return;
	
	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;
	
	if( pElem->m_nParentIdx == GetRHandIdx() )
		pmLocal = &pElem->m_mLocalRH;
	else if( pElem->m_nParentIdx == GetLHandIdx() )
		pmLocal = &pElem->m_mLocalLH;
	else
	{
		MatrixIdentity( m1 );
		pmLocal = &m1;						// 일단 에러는 안나게 이렇게 한다.
	}
		
	
//	if( m_pMotion )
//		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
	
	MatrixMultiply( m1, *pmLocal, m_mUpdateBone[ pElem->m_nParentIdx ] );
	if( nIdx == 0 )
		MatrixVec3Multiply( v1, (pElem->m_pObject3D->m_vForce1), m1 );
	else
		MatrixVec3Multiply( v1, (pElem->m_pObject3D->m_vForce2), m1 );
	
	MatrixVec3Multiply( v1, v1, mWorld );
	*vOut = v1;
	
}

// 주먹 중앙의 위치를 계산할때.
void	CModelObject::GetHandPos( Vec3 *vOut, int nParts, const MATRIX &mWorld )
{
	MATRIX *pmLocal;
//	O3D_ELEMENT		*pElem = GetParts( nParts );		// 오른손 무기의 포인터
	MATRIX	m1;
	Vec3		v1;

//	if( pElem->m_pObject3D == NULL )		return;
	
//	if( pElem->m_pObject3D->m_vForce1.x == 0 )	return;
//	if( pElem->m_pObject3D->m_vForce2.x == 0 )	return;
	
	int nParentIdx = 0;
	if( m_pBone == NULL )	return;
	if( nParts == PARTS_RWEAPON )
	{
		nParentIdx = GetRHandIdx();
		pmLocal = &m_pBone->m_mLocalRH;
	}
	else if( nParts == PARTS_LWEAPON  )
	{
		nParentIdx = GetLHandIdx();
		pmLocal = &m_pBone->m_mLocalLH;
	}
	else
	{
		MatrixIdentity( m1 );
		pmLocal = &m1;						// 일단 에러는 안나게 이렇게 한다.
	}
	
	
//	if( m_pMotion )
//		m_pMotion->AnimateBone( m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight );		// 일단 뼈대가 있다면 뼈대 애니메이션 시킴
	
	// 애니메이션끝난 본의 매트릭스와 LocalR/LH를 곱해서 최종 매트릭스 구함.
	MatrixMultiply( m1, *pmLocal, m_mUpdateBone[ nParentIdx ] );
	v1.x = m1.f[_41];		v1.y = m1.f[_42];		v1.z = m1.f[_43];		// 좌표만 필요함.
	
	MatrixVec3Multiply( v1, v1, mWorld );
	*vOut = v1;
	
}


///////////////////////////////////////////////////////////////////////////////////
//
//
//
Vec3		SplineSlerp( Vec3 *v1, Vec3 *v2, Vec3 *v3, Vec3 *v4, float fSlerp )
{
	Vec3		vResult;

	float	t = fSlerp;
	float	t2 = t * t;
	float	t3 = t2 * t;

	Vec3		m0, m1;
	const float alpha = 0.0f;

	m0 = ((1 - alpha) / 2.0f) * 
		 ((*v2 - *v1) + *v3 - *v2);
	m1 = ((1 - alpha) / 2.0f) *
		 ((*v3 - *v2) + *v4 - *v3);

	vResult = (((2 * t3) - (3 * t2) + 1) * *v2) +
			   ((t3 - (2 * t2) + t) * m0) + 
			   ((t3 - t2) * m1) +
			   (((-2 * t3) + (3 * t2)) *
			   *v3 );

	return vResult;
}

void	CSwordForce::Add( Vec3 v1, Vec3 v2 )
{
	if( m_nMaxVertex >= MAX_SF_SWDFORCE )
	{
		XLOG( "SWDForceAdd : 범위 초과 %d", m_nMaxVertex );
		return;
	}
	FVF_SWDFORCE*	pList = &m_aList[ m_nMaxVertex ];

	pList->position = v1;
	pList->color = 0xffffffff;

	pList = &m_aList2[ m_nMaxVertex ];
	pList->position = v2;
	pList->color = 0xffffffff;

	m_nMaxVertex ++;
}

// 등록된 키리스트를 스플라인 보간된 리스트로 바꾼다.
// 이렇게 만들어진 리스트를 최종 그린다.
void	CSwordForce::MakeSpline( void )
{
	FVF_SWDFORCE *pKey;
	int		i, j;
	Vec3 vSlp;
	int		a, b, c, d;
	int		nMaxVertex = m_nMaxVertex;

	m_nMaxSpline = 0;
	for( i = 0; i < nMaxVertex-1; i ++ )
	{
		// i ~ i+1사이를 보간한다.  
		a = i - 1;		if( a < 0 )	a = 0;
		b = i;
		c = i+1;
		d = i+2;		if( d >= nMaxVertex )	d = nMaxVertex - 1;
		for( j = 0; j < MAX_SF_SLERP+1; j ++ )
		{
			if( m_nMaxSpline >= MAX_SF_SPLINE )
			{
				XLOG( "SWDForceAdd Spline : 범위 초과 %d", m_nMaxSpline );
				break;
			}
			pKey = m_aList;
			vSlp = SplineSlerp( &pKey[a].position, &pKey[b].position, &pKey[c].position, &pKey[d].position, (float)j / MAX_SF_SLERP );
			m_aSpline[ m_nMaxSpline ].position = vSlp;
			m_aSpline[ m_nMaxSpline ].color = 0xffffffff;
			m_nMaxSpline ++;
			pKey = m_aList2;
			vSlp = SplineSlerp( &pKey[a].position, &pKey[b].position, &pKey[c].position, &pKey[d].position, (float)j / MAX_SF_SLERP );
			m_aSpline[ m_nMaxSpline ].position = vSlp;
			m_aSpline[ m_nMaxSpline ].color = 0xffffffff;
			m_nMaxSpline ++;
		}
	}
}

void	CSwordForce::Process( void )
{
	FVF_SWDFORCE *pList = m_aSpline;
	int		i;
	int		nRed1 =  0;
	int		nGreen1 =  0;
	int		nBlue1 =   0;
	int		nRed2 = (m_dwColor >> 16) & 0xff;
	int		nGreen2 = (m_dwColor >> 8) & 0xff;
	int		nBlue2 = m_dwColor & 0xff;
//	int		nRed2 = 120;
//	int		nGreen2 = 120;
//	int		nBlue2 = 230;
	
	if( m_nMaxSpline == 0 )		return;

	for( i = m_nMaxDraw - 1; i >= 0; i -- )
	{
		if( i % 2 )
			pList[i].color = XCOLOR_ARGB( 255, nRed1, nGreen1, nBlue1 ); 
		else
			pList[i].color = XCOLOR_ARGB( 255, nRed2, nGreen2, nBlue2 ); 
		
		nRed1   -= 3; if( nRed1   < 0 )	nRed1   = 0;
		nGreen1 -= 3; if( nGreen1 < 0 ) nGreen1 = 0;
		nBlue1  -= 3; if( nBlue1  < 0 )	nBlue1  = 0;

		nRed2   -= 3; if( nRed2   < 0 )	nRed2   = 0;
		nGreen2 -= 3; if( nGreen2 < 0 ) nGreen2 = 0;
		nBlue2  -= 3; if( nBlue2  < 0 )	nBlue2  = 0;
	}

}
//
//
//
void	CSwordForce::Draw( const MATRIX *mWorld )
{

//	if( m_nMaxSpline == 0 )	return;
	glPushMatrix();

	glMultMatrixf( mWorld->f );

	glPopMatrix(); 
}

/*

  SetMotionTrans(...);		// 이전에 실행하던 동작에서 새로운 동작으로 전이가 되도록 한다.



*/



