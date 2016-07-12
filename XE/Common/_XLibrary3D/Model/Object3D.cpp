#if 1
#include <stdio.h>
#include "Object3D.h"
#include "Material.h"
#include "xUtil3D.h"
#include "ModelObject.h"
#include "Timer.h"
#include "stddef.h"

GLuint	g_nReflectMap = NULL;

//LPDIRECT3DVERTEXDECLARATION9 g_pSkinVertexDeclaration;
//LPDIRECT3DVERTEXDECLARATION9 g_pShadowVertexDeclaration;
//LPDIRECT3DVERTEXSHADER9 g_pSkiningVS;
//LPDIRECT3DVERTEXSHADER9 g_pShadowVS;

/*MATRIX	g_mReflect = MATRIX(  0.50f, 0.00f, 0.00f, 0.00f,
									  0.00f,-0.50f, 0.00f, 0.00f,
									  0.00f, 0.00f, 1.00f, 0.00f,
									  0.50f, 0.50f, 0.00f, 1.00f );
*/

////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CObject3DMng		g_Object3DMng;

CObject3DMng::CObject3DMng()
{
	Init();
}

CObject3DMng::~CObject3DMng()
{
	Destroy();
}

void	CObject3DMng::Init( void )
{
	int		i;
	m_nSize = 0;
	m_nMax = 0;
	m_nCachePos = 0;

	for( i = 0; i < MAX_OBJECT3D; i ++ )	m_pObject3DAry[i] = NULL;
	for( i = 0; i < MAX_OBJECT3D; i ++ )	m_pCache[i] = NULL;

	m_tmTimer = timeGetTime();
}

// 디바이스 자원과 메모리 모두를 날림.
void	CObject3DMng::Destroy( void )
{
	int		i;
	CObject3D		**pAry = m_pObject3DAry;
	CObject3D		*pObject3D;

	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			pObject3D->DeleteDeviceObjects();
			SAFE_DELETE( pObject3D );
			//TRACE( "Delete Object 3D%p\n", pObject3D);
		}
	}

	Init();
}

//xRESULT CObject3DMng::InvalidateDeviceObjects()
//{
//	DeleteDeviceObjects();
//	return  xSUCCESS;
//}

// 디바이스 자원만 날림.
xRESULT CObject3DMng::DeleteDeviceObjects()
{
	int		i;
	CObject3D		**pAry = m_pObject3DAry;
	CObject3D		*pObject3D;
	
	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			pObject3D->DeleteDeviceObjects();
		}
	}

	return xSUCCESS;
}

// pTexture를 사용하는 매터리얼을 찾아 삭제한다.
// 공유되어 있는 텍스쳐라면 사용카운터를 보고 1인것만 삭제한다..
int CObject3DMng::DeleteObject3D( CObject3D *pObject3D )
{
	int		i, j;
	
	if( pObject3D == NULL )	return FALSE;
	if( m_nMax == 0 )	return FALSE;
	
	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		if( m_pObject3DAry[i] )
		{
			if( m_pObject3DAry[i] == pObject3D )		// pObject3D를 찾았다.
			{
				if( m_pObject3DAry[i]->m_nUseCnt == 1 )			// 공유된게 아니다(usecnt == 1)
				{
					for( j = 0; j < MAX_OBJECT3D_CACHE; j ++ )
					{
						if( m_pCache[j] == pObject3D )
						{
							m_pCache[j] = NULL;			// 캐쉬에 있었는지 먼저 검사해보고 캐쉬를 먼저 치움.
							break;
						}
					}
					SAFE_DELETE( m_pObject3DAry[i] );			// 오브젝트 관리자에서도 삭제하고 실제 객체도 날려버림.
					m_nMax --;
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}



//
//
//
CObject3D		*CObject3DMng::LoadObject3D( LPCTSTR szFileName )
{
	int			i;
	CObject3D	**pAry;
	CObject3D	*pObject3D;
	int			nIdx = -1;

	// 이미 메모리에 적재 되었는지 검색, 동시에 빈곳도 검색
	// 일단 캐쉬를 검색
	pAry = m_pCache;
	for( i = 0; i < MAX_OBJECT3D_CACHE; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			if( strcmpi(pObject3D->m_szFileName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
			{
				pObject3D->m_nUseCnt ++;	// 중복되어 사용되어지면 카운트 올림.
				return pObject3D;
			}
		}
	}
	// 이미 메모리에 적재 되었는지 검색, 동시에 빈곳도 검색
	pAry = m_pObject3DAry;
	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )				// 널이 아닌것은 무슨 모션이든 로딩되어 있다는 뜻
		{
			if( strcmpi(pObject3D->m_szFileName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
			{
				pObject3D->m_nUseCnt ++;	// 중복되어 사용되어지면 카운트 올림.
				return pObject3D;
			}
		} else
		{
			if( nIdx == -1 )		nIdx = i;			// 빈곳이 있으면 가장처음 빈곳이 나온곳을 기억해둠
		}
	}
	if( nIdx == -1 )	
	{
		XLOG( "%s : 읽을 수 있는 한계를 초과했다", szFileName );
	}

	pObject3D = new CObject3D;
//	pObject3D->InitDeviceObjects( pd3dDevice );

	// 로딩된게 아니었다면.  실제로 데이타 읽음.
	if( pObject3D->LoadObject( szFileName ) == FAIL )
	{
		return NULL;
	}

	pObject3D->m_nUseCnt = 1;	// 처음 로딩되었으면 1부터
	// 읽은 메쉬포인터를 리스트에 등록
	m_pObject3DAry[ nIdx ] = pObject3D;
	
	// 캐시에도 넣음
	m_pCache[ m_nCachePos++ ] = pObject3D;
	if( m_nCachePos >= MAX_OBJECT3D_CACHE )		m_nCachePos = 0;

	m_nMax ++;			// 가진 모션 갯수 증가

	return pObject3D;		// 읽은 모션 포인터 리턴
}

// 주기적으로 검사해서 사용한지 오래된놈은 메모리에서 날림.
// 서버에서는 사용하지 말자.
void CObject3DMng::Process( void )
{
	return;
}



////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////


CObject3D::CObject3D()
{
	Init();
}

CObject3D::~CObject3D()
{
	Destroy();
}

void	CObject3D::Init( void )
{
	m_pGroup	  = &m_Group[0];
	memset( m_Group, 0, sizeof(LOD_GROUP) * MAX_GROUP );
	m_pmExternBone = m_pmExternBoneInv = NULL;
	m_pBaseBone	   = NULL;
	m_pBaseBoneInv = NULL;
	m_pMotion = NULL;
	memset( &m_CollObject, 0, sizeof(GMOBJECT) );
	m_CollObject.m_Type = GMT_ERROR;
	m_nID = 0;
	m_nHavePhysique = FALSE;
	m_nMaxBone = 0;
	m_bSendVS = 0;
	m_bLOD = 0;
	memset( m_szFileName, 0, sizeof(m_szFileName) );

	m_vForce1.x = m_vForce1.y = m_vForce1.z = 0;
	m_vForce2.x = m_vForce2.y = m_vForce2.z = 0;

	m_vBBMin.x = m_vBBMin.y = m_vBBMin.z = 65535.0;
	m_vBBMax.x = m_vBBMax.y = m_vBBMax.z = -65535.0;
	m_nMaxFace = 0;
	m_nNoTexture = 0;
	m_nNoEffect = 0;

	m_nMaxEvent = 0;
	memset( m_vEvent, 0, sizeof(m_vEvent) );

	m_fScrlU = m_fScrlV = 0.0f;

	m_nUseCnt = 0;	// 일단 초기화는 0
	
	m_fAmbient[0] = 1.0f;
	m_fAmbient[1] = 1.0f;
	m_fAmbient[2] = 1.0f;

	m_nTextureEx = 0;
	m_pAttr = NULL;
}

void	CObject3D::Destroy( void )
{
	int		i, j, k;

	DeleteDeviceObjects();	// m_pObject[].m_nGL_VB
	
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT *pObject = m_Group[j].m_pObject;
		for( i = 0; i < m_Group[j].m_nMaxObject; i ++ )
		{
			SAFE_DELETE_ARRAY( pObject[i].m_pVertexList );
			SAFE_DELETE_ARRAY( pObject[i].m_pPhysiqueVertex );
			for( k = 0; k < pObject[i].m_nMaxMtrlBlk; k ++ )
			{
				g_TextureMng.DeleteMaterial( pObject[i].m_pGL_MtrlBlkTexture[k] );	// 사용하던 텍스쳐는 삭제한다. 공유하는것이면 삭제 안함.
			}
			
			for( int m = 0; m < pObject[i].m_nMaxMtrlBlk; m ++ )
				SAFE_DELETE_ARRAY( pObject[i].m_pMtrlBlk[m].m_pIB );
			SAFE_DELETE_ARRAY( pObject[i].m_pMtrlBlk );
			SAFE_DELETE_ARRAY( pObject[i].m_pGL_MtrlBlkTexture );
			SAFE_DELETE_ARRAY( pObject[i].m_pFrame );

			SAFE_DELETE_ARRAY( pObject[i].m_pVB );
//			SAFE_DELETE_ARRAY( pObject[i].m_pIB );
//			pObject[i].m_pIIB = NULL;		// IIB는 IB와 같은 블럭을 사용하므로 delete하면 안된다.
			SAFE_DELETE_ARRAY( pObject[i].m_pIIB );
		}
	}
	
	SAFE_DELETE_ARRAY( m_CollObject.m_pVertexList );
	SAFE_DELETE_ARRAY( m_CollObject.m_pPhysiqueVertex );
	for( int i = 0; i < m_CollObject.m_nMaxMtrlBlk; i ++ )
		SAFE_DELETE_ARRAY( m_CollObject.m_pMtrlBlk[i].m_pIB );
	SAFE_DELETE_ARRAY( m_CollObject.m_pMtrlBlk );
	SAFE_DELETE_ARRAY( m_CollObject.m_pGL_MtrlBlkTexture );
	SAFE_DELETE_ARRAY( m_CollObject.m_pFrame );

	
	SAFE_DELETE_ARRAY( m_pAttr );
	
	SAFE_DELETE_ARRAY( m_CollObject.m_pVB );
//	SAFE_DELETE_ARRAY( m_CollObject.m_pIB );
//	m_CollObject.m_pIIB = NULL;		// IIB는 IB와 같은 블럭을 사용하므로 delete하면 안된다.
	SAFE_DELETE_ARRAY( m_CollObject.m_pIIB );

	// m_pObject를 삭제하려면 DeleteDeviceObjets()를 먼저하고 해야한다.	
	
	SAFE_DELETE_ARRAY( m_Group[0].m_pObject );		// 통짜 메모리 풀이므로 [0]만 지우면 된다.
	m_Group[1].m_pObject = NULL;
	m_Group[2].m_pObject = NULL;
	SAFE_DELETE_ARRAY( m_Group[0]._mUpdate );		// 통짜 메모리 풀이므로 [0]만 지우면 된다.
	m_Group[1]._mUpdate = NULL;
	m_Group[2]._mUpdate = NULL;
	// 충돌용 메시는 d3d_VB를 생성하지 않기때문에 DeleteDevice를 안해도 된다.
	SAFE_DELETE_ARRAY( m_pBaseBone );
	SAFE_DELETE( m_pMotion );

	Init();
}

int		CObject3D::GetMaxVertex( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxVertexList;
	return nMax;
}

int		CObject3D::GetMaxFace( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxFaceList;
	return nMax;
}

int		CObject3D::GetMaxMtrlBlk( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxMtrlBlk;
	return nMax;
}


//
//	버텍스 버퍼를 생성/재생성.
//
xRESULT CObject3D::RestoreDeviceObjects( GLuint *pnGL_VB )
{
	int		i, j;
	GMOBJECT	*pObj;

	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT	*pObject = m_Group[j].m_pObject;
		int			nMaxObject = m_Group[j].m_nMaxObject;
		for( i = 0; i < nMaxObject; i ++ )
		{
			pObj = &pObject[i];
			if( pObj->m_Type == GMT_SKIN )	
			{
				// 스킨은 버텍스버퍼를 따로 써야 하므로 지정된 외부포인터에다 버텍스 버퍼를 받음.
				CreateDeviceBuffer( pObj, &pnGL_VB[i] );

			} else
			{
				if( pObj->m_nGL_VB )	
					continue;

				CreateDeviceBuffer( pObj, NULL );

			}
		}
		pnGL_VB += m_Group[j].m_nMaxObject;
	}


	return xSUCCESS;
}	

//xRESULT CObject3D::InvalidateDeviceObjects()
//{
//	DeleteDeviceObjects();
//	return  xSUCCESS;
//}	

xRESULT CObject3D::DeleteDeviceObjects()
{
	int		i, j;
	GMOBJECT	*pObj;
	
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT	*pObject = m_Group[j].m_pObject;
		int			nMaxObject = m_Group[j].m_nMaxObject;
		for( i = 0; i < nMaxObject; i ++ )
		{
			pObj = &pObject[i];
			glDeleteTextures( 1, &pObj->m_nGL_VB );
			for( int m = 0; m < pObj->m_nMaxMtrlBlk; m ++ )
				glDeleteTextures( 1, &pObj->m_pMtrlBlk[m].m_nGL_IB );
			// MtrlBlk의 m_nGL_Texture는 여기서 지우지 않는다.
		}
	}
	return  xSUCCESS;
}	

int		g_MaxVB = 0;
//
//	버텍스 버퍼 & 인덱스 버퍼생성.
//
xRESULT	CObject3D::CreateDeviceBuffer( GMOBJECT *pObject, GLuint *pnGL_VB )
{
	xRESULT		hr;
	int			nVertexSize;

	nVertexSize = pObject->m_nVertexSize;

	if( pnGL_VB == NULL )		// pnGL_VB가 널이면 내부... this->m_nGL_VB를 써야 한다.
	{
		// create vertexbuffer
		if( pObject->m_nGL_VB == 0 )
		{
			// for Indexed Primitive
#ifdef __XDEBUG
			g_MaxVB += (pObject->m_nMaxVB * nVertexSize);
#endif
			glGenBuffers( 1, &pObject->m_nGL_VB );
			if( pObject->m_nGL_VB == 0 )
			{
				XERROR( "1 %s Object3D 버텍스 버퍼 생성 실패 : m_nMaxVB=%d, nVertexSize=%d", 
																		  m_szFileName, pObject->m_nMaxVB, 
																		  nVertexSize );
				return xFAIL;
			}
			SendVertexBuffer( pObject, pObject->m_nGL_VB );
		}
	} else
	{
		if( *pnGL_VB == 0 )		// 아직 할당 되지 않았음
		{
			// for Indexed Primitive
#ifdef __XDEBUG
			g_MaxVB += (pObject->m_nMaxVB * nVertexSize);
#endif
			glGenBuffers( 1, pnGL_VB );
			if( *pnGL_VB == 0 )
			{
				XERROR( "2 %s Object3D 버텍스 버퍼 생성 실패 : m_nMaxVB=%d, nVertexSize=%d", 
																		  m_szFileName, pObject->m_nMaxVB, 
																		  nVertexSize );
				return xFAIL;
			}
			SendVertexBuffer( pObject, *pnGL_VB );
		}
	}
	
	// create indexbuffer
	for( int i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
	{
		MATERIAL_BLOCK *pBlock = &pObject->m_pMtrlBlk[i];
		int nMax = pBlock->m_nPrimitiveCount;
		if( pBlock->m_nGL_IB == 0 )
		{
	#ifdef __XDEBUG
			g_MaxVB += (nMax * sizeof(WORD) * 3);
	#endif	
			glGenBuffers( 1, &pBlock->m_nGL_IB );
			if( pBlock->m_nGL_IB == 0 )
			{
				XERROR( "%s Object3D 인덱스 버퍼 생성 실패 : m_nMaxIB=%d", 
					   m_szFileName, pObject->m_nMaxIB );
				return xFAIL;
			}
			SendIndexBuffer( pBlock );
		}
	}	
	
	return xSUCCESS;
}

// szFileName이 애니메이션 파일인가?
BOOL CObject3D::IsAnimateFile( LPCTSTR szFileName )
{
	FILE *fp = fopen( szFileName, "rb" );
	if( fp == NULL )	
	{
		XERROR( "IsAnimateFile : %s 읽기 실패", szFileName );
		return FAIL;
	}
	char c0;
	int d0;
	char buff[64];
	Vec3 v;
	float f0;

	fread( &c0, 1, 1, fp );		// 파일명 스트링 길이 일음.
	fread( buff, c0, 1, fp );	// 파일명 읽음.
	fread( &d0, 4, 1, fp );		// 버전
	
	fread( &d0, 4, 1, fp );		// Serial ID
	fread( &v, sizeof(Vec3), 1, fp );		// 검광1,2의 좌표인데 일단 이렇게 하자.
	fread( &v, sizeof(Vec3), 1, fp );
	fread( &f0, sizeof(float), 1, fp );
	fread( &f0, sizeof(float), 1, fp );
	fseek( fp, 16, SEEK_CUR );		// reserved
	
	fread( &v, sizeof(Vec3), 1, fp );		// 대표 바운딩 박스
	fread( &v, sizeof(Vec3), 1, fp );
	fread( &f0, sizeof(float), 1, fp );		// per slerp
	fread( &d0, 4, 1, fp );					// ani frame 수.  애니가 없으면 0이되도록 저장할것.

	BOOL bAnimate = FALSE;
	if( d0 > 0 )			// MaxFrame이 있냐?
		bAnimate = TRUE;
	else
		bAnimate = FALSE;

	fclose(fp);

	return bAnimate;
}

//
//		Load Object
//
int		CObject3D::LoadObject( LPCTSTR szFileName )
{
	int		j, nTemp;
	int		nVer;
	Vec3	vTemp;
	BOOL	bNormalObj = FALSE;

	FILE *fp;
	fp = fopen( szFileName, "rb" );
	if( fp == NULL )	
	{
		XERROR( "LoadObject : %s 읽기 실패", szFileName );
		return FAIL;
	}
	Init();

	// 파일명 카피
	char szName[64];
	strcpy( m_szFileName, GetFileName( szFileName ) );
	strlwr( m_szFileName );
	strcpy( szName, GetFileName(szFileName) );

	char buff[64];
	char cLen;
	fread( &cLen, 1, 1, fp );		// 파일명 스트링 길이 일음.
	fread( buff, cLen, 1, fp );	// 파일명 읽음.
	for( j = 0; j < cLen; j ++ )
		buff[j] = buff[j] ^ (char)0xcd;	// 암호화 해제

	if( cLen >= 64 )
	{
		XERROR( "LoadObject3D : %s 파일명이 너무 길다 %d", szFileName, (int)cLen );
		return 0;
	}

	buff[cLen] = 0;	// 끝에 널 붙임.
	if( strcmpi( szName, buff ) != 0 )	// 헤더의 파일명과 비교해보고 틀리면 에러.
	{
		XERROR( "잘못된 파일 : %s", szFileName );
		return 0;
	}

	//--- 공통 헤더부
	fread( &nVer, 4, 1, fp );		// 버전
	if( nVer < VER_MESH )
	{
		XERROR( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_MESH );
		fclose(fp);
		return FAIL;
	}
		
	fread( &m_nID, 4, 1, fp );		// Serial ID
	fread( &m_vForce1, sizeof(Vec3), 1, fp );		// 검광1,2의 좌표인데 일단 이렇게 하자.
	fread( &m_vForce2, sizeof(Vec3), 1, fp );
	fread( &m_fScrlU, sizeof(float), 1, fp );
	fread( &m_fScrlV, sizeof(float), 1, fp );
	fseek( fp, 16, SEEK_CUR );		// reserved

	fread( &m_vBBMin, sizeof(Vec3), 1, fp );		// 대표 바운딩 박스
	fread( &m_vBBMax, sizeof(Vec3), 1, fp );
	fread( &m_fPerSlerp, sizeof(float), 1, fp );		// per slerp
	fread( &m_nMaxFrame, 4, 1, fp );					// ani frame 수.  애니가 없으면 0이되도록 저장할것.

	fread( &m_nMaxEvent, 4, 1, fp );	// 이벤트 좌표
	if( m_nMaxEvent > 0 )
		fread( m_vEvent, sizeof(Vec3) * m_nMaxEvent, 1, fp );

	fread( &nTemp, 4, 1, fp );
	if( nTemp )
	{
		m_CollObject.m_Type = GMT_NORMAL;
		LoadGMObject( fp, &m_CollObject );		// 충돌용 메시
	}
	fread( &m_bLOD, 4, 1, fp );					// LOD가 있는가?

	//--- 자체내장 본 애니메이션이 있다면 본 개수가 있을것이다.  ex) Obj_풍선.o3d    parts_female.o3d, mvr_타조.o3d는 본파일이 따로 있음.
	fread( &m_nMaxBone, 4, 1, fp );

	if( m_nMaxBone > 0 )
	{
		m_pBaseBone = new MATRIX[ m_nMaxBone * 2 ];
		m_pBaseBoneInv = m_pBaseBone + m_nMaxBone;		// InverseTM은 뒤쪽에 붙는다.
		fread( m_pBaseBone,	   sizeof(MATRIX) * m_nMaxBone, 1, fp );		// 디폴트 뼈대 셋트 
		fread( m_pBaseBoneInv, sizeof(MATRIX) * m_nMaxBone, 1, fp );		// InverseTM 세트
		if( m_nMaxFrame > 0 )	// 본이 있고 MaxFrame이 있으면 애니메이션이 있다는걸로 간주.
		{
			m_pMotion = new CMotion;
			m_pMotion->ReadTM( fp, m_nMaxBone, m_nMaxFrame );		// 본 애니메이션 읽음.
		}
		fread( &m_bSendVS, 4, 1, fp );		// 본개수가 MAX_VS_BONE보다 적어 VS로 한번에 전송가능한 것인가?
	}

	int nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;	// LOD가 있으면 3개다 읽음. 없으면 1개만 읽음.
	LOD_GROUP *pGroup;
	int nPoolSize, nDebugSize = 0;
	fread( &nPoolSize, 4, 1, fp );		// 메모리 풀 사이즈.
	GMOBJECT *pPool = new GMOBJECT[ nPoolSize ];	// 메모리 풀.
	if( pPool == NULL )
		XERROR( "메모리 할당 실패:CObject3D::LoadObject( %s ) %d", m_szFileName, nPoolSize );
	memset( pPool, 0, sizeof(GMOBJECT) * nPoolSize );
	for( int i = 0; i < nMaxGroup; i ++ )
	{
		pGroup = &m_Group[i];
		fread( &pGroup->m_nMaxObject, 4, 1, fp );		// geometry 갯수

		pGroup->m_pObject = pPool;		// 메모리 포인터 할당.
		pPool += pGroup->m_nMaxObject;
#ifdef __XDEBUG
		nDebugSize += pGroup->m_nMaxObject;
		if( nDebugSize > nPoolSize )
			XERROR( "LoadObject : 메모리 오버플로우 %d, %d", nPoolSize, nDebugSize );
#endif
		memset( pGroup->m_pObject, 0, sizeof(GMOBJECT) * pGroup->m_nMaxObject );
		for( j = 0; j < pGroup->m_nMaxObject; j ++ )		pGroup->m_pObject[j].m_nID = -1;
		
		GMOBJECT	*pObject;
		int		nParentIdx;
		BOOL	bAnimate = FALSE;
		int		nType;
		//------  Geometry Object
		for( j = 0; j < pGroup->m_nMaxObject; j ++ )
		{
			pObject = &pGroup->m_pObject[j];

			fread( &nType, 4, 1, fp );	// Type
			pObject->m_Type = (GMTYPE)(nType & 0xffff);
			if( nType & 0x80000000 )
				pObject->m_bLight = TRUE;

			fread( &pObject->m_nMaxUseBone, 4, 1, fp );
			if( pObject->m_nMaxUseBone > 0 )
				fread( pObject->m_UseBone, sizeof(int) * pObject->m_nMaxUseBone, 1, fp );	// 오브젝트가 사용하는 본리스트
			
			if( pObject->m_Type == GMT_SKIN )
			{
				pObject->m_nVertexSize = sizeof(SKINVERTEX);
//				pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
			} else
			{
				pObject->m_nVertexSize = sizeof(NORMALVERTEX);
//				pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
			}

			fread( &pObject->m_nID, 4, 1, fp );	// Object ID
			fread( &nParentIdx, 4, 1, fp );			// parent idx
			pObject->m_nParentIdx = nParentIdx;		// save를 위해 백업 받아둠
			
			// 부모가 있다면 부모 포인터 지정
			if( nParentIdx != -1 )
			{
				fread( &pObject->m_ParentType, 4, 1, fp );		// 부모의 타입 읽음.
				// 부모의 포인터를 세팅
				switch( pObject->m_ParentType )
				{
				case GMT_BONE:		pObject->m_pParent = NULL;	break;		// 부모가 본이면 m_pParent를 사용하지 않음.
				case GMT_NORMAL:	pObject->m_pParent = &pGroup->m_pObject[ nParentIdx ];		break;	// 부모가 일반오브젝이라면 그 포인터 지정.
					break;
				}
			}

			// 원점기준의 LocalTM.  부모가 있다면 원점은 부모가 된다.
			fread( &pObject->m_mLocalTM, sizeof(MATRIX), 1, fp );

			// load geometry
			LoadGMObject( fp, pObject );			// Mesh부 읽음
			
			if( pObject->m_Type == GMT_SKIN )
			{
				{
					pObject->m_nVertexSize = sizeof(SKINVERTEX);
//					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
				}
			} else
			{
				{
					pObject->m_nVertexSize = sizeof(NORMALVERTEX);
//					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
				}
			}
			
			if( pObject->m_pPhysiqueVertex == NULL )	bNormalObj = TRUE;	// 일반형오브젝트가 하나라도 있을때.
			else										m_nHavePhysique = TRUE;		// 피지크 오브젝트가 하나라도 있으면 TRUE가 된다.

			// load TM animation - 
			if( pObject->m_Type == GMT_NORMAL )	// 스킨에는 m_pFrame이 아예 없다.
			{
				if( m_nMaxFrame > 0 )	LoadTMAni( fp, pObject );			// TM Animation 데이타 부분.  maxframe이0이면 아예 읽지 않음.
			}

		}
	} // LOD_GROUP
	// boudbox vMin, vMax값을 이용해 8개의 벡터로 풀어냄
	SetBB( m_vBBVList, m_vBBMin, m_vBBMax );

	// 갱신용 매트릭스 리스트 생성. - 일반형오브젝트가 하나라도 잇으면 생성.
	if( bNormalObj == TRUE )
	{
		MATRIX *pmPool = new MATRIX[ nPoolSize ];
		for( int i = 0; i < nMaxGroup; i ++ )
		{
			m_Group[i]._mUpdate = pmPool;
			pmPool += m_Group[i].m_nMaxObject;
			for( j = 0; j < m_Group[i].m_nMaxObject; j ++ )
				MatrixIdentity( m_Group[i]._mUpdate[j] );
		}
	}

	if( m_nMaxFrame > 0 )
	{
		int nAttr = 0;
		
		fread( &nAttr, 4, 1, fp );
		if( nAttr == m_nMaxFrame )		// 프레임 속성 있는지 검사
		{
			// 프레임 속성 읽음.
			m_pAttr	= new MOTION_ATTR[ m_nMaxFrame ];
			fread( m_pAttr, sizeof(MOTION_ATTR) * m_nMaxFrame, 1, fp );
		}
	}
	

	fclose(fp);

	return SUCCESS;
}


//
//	Save
//
int		CObject3D::SaveObject( LPCTSTR szFileName )
{
	return SUCCESS;
}

//
//		GMOBJECT 부 읽가
//
int		CObject3D::LoadGMObject( FILE *fp, GMOBJECT *pObject )
{
	DWORD	dwTemp;
	int		i;
	
	fread( &pObject->m_vBBMin, sizeof(Vec3), 1, fp );
	fread( &pObject->m_vBBMax, sizeof(Vec3), 1, fp );
	
	fread( &pObject->m_bOpacity,    4, 1, fp );
	fread( &pObject->m_bBump, 4, 1, fp );
	fread( &pObject->m_bRigid,		4, 1, fp );
	fseek( fp, 28, SEEK_CUR );		// reserved

	// size of list
	fread( &pObject->m_nMaxVertexList,	4, 1, fp );		// 버텍스 개수
	fread( &pObject->m_nMaxVB,			4, 1, fp );		// 버텍스 버퍼 크기
	fread( &pObject->m_nMaxFaceList,	4, 1, fp );		// 페이스 개수
//	fread( &pObject->m_nMaxIB,		4, 1, fp );		// indexed 인덱스 버퍼개수

	m_nMaxFace += pObject->m_nMaxFaceList;

	// 버텍스 리스트를 할당하고 버텍스개수만큼 통째로 읽음
	// Vertex list
	if( pObject->m_Type == GMT_SKIN )	// 이런 LoadObject()에서 이미 읽어서 온것이다.
	{
		pObject->m_pVertexList	= new Vec3[ pObject->m_nMaxVertexList ];
		SKINVERTEX *pVB			= new SKINVERTEX[ pObject->m_nMaxVB ];	// Indexed Vertex buffer -
		pObject->m_pVB = pVB;
	} else
	{
		pObject->m_pVertexList	= new Vec3[ pObject->m_nMaxVertexList ];
		NORMALVERTEX *pVB		= new NORMALVERTEX[ pObject->m_nMaxVB ];	// Indexed Vertex buffer

		pObject->m_pVB = pVB;
	}
//	pObject->m_pIB  = new WORD[ pObject->m_nMaxIB + pObject->m_nMaxVB ];		// m_pIIB도 WORD형을  쓰므로 같이 할당해서 씀.
	pObject->m_pIIB  = new WORD[ pObject->m_nMaxVB ];		// m_pIIB도 WORD형을  쓰므로 같이 할당해서 씀.
//	pObject->m_pIIB = pObject->m_pIB + pObject->m_nMaxIB;

	fread( pObject->m_pVertexList, sizeof(Vec3) * pObject->m_nMaxVertexList, 1, fp );
	if( pObject->m_Type == GMT_SKIN )
		fread( pObject->m_pVB,  sizeof(SKINVERTEX) * pObject->m_nMaxVB, 1, fp );		// 버텍스 버퍼 읽음
	else
		fread( pObject->m_pVB,  sizeof(NORMALVERTEX) * pObject->m_nMaxVB, 1, fp );		// 버텍스 버퍼 읽음


//	fread( pObject->m_pIB,  sizeof(WORD) * pObject->m_nMaxIB, 1, fp );			// 인덱스 버퍼 읽음
	fread( pObject->m_pIIB, sizeof(WORD) * pObject->m_nMaxVB, 1, fp );

	fread( &dwTemp, 4, 1, fp );			// 피지크가 있는가 없는가
	if( dwTemp )
	{
		// Physique
		pObject->m_pPhysiqueVertex = new int[ pObject->m_nMaxVertexList ];
		fread( pObject->m_pPhysiqueVertex, sizeof(int) * pObject->m_nMaxVertexList, 1, fp );	// 버텍스개수와 같은 피지크데이타 읽음

		// m_pVB에는 world로 변환된 버텍스들이 들어가있게 되고
		// _pVB에는 뼈대 기준 로컬로 들어있게 된다.
		// 스키닝 오브젝트의 intersect등을 처리하려면 m_pVB를 써야 한다.
	}


	// Material
	D3DMATERIAL9	mMaterial;
	char			szBitmap[256];
	int				nLen;
	MATERIAL		*mMaterialAry[16];
	int				bIsMaterial;

	fread( &bIsMaterial, 4, 1, fp );		// ASE의 Main MaxMaterial을 저장했다.  이게 0이면 매터리얼이 없다는 것.
	pObject->m_bMaterial = bIsMaterial;				// 나중에 저장을 위해서 백업받아둔다.
	if( bIsMaterial )
	{
		memset( mMaterialAry, 0, sizeof(mMaterialAry) );
		
		fread( &pObject->m_nMaxMaterial, 4, 1, fp );				// 사용하는 매트리얼 개수 읽음

		if( pObject->m_nMaxMaterial == 0 )	pObject->m_nMaxMaterial = 1;	// CASEMesh의 Save부분을 참고할것.

		for( i = 0; i < pObject->m_nMaxMaterial; i ++ )
		{
			fread( &mMaterial, sizeof(D3DMATERIAL9), 1, fp );
			fread( &nLen, 4, 1, fp );		// bitmap filename length;  null 포함
#ifdef	__XDEBUG
			if( nLen > sizeof(szBitmap) )		XERROR( "CObject3D::LoadGMObject : %s 텍스쳐 파일명이 너무길다 : 길이 = %d", m_szFileName, nLen );
#endif
			fread( szBitmap, nLen, 1, fp );
			strlwr( szBitmap );		// 소문자로 변환
			
			pObject->m_MaterialAry[i].m_Material = mMaterial;
#ifdef	__XDEBUG
			if( strlen(szBitmap)+1 > sizeof(pObject->m_MaterialAry[i].strBitMapFileName) )
				XERROR( "CObject3D::LoadGeoMesh() : %s의 길이가 너무 길다", szBitmap );
#endif
			strcpy( pObject->m_MaterialAry[i].strBitMapFileName, szBitmap );
			if( IsEmpty(szBitmap) == FALSE )
				mMaterialAry[i] = g_TextureMng.AddMaterial( &mMaterial, szBitmap );
		}
	}

	// vertex buffer Material block
	fread( &pObject->m_nMaxMtrlBlk, 4, 1, fp );

	if( pObject->m_nMaxMtrlBlk >= 32 )
	{
#ifdef __XDEBUG
		XERROR( "매터리얼 갯수가 32개를 넘었다. %s", m_szFileName );
#endif
		return FAIL;
	}
	if( pObject->m_nMaxMtrlBlk > 0 )
	{
		pObject->m_pMtrlBlk = new MATERIAL_BLOCK[ pObject->m_nMaxMtrlBlk ];		// 매터리얼 블럭을 할당하고 읽음
		pObject->m_pGL_MtrlBlkTexture = new GLuint[ pObject->m_nMaxMtrlBlk * 8 ];	// 확장텍스쳐(최대8개)랑 같이 쓴다. 없으면 걍 널이다.
		memset( pObject->m_pGL_MtrlBlkTexture, 0, sizeof(GLuint) * (pObject->m_nMaxMtrlBlk * 8) );
		memset( pObject->m_pMtrlBlk, 0, sizeof( MATERIAL_BLOCK ) * pObject->m_nMaxMtrlBlk );
//		fread( pObject->m_pMtrlBlk, sizeof(MATERIAL_BLOCK) * pObject->m_nMaxMtrlBlk, 1, fp );
		for( i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			fread( &pObject->m_pMtrlBlk[i].m_nPrimitiveCount, 4, 1, fp );
			pObject->m_pMtrlBlk[i].m_pIB = new WORD[ pObject->m_pMtrlBlk[i].m_nPrimitiveCount * 3 ];
			fread( pObject->m_pMtrlBlk[i].m_pIB, sizeof(WORD) * pObject->m_pMtrlBlk[i].m_nPrimitiveCount * 3, 1, fp );
			fread( &pObject->m_pMtrlBlk[i].m_nTextureID, 4, 1, fp );
			fread( &pObject->m_pMtrlBlk[i].m_dwEffect, 4, 1, fp );
			fread( &pObject->m_pMtrlBlk[i].m_nAmount, 4, 1, fp );
			fread( &pObject->m_pMtrlBlk[i].m_nMaxUseBone, 4, 1, fp );
			fread( pObject->m_pMtrlBlk[i].m_UseBone, sizeof(int) * MAX_VS_BONE, 1, fp );
		}
		
		for( i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			if( bIsMaterial )
			{
				if( mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ] )
				{
					pObject->m_pGL_MtrlBlkTexture[i] = mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->m_nGL_Texture;
						
				}
			}
		}
	}

	return SUCCESS;
}

//
//	TM Animation data load
//
int		CObject3D::LoadTMAni( FILE *fp, GMOBJECT *pObject )
{
	int	bFrame;
	fread( &bFrame, 4, 1, fp );
	if( bFrame == 0 )	return FAIL;	// 프레임 없으면 읽지 않음.

	pObject->m_pFrame = new TM_ANIMATION[ m_nMaxFrame ];
	fread( pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1, fp );

	return SUCCESS;
} 

//
// 메쉬내에서 szSrc텍스쳐를 szDest텍스쳐로 바꾼다.
//
//
void	CObject3D::ChangeTexture( LPCTSTR szSrc, LPCTSTR szDest )
{
	int		i, j, k;
	GMOBJECT	*pObject;
	char	szBuff[128];

	strcpy( szBuff, szSrc );
	strlwr( szBuff );
	TCHAR szBitMapFileName[ 16 ][ 128 ];
	for( k = 0; k < MAX_GROUP; k++ )
	{
		for( i = 0; i < m_Group[k].m_nMaxObject; i ++ )
		{
			pObject = &m_Group[k].m_pObject[i];

			// 파일명 비교를 위해서 복사해 놓는다. 교체 처리후 
			// pObject->m_MaterialAry[ nID ].strBitMapFileName의 내용이 변경되기 때문에 오리지날 스트링을
			// 비교를 위해 임시 저장해둘 필요가 있다.
			for( j = 0; j < pObject->m_nMaxMtrlBlk; j ++ )		// 매터리얼 블럭을 돌면서
			{
				int nID = pObject->m_pMtrlBlk[j].m_nTextureID;
				_tcscpy( szBitMapFileName[ j ], pObject->m_MaterialAry[ nID ].strBitMapFileName );
			}
			for( j = 0; j < pObject->m_nMaxMtrlBlk; j ++ )		// 매터리얼 블럭을 돌면서
			{
				int nID = pObject->m_pMtrlBlk[j].m_nTextureID;
				if( strcmp( szBitMapFileName[j], szBuff ) == 0 )	// szSrc랑 같은 파일명이 있으면
				{
					MATERIAL	*pMtrl;
					D3DMATERIAL9	mMtrl;
					pMtrl = g_TextureMng.AddMaterial( &mMtrl, szDest );		// szDest로 읽어서
					pObject->m_pGL_MtrlBlkTexture[j] = pMtrl->m_nGL_Texture;	// 그놈으로 대체시키고
					strcpy( pObject->m_MaterialAry[ nID ].strBitMapFileName, szDest );	// 파일명 바꿔놓는다.
				}
			}
		}
	} // LOD_GROUP
}

//
//	GMOBJECT부 저장
//
int		CObject3D::SaveGMObject( FILE *fp, GMOBJECT *pObject )
{

	return SUCCESS;
}



//
//
//
int		CObject3D::SaveTMAni( FILE *fp, GMOBJECT *pObject )
{
	int		bFrame;
	if( pObject->m_pFrame )	bFrame = 1;
	else					bFrame = 0;
	fwrite( &bFrame, 4, 1, fp );
	if( bFrame == 0 )	return FAIL;	// 프레임 없으면 쓰지 않음.

	if( m_nMaxFrame > 0 )
	{
		fwrite( pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1, fp );
	}

	return SUCCESS;
}

static int	_nSlideCnt = 0;

//
// Line과 교차하는 삼각형을 찾아 Slide벡터를 계산.
// 주의 : SlideVectorXZ는 스키닝 오브젝트에는 사용금지다.
// XZ방향으로 이동하는 벡터에만 사용하는 것으로 일반적인 상황엔 SlideVector()를 써야 한다.
// bCollObj : 충돌메시로 검사하는가? (디폴트)
//
int		CObject3D::SlideVectorXZ( Vec3 *pOut, const Vec3 &vPos, const Vec3 &vEnd , const MATRIX &mWorld, BOOL bCollObj )
{
	Vec3	*v1, *v2, *v3;
	Vec3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	Vec3 vA, vB, vN, vTemp;
	MATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;		// 충돌메시로 충돌검사.
//	GMOBJECT* pObject = m_Group[0].m_pObject;
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// 충돌메시로 충돌검사 하지마라.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		return FAIL;
	}

	//	if( _mUpdate == NULL )		return FAIL;
	
	if( pObject->m_Type == GMT_ERROR )		return 0;

	for( i = 0; i < nMaxObj; i++ )
	{
#ifdef	__XDEBUG
		if( pObject->m_Type == GMT_SKIN )	XERROR( "SlideVectorXZ:스킨오브젝트 발견. 에러%s", m_szFileName ); 
#endif
		nMaxFace = pObject->m_nMaxFaceList;
//		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		if( bCollObj == FALSE )
			MatrixMultiply( mTM, pGroup->_mUpdate[i], mWorld );
		else
			mTM = mWorld;
//		mTM = m_Group[0].m_pObject[0].m_mLocalTM * mWorld;
//		mTM = _mUpdate[i] * mWorld;

		pObject++;
		MatrixInverse( mInv, mTM );
		MatrixVec3Multiply( vInvPos, vPos, mInv );	// Line시작점을 Geometry기준으로 변환
		MatrixVec3Multiply( vInvEnd, vEnd, mInv );	// Line끝점을 역시 변환
		vInvDir = vInvEnd - vInvPos;
		for( int i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			//		for( j = 0; j < nMaxFace; j++ )
			MATERIAL_BLOCK *pBlock = &pObject->m_pMtrlBlk[i];
			pIB = pBlock->m_pIB;
			for( j = 0; j < pBlock->m_nPrimitiveCount; j ++ )
			{
				v1 = &pVB[ *pIB++ ].position;
				v2 = &pVB[ *pIB++ ].position;
				v3 = &pVB[ *pIB++ ].position;

	//			FLOAT fU, fV;
	//			bRet = D3DXIntersectTri( v1, v2, v3, &vInvPos, &vInvDir, &fU, &fV, &fDist );
				bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );

				if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
				{
					// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
					// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
					if( fDist < 1.0f )
					{
						// Line과 닿은 삼각형을 찾아냈다.
						vDir = vEnd - vPos;
						fDist *= 0.5f;
						vIntersect = vPos + fDist * vDir;	// 교차점을 계산.

						vA = *v2 - *v1;
						vB = *v3 - *v1;
						MatrixVec3CrossProduct( vN, vA, vB );		// 충돌한 면의 노말 구함. 이건 나중에 미리 계산해두자.
						mInv = mTM;
						mInv.f[_41] = mInv.f[_42] = mInv.f[_43] = 0;
						MatrixVec3Multiply( vN, vN, mInv );	// 충돌한면의 노말을 원래대로(mWorld)돌림.
	#ifdef	__XDEBUG
						if( vN.x == 0 && vN.z == 0 )
							XLOG( "CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다" );
	#endif
						vN.y = 0;	// y성분을 없애서 수직면의 법선인것처럼 변환
						MatrixVec3Normalize( vN, vN );		// 최종 단위벡터로 변환
						
						// 자, 이제 vIntersect와 vN은 월드좌표계로 준비가 되었다.
						vTemp = vEnd - vIntersect;		// 교차점 - 라인끝 vector V라고 칭함
						CalcSlideVec( &vTemp, vTemp, vN );
	//					vTemp += vIntersect;
	//					*pOut = vTemp - vPos;
						*pOut = vTemp;

						vTemp += vPos;
						
						if( ++_nSlideCnt < 3 )  // 무한 리커전 방지
							SlideVectorXZ( pOut, vPos, vTemp, mWorld );		// 이거왜 시작점이 vPos일까나 -.-;;;
						
						_nSlideCnt = 0;
						return 1;
					}
				}
			}
		}
	}
	_nSlideCnt = 0;
	return 0;
}

//
// 이 함수 자체로 리커시브콜을 하지 않는 버전.
//
int		CObject3D::SlideVectorXZ2( Vec3 *pOut, Vec3 *pIntersect, const Vec3 &vPos, const Vec3 &vEnd , const MATRIX &mWorld, BOOL bCollObj )
{
	Vec3	*v1, *v2, *v3;
	Vec3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	Vec3 vA, vB, vN, vTemp;
	MATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;		// 충돌메시로 충돌검사.
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// 충돌메시로 충돌검사 하지마라.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		return FAIL;
	}

	
	if( pObject->m_Type == GMT_ERROR )		return 0;

	for( i = 0; i < nMaxObj; i++ )
	{
#ifdef	__XDEBUG
		if( pObject->m_Type == GMT_SKIN )	XERROR( "SlideVectorXZ:스킨오브젝트 발견. 에러%s", m_szFileName ); 
#endif
		nMaxFace = pObject->m_nMaxFaceList;
//		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		if( bCollObj == FALSE )
			MatrixMultiply( mTM, pGroup->_mUpdate[i], mWorld );
		else
			mTM = mWorld;

		pObject++;
		MatrixInverse( mInv, mTM );
		MatrixVec3Multiply( vInvPos, vPos, mInv );	// Line시작점을 Geometry기준으로 변환
		MatrixVec3Multiply( vInvEnd, vEnd, mInv );	// Line끝점을 역시 변환
		vInvDir = vInvEnd - vInvPos;
		for( int i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			MATERIAL_BLOCK *pBlock = &pObject->m_pMtrlBlk[i];
			pIB = pBlock->m_pIB;
			for( j = 0; j < pBlock->m_nPrimitiveCount; j ++ )
			{
				v1 = &pVB[ *pIB++ ].position;
				v2 = &pVB[ *pIB++ ].position;
				v3 = &pVB[ *pIB++ ].position;

				bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );

				if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
				{
					// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
					// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
					if( fDist < 1.0f )
					{
						// Line과 닿은 삼각형을 찾아냈다.
						vDir = vEnd - vPos;
	//					fDist *= 0.5f;
						vIntersect = vPos + fDist * vDir;	// 교차점을 계산.

						vA = *v2 - *v1;
						vB = *v3 - *v1;
						MatrixVec3CrossProduct( vN, vA, vB );		// 충돌한 면의 노말 구함. 이건 나중에 미리 계산해두자.
						mInv = mTM;
						mInv.f[_41] = mInv.f[_42] = mInv.f[_43] = 0;
						MatrixVec3Multiply( vN, vN, mInv );	// 충돌한면의 노말을 원래대로(mWorld)돌림.
	#ifdef	__XDEBUG
						if( vN.x == 0 && vN.z == 0 )
							XLOG( "CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다" );
	#endif
						vN.y = 0;	// y성분을 없애서 수직면의 법선인것처럼 변환
						MatrixVec3Normalize( vN, vN );		// 최종 단위벡터로 변환
						
						// 자, 이제 vIntersect와 vN은 월드좌표계로 준비가 되었다.
						vTemp = vEnd - vIntersect;		// 교차점 - 라인끝 vector V라고 칭함
						*pIntersect = vIntersect;		// 교차점을 받아둠
						CalcSlideVec( &vTemp, vTemp, vN );
						
						*pOut = vTemp;		// 미끄러진 벡터를 결과로 받도록 바꿔보자.
	//					*pOut = vTemp;
						
						return 1;
					}
				}
			}
		}
	}
	return 0;
}


//
// Line과 교차하는 삼각형을 찾아 Slide벡터를 계산.
// 주의 : SlideVector는 스키닝 오브젝트에는 사용금지다.
// 중력벡터의 바닥 미끄러짐계산시 사용하는 전용함수.
// 리커전을 하지 않는다.  - 속도땜에 -
//
int		CObject3D::SlideVectorUnder( Vec3 *pOut, const Vec3 &vPos, const Vec3 &vEnd , const MATRIX &mWorld, Vec3 *pIntersect )
{
	Vec3	*v1, *v2, *v3;
	Vec3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	Vec3 vA, vB, vN, vTemp;
	MATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
//	GMOBJECT* pObject = m_Group[0].m_pObject;
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

//	if( _mUpdate == NULL )		return FAIL;

	if( pObject->m_Type == GMT_ERROR )
		return 0;

//	for( i = 0; i < nMaxObj; i++ )
	{
#ifdef	__XDEBUG
		if( pObject->m_Type == GMT_SKIN )	XERROR( "SlideVectorUnder:스킨오브젝트 발견. 에러%s", m_szFileName ); 
#endif
		nMaxFace = pObject->m_nMaxFaceList;
//		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		pObject++;
		mTM = mWorld;		// 툴에선 LocalTM을 쓰지 않는다.
//		mTM = m_Group[0].m_pObject[0].m_mLocalTM * mWorld;

		MatrixInverse( mInv, mTM );
		MatrixVec3Multiply( vInvPos, vPos, mInv );	// Line시작점을 Geometry기준으로 변환
		MatrixVec3Multiply( vInvEnd, vEnd, mInv );	// Line끝점을 역시 변환
		vInvDir = vInvEnd - vInvPos;
		for( int i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			//		for( j = 0; j < nMaxFace; j++ )
			MATERIAL_BLOCK *pBlock = &pObject->m_pMtrlBlk[i];
			pIB = pBlock->m_pIB;
			for( j = 0; j < pBlock->m_nPrimitiveCount; j ++ )
			{
				v1 = &pVB[ *pIB++ ].position;
				v2 = &pVB[ *pIB++ ].position;
				v3 = &pVB[ *pIB++ ].position;

				bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );

				if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
				{
					// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
					// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
					if( fDist < 1.0f )
					{
						// Line과 닿은 삼각형을 찾아냈다.
						vDir = vEnd - vPos;
						*pIntersect = vPos + fDist * vDir;	// 교차점을 계산.
		
						vA = *v2 - *v1;		// 노말 계산 시작.
						vB = *v3 - *v1;
						MatrixVec3CrossProduct( vTemp, vA, vB );		// 충돌한 면의 노말 구함. 이건 나중에 미리 계산해두자.
						MatrixVec3Normalize( vN, vTemp );		// 단위벡터로 변환

						mInv = mTM;
						mInv.f[_41] = mInv.f[_42] = mInv.f[_43] = 0;
						MatrixVec3Multiply( vTemp, vN, mInv );	// 충돌한면의 노말을 원래대로(mWorld)돌림.
						vN = vTemp;		// 충돌면 노말.
	//					D3DXVec3TransformCoord( pIntersect, &vIntersect, &mTM );	// 충돌지점을 mTM적용시켜서 리턴.
						vDir.x = vDir.z = 0;	vDir.y = -1.0f;
						if( MatrixVec3DotProduct( vN, vDir ) < -0.19080f )	// 일정각 이상 기울어진 면만 슬라이드를 적용시킨다. 두벡터다 노말이어야 한다.
							return 1;
						vDir.y = -0.01f;
	//					D3DXVec3TransformCoord( &vDir, &vDir, &mInv );	// 면안쪽으로 박힌 벡터를 적당히 하나 만든다.

						// 자, 이제 vIntersect와 vN은 월드좌표계로 준비가 되었다.
	//					vTemp = vEnd - vIntersect;		// 교차점 - 라인끝, 삼각형속에 박힌부분의 벡터만 구함.
						CalcSlideVec( &vTemp, vDir, vN );	// 충돌점에서 미끄러진부분까지의 벡터
	//					vTemp += vIntersect;		// 미끄러진부분의 절대좌표.
	//					*pOut = vTemp - vPos;		// 미끌절대좌표에서 원점빼서 원점에서 미끄러짐까지의 벡터구함.
	//					*pOut = vTemp;		// 충돌점에서 미끌좌표까지의 벡터만 구할때...
						*pOut = *pIntersect + vTemp;
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

//
// Line과 교차하는 삼각형을 찾아 리턴
// pTriOut : Vec3 *pTri[3]; 의 시작 포인터.
// 주의 : 스키닝 오브젝트에는 사용 금지.
//
void CObject3D::FindTouchTriLine( Vec3 **pTriOut, const Vec3 &vPos, const Vec3 &vEnd , const MATRIX &mWorld, FLOAT *pDist, BOOL bCollObj )
{
	Vec3	*v1, *v2, *v3;
	Vec3	vInvPos, vInvEnd, vInvDir, vDir;
	Vec3 vA, vB, vN, vTemp;
	MATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
	NORMALVERTEX *pVB;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// 충돌메시로 충돌검사 하지마라.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		return;
	}
	
	if( pObject->m_Type == GMT_ERROR )
	{
		*pTriOut = NULL;
		return;
	}

	WORD	*pIB;
	int		j;
	BOOL	bRet;

	for( i = 0; i < nMaxObj; i++ )
	{
#ifdef	__XDEBUG
		if( pObject->m_Type == GMT_SKIN )	XERROR( "FindTouchTriLine:스킨오브젝트 발견. 에러%s", m_szFileName );
#endif
		nMaxFace = pObject->m_nMaxFaceList;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
//		pIB	= pObject->m_pIB;
		pObject++;
		if( bCollObj == FALSE )
			MatrixMultiply( mTM, pGroup->_mUpdate[i], mWorld );
		else
			mTM = mWorld;		// 충돌용 메시에는 LocalTM이 없다.

		MatrixInverse( mInv, mTM );
		MatrixVec3Multiply( vInvPos, vPos, mInv );	// Line시작점을 Geometry기준으로 변환
		MatrixVec3Multiply( vInvEnd, vEnd, mInv );	// Line끝점을 역시 변환
		vInvDir = vInvEnd - vInvPos;
		for( int i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			//		for( j = 0; j < nMaxFace; j++ )
			MATERIAL_BLOCK *pBlock = &pObject->m_pMtrlBlk[i];
			pIB = pBlock->m_pIB;
			for( j = 0; j < pBlock->m_nPrimitiveCount; j ++ )
			{
				v1 = &pVB[ *pIB++ ].position;
				v2 = &pVB[ *pIB++ ].position;
				v3 = &pVB[ *pIB++ ].position;

				bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, pDist );

				if( bRet && *pDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
				{
					// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
					// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
					if( *pDist < 1.0f )
					{
						// Line과 닿은 삼각형을 찾아냈다.
						// 이 좌표들 mTM으로 곱해서 넘겨야 하는거 아닌가?.
						pTriOut[0] = v1;
						pTriOut[1] = v2;
						pTriOut[2] = v3;
						return;
					}
				}
			}
		}
	}

	*pTriOut = NULL;
}

//
//	주어진 ray와 메쉬의 각 vertex AABB의 교차를 검사한다.
//  찾은 버텍스는 pOut에 담겨지고
//  실제 찾은 버텍스의 포인터는 리턴된다.
//
Vec3 *CObject3D::IntersecRayVertex( Vec3 *pOut, const Vec3 &vRayOrig, const Vec3 &vRayDir )
{
	int		i, j;
	GMOBJECT	*pObject = m_pGroup->m_pObject;
	int		nMax;
	Vec3	*pVertex, v;
	
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
	{
		nMax = pObject->m_nMaxVertexList;
		pVertex = pObject->m_pVertexList;
		for( j = 0; j < nMax; j ++ )
		{
			if( m_pGroup->_mUpdate )
			{
				MatrixVec3Multiply( v, *pVertex, m_pGroup->_mUpdate[i] );		// v = vertex * WorldTM
				if( IsTouchRayAABB( vRayOrig, vRayDir, v, Vec3(0.025f, 0.025f, 0.025f)) )
				{
					*pOut = v;
					return pVertex;
				}
			}
			pVertex ++;
		}
		pObject ++;
	}
	
	return NULL;
}


// intersectRayTri에서 교차한 삼각형 데이타가 이곳으로 임시로 들어온다.
// 용량이 큰 이유는 스킨오브젝트는 뼈대변환된 버텍스를 다시 계산 해야하므로 그 변환된 버텍스가 vPool로 들어온다.
static Vec3	_vPool[4096];
static Vec3	s_vLastPickTri[3];

Vec3 *GetLastPickTri( void )
{
	return s_vLastPickTri;
}

//
// 레이와 교차한 삼각형의 시작포인터를 리턴.
//
Vec3 *CObject3D::IntersectRayTri( const Vec3 &vRayOrig, const Vec3 &vRayDir, const MATRIX &mWorld, 
											  Vec3* pvIntersect, FLOAT* pfDist, BOOL bColl )
{
	Vec3	*v1, *v2, *v3, *v4;
	Vec3	vInvRayOrig, vInvRayDir, vw1, vw2;
	MATRIX	mTM, mInv, *pBone, *pBoneInv;
	int		nMax;
	int		nMaxObj = m_Group[0].m_nMaxObject;
	GMOBJECT* pObj = &m_Group[0].m_pObject[0];
	WORD		*pIB, *pIIB;
	int		i, j, nVIdx;
	FLOAT	fDist, fMinDist = 65535.0f;
	BOOL	bRet, bFlag = FALSE;

	if( bColl )		// 충돌메시로 검사하게 하는 옵션
	{
		if( m_CollObject.m_Type != GMT_ERROR )	// 충돌메시가 없으면 걍 함.
		{
			pObj = &m_CollObject;
			nMaxObj = 1;	// 충돌메시는 무조건 1개다.
		}
	}

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObj->m_Type == GMT_SKIN )	
		{
			SKINVERTEX *pVB;
			mTM = /*_mUpdate[i] * */mWorld;
			MatrixInverse( mInv, mTM );
			MatrixVec3Multiply( vInvRayOrig, vRayOrig, mInv );
			
			mInv.f[_41] = 0;	mInv.f[_42] = 0;	mInv.f[_43] = 0;	// vRayDir은 방향벡터만 있기땀시 위치는 없애준다.
			MatrixVec3Multiply( vInvRayDir, vRayDir, mInv );
			int *pPhysique = pObj->m_pPhysiqueVertex;
			Vec3	*pVList = pObj->m_pVertexList;
//			pIB	= pObj->m_pIB;
			pIIB = pObj->m_pIIB;
			pVB = (SKINVERTEX *)pObj->m_pVB;
			if( m_pmExternBone )	
			{
				pBone = m_pmExternBone;		// 외부지정 뼈대가 있다면 그걸 쓰고
				pBoneInv = m_pmExternBoneInv;
			}
			else					
			{
				pBone = m_pBaseBone;		// 없다면 디폴트를 쓴다.
				pBoneInv = m_pBaseBoneInv;
			}
			nMax = pObj->m_nMaxVB;
#ifdef	__XDEBUG
			if( nMax > sizeof(_vPool) / sizeof(Vec3) )
				XERROR( "IntersectRayTri : 버텍스버퍼의 개수가 너무 크다 %d", nMax );
#endif
			int	matIdx;
			v4 = _vPool;
			for( j = 0; j < nMax; j ++ )
			{
				nVIdx = *pIIB++;
//				D3DXVec3TransformCoord( v4, &pVList[nVIdx], &pBone[ pPhysique[nVIdx] ] );
				matIdx = pPhysique[ nVIdx ];	// 2link는 무시하고 그중 하나만 쓴다. 그래서 계산이 정확하지 않을 수 있다,.
				MatrixMultiply( mTM, pBoneInv[ matIdx ], pBone[ matIdx ] );
				MatrixVec3Multiply( *v4, pVB[j].position, mTM );
				v4 ++;
			}
			v4 = _vPool;	// pVB
			for( int m = 0; m < pObj->m_nMaxMtrlBlk; m ++ )
			{
				MATERIAL_BLOCK *pBlock = &pObj->m_pMtrlBlk[m];
				nMax = pBlock->m_nPrimitiveCount;
				pIB = pBlock->m_pIB;
				for( j = 0; j < nMax; j++ )
				{
					v1 = &v4[ *pIB++ ];
					v2 = &v4[ *pIB++ ];
					v3 = &v4[ *pIB++ ];

	//#ifdef __BEAST
					// 셈플 소스코드를 이용한 피킹에 버그 있음. 못집는 오브젝트가 있어서 API를 사용해서 피킹함.
	//				FLOAT fU, fV;
	//				bRet = D3DXIntersectTri( v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fU, &fV, &fDist );
	//#else
					bRet = IsTouchRayTri( v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist );
	//#endif
					if( bRet && fDist >= 0.0f )	// 반대방향 면(-)은 검사하지 않음.
					{
						if( fDist < fMinDist )	// 레이원점으로부터 가장 가까운 거리를 찾음.
						{
							fMinDist = fDist;
							bFlag = TRUE;
	#ifdef	__COLA
	//						D3DXVec3TransformCoord( &_vPool[0], v1, &_mUpdate[i] );
	//						D3DXVec3TransformCoord( &_vPool[1], v2, &_mUpdate[i] );
	//						D3DXVec3TransformCoord( &_vPool[2], v3, &_mUpdate[i] );
							_vPool[0] = *v1;	_vPool[1] = *v2;	_vPool[2] = *v3;
	#endif
							s_vLastPickTri[0] = *v1;	s_vLastPickTri[1] = *v2;	s_vLastPickTri[2] = *v3;
						}
					}
				}
			}
		} else
		{  // non-skin object
			NORMALVERTEX *pVB;
			MatrixMultiply( mTM, m_Group[0]._mUpdate[i], mWorld );
			MatrixInverse( mInv, mTM );
			MatrixVec3Multiply( vInvRayOrig, vRayOrig, mInv );
			
			mInv.f[_41] = 0;	mInv.f[_42] = 0;	mInv.f[_43] = 0;	// vRayDir은 방향벡터만 있기땀시 위치는 없애준다.
			MatrixVec3Multiply( vInvRayDir, vRayDir, mInv );
			pVB	= (NORMALVERTEX*)(pObj->m_pVB);
//			pIB	= pObj->m_pIB;
			for( int m = 0; m < pObj->m_nMaxMtrlBlk; m ++ )
			{
				MATERIAL_BLOCK *pBlock = &pObj->m_pMtrlBlk[m];
				nMax = pBlock->m_nPrimitiveCount;
				pIB = pBlock->m_pIB;
				//				nMax = pObj->m_nMaxFaceList;
				for( j = 0; j < nMax; j++ )
				{
					v1 = &pVB[ *pIB++ ].position;
					v2 = &pVB[ *pIB++ ].position;
					v3 = &pVB[ *pIB++ ].position;

					bRet = IsTouchRayTri( v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist );
					//bRet = IntersectTriangle( v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist );
					if( bRet && fDist >= 0.0f )	// 반대방향 면(-)은 검사하지 않음.
					{
						if( fDist < fMinDist )	// 레이원점으로부터 가장 가까운 거리를 찾음.
						{
							fMinDist = fDist;
							bFlag = TRUE;
							MatrixVec3Multiply( s_vLastPickTri[0], *v1, m_Group[0]._mUpdate[i] );
							MatrixVec3Multiply( s_vLastPickTri[1], *v2, m_Group[0]._mUpdate[i] );
							MatrixVec3Multiply( s_vLastPickTri[2], *v3, m_Group[0]._mUpdate[i] );
							
						}
					}
				}
			}
		}
		pObj++;

	}
	if( bFlag )	// 한번이라도 교차된 삼각형을 찾았다면.
	{
/*
    t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    t *= fInvDet;
    u *= fInvDet;
    v *= fInvDet;

	*pIntersect = orig + t * dir;
*/

		*pvIntersect = vRayOrig + fMinDist * vRayDir;		// 교차했던 가장 가까운거리로 충돌지점 계산.
		return _vPool;		// 그 삼각형 좌표 리턴
	}
	
	return NULL;
}


//
//	this의 OBB(AABB + mWorld)와 주어진 레이의 교차 검사
//
BOOL	CObject3D::IsTouchOBB_Line( const Vec3 &vStart, const Vec3 &vEnd, 
									const MATRIX &mWorld, Vec3* pvIntersect )
{
	MATRIX	mTM;
	int		nMaxObj = m_pGroup->m_nMaxObject;

	if( ::IsTouchOBB_Line( m_vBBMin, m_vBBMax, mWorld, vStart, vEnd, pvIntersect ) == TRUE )
	{
		return TRUE;
	}
	return FALSE;
}


void	CObject3D::SetTexture( LPCTSTR szTexture )
{
	MATERIAL	*pMtrl;
	D3DMATERIAL9	mMtrl;

	pMtrl = g_TextureMng.AddMaterial( &mMtrl, szTexture );
	m_Group[0].m_pObject[0].m_pGL_MtrlBlkTexture[0] = pMtrl->m_nGL_Texture;
}

void	CObject3D::SetTexture( GLuint nGLTexture )
{
	m_Group[0].m_pObject[0].m_pGL_MtrlBlkTexture[0] = nGLTexture;
}

void	CObject3D::LoadTextureEx( int nNumEx, GMOBJECT *pObj, MATERIAL *pmMaterial[16] )
{
	int		i;
	char	szTexture[ 256 ];
	char	szNum[16];
	D3DMATERIAL9	mMaterial;

	memset( &mMaterial, 0, sizeof(mMaterial) );

	// 사용하는 텍스쳐에 -et를 붙여서 확장 텍스쳐를 읽어들임.
	for( i = 0; i < pObj->m_nMaxMaterial; i ++ )
	{
		if( IsEmpty( pObj->m_MaterialAry[i].strBitMapFileName ) )	continue;
		strcpy( szTexture, pObj->m_MaterialAry[i].strBitMapFileName );		// mvr_bang.dds
		strcpy( szTexture, ::GetFileTitle( pObj->m_MaterialAry[i].strBitMapFileName ) );	// mvr_bang
		strcat( szTexture, "-et" );		// -et. 를 붙임.  mvr_bang-et.
		sprintf( szNum, "%02d.", nNumEx );	
		strcat( szTexture, szNum );			// -et01 ~ 07
		strcat( szTexture, ::GetFileExt( pObj->m_MaterialAry[i].strBitMapFileName ) );		// mvr_bang-et.dds


		if( IsEmpty(szTexture) == FALSE )
			pmMaterial[i] = g_TextureMng.AddMaterial( &mMaterial, szTexture );
	}
}

// 텍스쳐를 확장텍스쳐로 지정한다.
// 기본은 확작텍스쳐를 로딩하지 않지만 요청이 들어오면
// 텍스쳐를 로딩하고 그포인트로 기본포인터로 바꾼다.
void	CObject3D::SetTextureEx( GMOBJECT *pObj, int nNumEx )
{
	GLuint	*pnGLTextureEx;		// 매터리얼 블럭내 텍스쳐포인터
	int		i;
	MATERIAL	*mMaterial[16];
	int		nID;
	
	if( nNumEx >= 8 )
	{
		XLOG( "CObject3D::SetTextureEx : nNumEx = %d", nNumEx );
		return;
	}
	pnGLTextureEx = pObj->m_pGL_MtrlBlkTexture + (pObj->m_nMaxMtrlBlk * nNumEx);		// 확장부분 포인터.
	
//	if( bUse )
	if( 1 )
	{
		memset( mMaterial, 0, sizeof(mMaterial) );
		
		if( pnGLTextureEx[0] == 0 )		// 확장 텍스쳐가 로딩된적이 없다.
		{
			LoadTextureEx( nNumEx, pObj, mMaterial );		// 확장텍스쳐를 로딩함.
			for( i = 0; i < pObj->m_nMaxMtrlBlk; i ++ )
			{
				nID = pObj->m_pMtrlBlk[i].m_nTextureID;
				if( mMaterial[ nID ] )
					pnGLTextureEx[i] = mMaterial[ nID ]->m_nGL_Texture;		// 확장텍스쳐를 로딩함.
			}
		}
	}
	else
	{
		for( i = 0; i < pObj->m_nMaxMtrlBlk; i ++ )
			pnGLTextureEx[i] = NULL;
	}
	
	m_nTextureEx = nNumEx;
}

void	CObject3D::ClearNormal( void )
{
/*	int		i;
	Vec3		n = Vec3( 0.0f, 1.0f, 0.0f );

	for( int j = 0; j < m_nMaxObject; j++ )
	{
		GMOBJECT* pObject = &m_pObject[j];
		if( pObject->m_Type == GMT_SKIN )
		{
			SKINVERTEX *pVB = (SKINVERTEX*)(pObject->m_pVB);
			for( i = 0; i < pObject->m_nMaxVB; i ++ )		pVB[i].normal = n;
		}
		else
		{
			NORMALVERTEX *pVB = (NORMALVERTEX*)(pObject->m_pVB);
			for( i = 0; i < pObject->m_nMaxVB; i ++ )		pVB[i].normal = n;
		}

		SendVertexBuffer( pObject, NULL );
	}
	*/
}

//
// Animate()수행후 곧바로 Render를 호출해야 한다.
// 임시버퍼가 손상될 수 있기 때문이다.
//
// 스킨 
//
void	CObject3D::Animate( FLOAT fFrameCurrent, int nNextFrame )
{
	int		i;
	TM_ANIMATION	*pFrame = NULL;					// 현재 프레임
	TM_ANIMATION	*pNext = NULL;					// 다음 프레임
	QUATERNION	qSlerp;
	Vec3		vSlerp;
	int		nCurrFrame;
	float	fSlp;
	MATRIX	m1, m2;
	MATRIX *mUpdate = m_pGroup->_mUpdate;
	MATRIX *mParent;

	GMOBJECT	*pObj;
	int nMaxObj = m_pGroup->m_nMaxObject;
	for( i = 0; i < nMaxObj; i ++ )
	{
		pObj = &m_pGroup->m_pObject[i];
		if( pObj->m_ParentType == GMT_BONE )
		{
			if( m_pmExternBone )	mParent = m_pmExternBone;
			else					mParent = m_pBaseBone;
		} else
			mParent = m_pGroup->_mUpdate;
		if( m_nMaxFrame > 0 )	// 프레임이 있을때
		{
			if( pObj->m_pFrame )
			{
				// 보간을 위한 Slerp 계산.
				nCurrFrame = (int)fFrameCurrent;			// 소숫점 떼내고 정수부만..
				fSlp = fFrameCurrent - (float)nCurrFrame;	// 소숫점 부분만 가져옴

				pFrame = &pObj->m_pFrame[ nCurrFrame ];		// 현재 프레임 포인터 받음
				pNext  = &pObj->m_pFrame[ nNextFrame ];		// 다음 프레임 포인터 받음

				MatrixQuaternionSlerp( qSlerp, pFrame->m_qRot, pNext->m_qRot, fSlp );		// 회전 보간
				MatrixVec3Lerp( vSlerp, pFrame->m_vPos, pNext->m_vPos, fSlp );					// 벡터 보간

				// matAniTM계산
				// 이동행렬 만듦
				MatrixTranslation( m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
			
				// 쿼터니온 보간된 회전키값을 회전행렬로 만듦
				MatrixRotationQuaternion( m2, qSlerp );
				MatrixMultiply( mUpdate[i], m2, m1 );		// 이동행렬 X 회전행렬 = 애니행렬
				if( pObj->m_nParentIdx != -1 )
					MatrixMultiply( mUpdate[i], mUpdate[i], mParent[ pObj->m_nParentIdx ] );
//				else
//					mUpdate[i] *= *mCenter;		// 루트는 센터랑 최초 곱함.
					
			} else
			// 이 오브젝트에 프레임이 없을때
			{
				if( pObj->m_nParentIdx != -1 )	// 부모가 있는 경우
					MatrixMultiply( mUpdate[i], pObj->m_mLocalTM, mParent[ pObj->m_nParentIdx ] );
				else
					mUpdate[i] = pObj->m_mLocalTM;					// 부모가 없다면 루트다.
//					mUpdate[i] = pObj->m_mLocalTM * *mCenter;		// 부모가 없다면 루트다. 루트는 센터랑 최초 곱함.
			}
		} else
		// 프레임이 없을때.
		{
			if( pObj->m_nParentIdx != -1 )
				MatrixMultiply( mUpdate[i], pObj->m_mLocalTM, mParent[ pObj->m_nParentIdx ] );
			else
				mUpdate[i] = pObj->m_mLocalTM;		// 부모가 없다면 루트다. 
//				mUpdate[i] = pObj->m_mLocalTM * *mCenter;		// 부모가 없다면 루트다. 루트는 센터랑 최초 곱함.
		}

		// mUpdate[i]  최종 WorldTM
	}

}

//
// 스키닝.
// 본의 애니메이션이 끝난후 뼈대와 로컬버텍스를 곱하여 최종 버텍스좌표를 계산한다.
// mBones : 계산이 끝난 뼈대들의 매트릭스 
//
void	CObject3D::Skining( GMOBJECT *pObj, const MATRIX *mBones )
{
/*
	Vec3	*vLocal, *vWorld;
	int			*pPhysique;
	int			nMax;

	// 버텍스 갯수만큼 돈다.
	// 스킨의 각 버텍스들은 자기가 소속된 BONE의 최종결과 매트릭스와 자기로컬 좌표를 곱해서
	// 최종 좌표를 생성해낸다.
	vLocal	  = pObj->m_pVertexList;
	pPhysique = pObj->m_pPhysiqueVertex;
	nMax	  = pObj->m_nMaxVertexList;
	vWorld	  = pObj->_pVertexUpdate;
	while( nMax-- )
	{
		D3DXVec3TransformCoord( vWorld, vLocal, &mBones[ *pPhysique ] );		// 일단은 영향받는 bone을 한개만 쓴다.
		// 여기서 노말도 같이 돌려야 한다.
		vLocal ++;
		vWorld ++;
		pPhysique ++;
	}
*/
}

// 본의 변환이 모두 끝난후 실행된다.
// 피지크 버텍스들을 본에 맞춰 다시 계산해서 월드 좌표로 생성
// 월드좌표로 생성된 버텍스들을 버텍스 버퍼에 카피
xRESULT		CObject3D::SetVertexBuffer( GMOBJECT *pObj )
{
/*
	CUSTOMVERTEX	*_pVB = pObj->_pVB;			// 스키닝을 위한 임시버퍼
	WORD			*pIB;
	Vec3		*pVList;
	int				nMax = pObj->m_nMaxVB;		// 

	// WorldPos값만 갱신되면 되므로 
	// Skining()에서 계산된 WorldPos값만 카피시켜 준다.
	pIB    = pObj->m_pIIB;
	pVList = pObj->_pVertexUpdate;
	while( nMax-- )
	{
		_pVB->position = pVList[ *pIB ];
		_pVB ++;
		pIB ++;
	}
*/
	return xSUCCESS;
}

// 스키닝으로 갱신된 버텍스버퍼를 d3d버텍스 버퍼로 전송.
//
xRESULT CObject3D::SendVertexBuffer( GMOBJECT *pObj, GLuint nGL_VB )
{
	xRESULT	hr;
	int		nMax;
	int		nVertexSize;
	
	nVertexSize = pObj->m_nVertexSize;
	
	// 버텍스 버퍼 전송
	nMax = pObj->m_nMaxVB * nVertexSize;
				
	glBindBuffer( GL_ARRAY_BUFFER, nGL_VB );
	glBufferData( GL_ARRAY_BUFFER, nMax, pObj->m_pVB, GL_STATIC_DRAW );
	
	return xSUCCESS;
}

//
//
//
xRESULT CObject3D::SendIndexBuffer( MATERIAL_BLOCK *pBlock )
{
	int		nMax;

	// 인덱스 버퍼 전송
	nMax = pBlock->m_nPrimitiveCount * 3 * sizeof(WORD);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pBlock->m_nGL_IB );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, nMax, pBlock->m_pIB, GL_STATIC_DRAW );

	return xSUCCESS;
}


void CObject3D::SetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor )
{
	// set
	if( pBlock->m_dwEffect & XE_2SIDE )
		glDisable( GL_CULL_FACE );

	if( m_nNoEffect == 1 )	return;
	
	const GLfloat fAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const GLfloat fDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, fAmbient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fDiffuse );
	if( dwBlendFactor != 0xff000000 ) // 
	{
		if( m_nNoEffect )	return;
		const GLfloat fAmbient[] = { 0, 0, 0, 1.0f };
		const GLfloat fDiffuse[] = { 1.0f, 1.0f, 1.0f, (float)((dwBlendFactor>>24)&0xff)/255.0f };
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, fAmbient );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fDiffuse );
		glEnable( GL_BLEND );
		glEnable( GL_ALPHA_TEST );
		glAlphaFunc( GL_GEQUAL, 0 );
	}
	else
	if( pBlock->m_dwEffect & XE_OPACITY )	// 알파채널
	{
		glEnable( GL_ALPHA_TEST );
		GLenum err = glGetError();
		glAlphaFunc( GL_GEQUAL, (float)(0xb0 / 255.0f));	
		if( m_nNoEffect )	return;
		
		if( pBlock->m_nAmount < 255 )		// 
		{
			const GLfloat fAmbient[] = { 0, 0, 0, 1.0f };
			const GLfloat fDiffuse[] = { 1.0f, 1.0f, 1.0f, (float)pBlock->m_nAmount/255.0f };
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, fAmbient );
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fDiffuse );
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glAlphaFunc( GL_GEQUAL, 0 );			
		}
																	
	} else
	// Set Reflection
	if( (pBlock->m_dwEffect & XE_REFLECT) || (nEffect & XE_REFLECT) )
	{
		if( m_nNoEffect )	return;
	} else
	if( pBlock->m_dwEffect & XE_SELF_ILLUMINATE )
	{
		glDisable( GL_LIGHTING );
	} else
	// Set Specular
	if( (pBlock->m_dwEffect & XE_SPECULAR) || (nEffect & XE_SPECULAR) )
	{
		if( m_nNoEffect )	return;
	} else
	if( (pBlock->m_dwEffect & XE_BUMP) || (nEffect & XE_BUMP) )
	{
		if( m_nNoEffect )	return;
	}
	else
	if( (pBlock->m_dwEffect & XE_HIGHLIGHT_OBJ ) || (nEffect & XE_HIGHLIGHT_OBJ) ) 
	{
		if( m_nNoEffect )	return;
		glBlendFunc( GL_ONE, GL_ONE );
	}
	
//	m_nGLDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
//	m_nGLDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

}

//
//
//
void CObject3D::ResetState( MATERIAL_BLOCK* pBlock,  int nEffect, DWORD dwBlendFactor )
{
	// reset
	if( pBlock->m_dwEffect & XE_2SIDE )
		glEnable( GL_CULL_FACE );
	if( m_nNoEffect == 1 )	return;
	
	//if( nBlendFactor == 255 )
	if( dwBlendFactor == 0xff000000 )
	{
		glDisable( GL_BLEND );
		glDisable( GL_ALPHA_TEST );
		if( m_nNoEffect )	return;
	}
	else
	{
		glDisable( GL_ALPHA_TEST );
		if( m_nNoEffect )	return;
		glAlphaFunc( GL_GEQUAL, 0xb0 );
		const GLfloat fAmbient[] = { 0, 0, 0, 1.0 };
		const GLfloat fDiffuse[] = { 1.0, 1.0f, 1.0f, 1.0f };
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, fAmbient );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fDiffuse );
	}
	
	if( 0 ) //pBlock->m_nOpacity )
	{
		if( m_nNoEffect )	return;
		if( dwBlendFactor == 255 )
		{
			glDisable( GL_BLEND );
			glDisable( GL_ALPHA_TEST );
		}
		else
		{
			glDisable( GL_ALPHA_TEST );
			glAlphaFunc( GL_GEQUAL, 0xb0 );
			const GLfloat fAmbient[] = { 0, 0, 0, 1.0 };
			const GLfloat fDiffuse[] = { 1.0, 1.0f, 1.0f, 1.0f };
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, fAmbient );
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fDiffuse );
		}
	} else
	if( (pBlock->m_dwEffect & XE_REFLECT) || (nEffect & XE_REFLECT) )
	{
		if( m_nNoEffect )	return;
//		m_nGLDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//		m_nGLDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	
//		m_nGLDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
//		m_nGLDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	} else
	if( pBlock->m_dwEffect & XE_SELF_ILLUMINATE )
	{
		if( GetLight() )
			glEnable( GL_LIGHTING );
		else 
			glDisable( GL_LIGHTING );
	} else
	// reset Specular
	if( (pBlock->m_dwEffect & XE_SPECULAR) || (nEffect & XE_SPECULAR) )
	{
		if( m_nNoEffect )	return;
	} else
	if( (pBlock->m_dwEffect & XE_BUMP) || (nEffect & XE_BUMP) )
	{
		if( m_nNoEffect )	return;
	}
	

	glBindTexture( GL_TEXTURE_2D, 0 );
}
//
//	스킨형태의 오브젝트 렌더러
//  단독으로는 동작하지 못한다.
//
void	CObject3D::RenderSkin( GLuint pnGL_VB, GMOBJECT *pObj, const MATRIX *mWorld, int nEffect, DWORD dwBlendFactor )
{
	//---- 그대! 스킨오브젝트만 화면에 안나오는가! 그러면 CModelObject::Render()의 설명을 읽어보고 확인해보아라!	
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB, nMaxBone;
	MATRIX *pBone;
	MATRIX *pmBonesInv;
	int		i, idx;

	if( m_nNoEffect == 0 )
	{
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	}
	
	MATRIX	m1;

	if( m_pmExternBone )	
	{
		pBone = m_pmExternBone;		// 외부지정 뼈대가 있다면 그걸 쓰고
		pmBonesInv = m_pmExternBoneInv;
	}
	else					
	{
		pBone = m_pBaseBone;		// 없다면 디폴트를 쓴다.
		pmBonesInv = m_pBaseBoneInv;
	}

	glPushMatrix();
//	glMultMatrixf( mWorld->f );
	glEnable( GL_MATRIX_PALETTE_OES );
	GLenum err = glGetError();
	glMatrixMode( GL_MATRIX_PALETTE_OES );
	err = glGetError();
	const MATRIX *pmView = GetTransformView();
	if( pObj->m_nMaxUseBone )		// 뼈대개수가 많아서 따로 전송해야함.
	{
		nMaxBone = pObj->m_nMaxUseBone;
		for( i = 0; i < nMaxBone; i ++ )	// 
		{
			idx = pObj->m_UseBone[i];
			MatrixMultiply( m1, pmBonesInv[idx], pBone[idx] );

			glCurrentPaletteMatrixOES( i );
			MatrixMultiply( m1, m1, *mWorld );
			MatrixMultiply( m1, m1, *pmView );
			glLoadMatrixf( m1.f );

		}
	}

	glBindBuffer( GL_ARRAY_BUFFER, pnGL_VB );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_MATRIX_INDEX_ARRAY_OES );
	err = glGetError();
	glEnableClientState( GL_WEIGHT_ARRAY_OES );
	err = glGetError();
	glVertexPointer( 3, GL_FLOAT, sizeof(SKINVERTEX), (void*)offsetof(SKINVERTEX,position) );
	err = glGetError();
	glNormalPointer( GL_FLOAT, sizeof(SKINVERTEX), (void*)offsetof(SKINVERTEX,normal) );
	err = glGetError();
	glTexCoordPointer( 2, GL_FLOAT, sizeof(SKINVERTEX), (void*)offsetof(SKINVERTEX,tu) );
	err = glGetError();
	glMatrixIndexPointerOES( 2, GL_UNSIGNED_BYTE, sizeof(SKINVERTEX), (void*)offsetof(SKINVERTEX,matIdx1) );
	err = glGetError();
	glWeightPointerOES( 2, GL_FLOAT, sizeof(SKINVERTEX), (void*)offsetof(SKINVERTEX,w1) );
	err = glGetError();
	
//	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );	
//	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pGL_MtrlBlkTexture;
	GLuint *pnGL_BlkTexture = pObj->m_pGL_MtrlBlkTexture+0;

	if( m_nTextureEx )
		pnGL_BlkTexture += (pObj->m_nMaxMtrlBlk * m_nTextureEx);
	pBlock   = pObj->m_pMtrlBlk+0;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
//	nMaxMtrl = 1;
	while( nMaxMtrl-- )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pBlock->m_nGL_IB );
		if( pBlock->m_nMaxUseBone )		// 뼈대개수가 많아서 따로 전송해야함.
		{
			nMaxBone = pBlock->m_nMaxUseBone;
			for( i = 0; i < nMaxBone; i ++ )	// 
			{
				idx = pBlock->m_UseBone[i];
				MatrixMultiply( m1, pmBonesInv[idx], pBone[idx] );
				
				glCurrentPaletteMatrixOES( i );
				err = glGetError();
				MatrixMultiply( m1, m1, *mWorld );
				MatrixMultiply( m1, m1, *pmView );
				glLoadMatrixf( m1.f );
				err = glGetError();
			}
		}
		// 상태 세팅 
		SetState( pBlock, nEffect, dwBlendFactor );
		{
			GLuint pnGL_CloakTexture = g_ModelGlobal.GetTexture();
			if( pnGL_CloakTexture )		// 외부 지정 텍스쳐가 있을때.
				glBindTexture( GL_TEXTURE_2D, pnGL_CloakTexture );
			else
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				glBindTexture( GL_TEXTURE_2D, 0 );
			else
				glBindTexture( GL_TEXTURE_2D, *pnGL_BlkTexture );
		}

		glDrawElements( GL_TRIANGLES, pBlock->m_nPrimitiveCount * 3, GL_UNSIGNED_SHORT, NULL );
		GLenum err = glGetError();

		ResetState( pBlock, nEffect, dwBlendFactor );
#ifdef	__XDEBUG
		g_nMaxTri += pBlock->m_nPrimitiveCount;
#endif
		pBlock ++;
		pnGL_BlkTexture ++;
	}
/*	pnGL_BlkTexture = pObj->m_pGL_MtrlBlkTexture+7;
	
	if( m_nTextureEx )
		pnGL_BlkTexture += (pObj->m_nMaxMtrlBlk * m_nTextureEx);
	pBlock   = pObj->m_pMtrlBlk+7;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	nMaxMtrl = 1;
	while( nMaxMtrl-- )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pBlock->m_nGL_IB );
		if( pBlock->m_nMaxUseBone )		// 뼈대개수가 많아서 따로 전송해야함.
		{
			nMaxBone = pBlock->m_nMaxUseBone;
			for( i = 0; i < nMaxBone; i ++ )	// 
			{
				idx = pBlock->m_UseBone[i];
				MatrixMultiply( m1, pmBonesInv[idx], pBone[idx] );
				
				glCurrentPaletteMatrixOES( i );
				MatrixMultiply( m1, m1, *mWorld );
				MatrixMultiply( m1, m1, *pmView );
				glLoadMatrixf( m1.f );
			}
		}
		// 상태 세팅 
		SetState( pBlock, nEffect, dwBlendFactor );
		{
			GLuint pnGL_CloakTexture = g_ModelGlobal.GetTexture();
			if( pnGL_CloakTexture )		// 외부 지정 텍스쳐가 있을때.
				glBindTexture( GL_TEXTURE_2D, pnGL_CloakTexture );
			else
				if( m_nNoTexture && pObj->m_bOpacity == 0 )
					glBindTexture( GL_TEXTURE_2D, 0 );
				else
					glBindTexture( GL_TEXTURE_2D, *pnGL_BlkTexture );
		}
		
		glDrawElements( GL_TRIANGLES, pBlock->m_nPrimitiveCount * 3, GL_UNSIGNED_SHORT, NULL );
		
		ResetState( pBlock, nEffect, dwBlendFactor );
#ifdef	__XDEBUG
		g_nMaxTri += pBlock->m_nPrimitiveCount;
#endif
		pBlock ++;
		pnGL_BlkTexture ++;
	}
	*/
	glDisable( GL_MATRIX_PALETTE_OES );
	glDisableClientState( GL_MATRIX_INDEX_ARRAY_OES );
	glDisableClientState( GL_WEIGHT_ARRAY_OES );
		
	glPopMatrix();
}

//
// 단독으로는 동작하지 못한다. 사전에 SetRender등을 해줘야 한다.
// 일반형태의 렌더러
// mUpdate : 각 오브젝트가 Animate되고난 후, 최종 매트릭스의 리스트.
// mWorld : 기준 매트릭스.
//
void	CObject3D::RenderNormal( GMOBJECT *pObj, const MATRIX *mWorld, int nEffect, int nBlendFactor )
{
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB;
//	int		i;
//	MATRIX	m1;
//	MATRIX *mUpdate = _mUpdate;
	
	if( m_nNoEffect == 0 )
	{
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}
	
//	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pGL_MtrlBlkTexture;
	GLuint *pnGL_BlkTexture = pObj->m_pGL_MtrlBlkTexture;
	
	glBindBuffer( GL_ARRAY_BUFFER, pObj->m_nGL_VB );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glVertexPointer( 3, GL_FLOAT, sizeof(NORMALVERTEX), (void*)offsetof(NORMALVERTEX,position) );
	glNormalPointer( GL_FLOAT, sizeof(NORMALVERTEX), (void*)offsetof(NORMALVERTEX,normal) );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(NORMALVERTEX), (void*)offsetof(NORMALVERTEX,tu) );
	
//	pd3dDevice->SetIndices( pObj->m_nGL_IB );
	
//	m1 = mUpdate[i] * mWorld;		// mUpdate에 미리 mWorld를 곱한상태로 와도 될것 같다.
	glPushMatrix();
	glMultMatrixf( mWorld->f );

	pBlock   = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	
	while( nMaxMtrl-- )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pBlock->m_nGL_IB ); 
		// 상태 세팅 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				glDepthMask( FALSE );		// zwrite enable = false
				glDisable( GL_LIGHTING );
				glDisable( GL_CULL_FACE );
				glEnable( GL_BLEND );
				glBlendFunc( GL_ONE, GL_ONE );
			}
		} else
			SetState( pBlock, nEffect, nBlendFactor );

		{
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				glBindTexture( GL_TEXTURE_2D, 0 );
			else
				glBindTexture( GL_TEXTURE_2D, *pnGL_BlkTexture );
		}
		GLenum err = glGetError();

		glDrawElements( GL_TRIANGLES, pBlock->m_nPrimitiveCount * 3, GL_UNSIGNED_SHORT, NULL );

		// 상태 복원 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				glDepthMask( TRUE );		// zwrite enable = false
				if( GetLight() )
					glEnable( GL_LIGHTING );
				else
					glDisable( GL_LIGHTING );
				glEnable( GL_CULL_FACE );
				glDisable( GL_BLEND );
			}
		} else
			ResetState( pBlock, nEffect, nBlendFactor );
#ifdef	__XDEBUG
		g_nMaxTri += pBlock->m_nPrimitiveCount;
#endif
		pBlock ++;
		pnGL_BlkTexture ++;
	}
	glPopMatrix();
}

#if 0
// 충돌메시의 렌더.
void	CObject3D::RenderCollObject( LPDIRECT3DDEVICE9 pd3dDevice, const MATRIX *mWorld )
{
	GMOBJECT *pObj = &m_CollObject;
	int nEffect = 0;
	int nBlendFactor = 255;
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB;

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX );

#ifdef __COLA
	DWORD	dwFillMode;
	pd3dDevice->GetRenderState( D3DRS_FILLMODE, &dwFillMode );
#endif
	if( m_nNoEffect == 0 )
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}
	
	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pGL_MtrlBlkTexture;

#ifdef __YNORMAL_MAP
	if( pObj->m_bBump && g_Option.m_bSpecBump )
		pd3dDevice->SetStreamSource( 0, pObj->m_nGL_VB, 0, sizeof(NORMALVERTEX_BUMP) );
	else
#endif //__YNORMAL_MAP
		pd3dDevice->SetStreamSource( 0, pObj->m_nGL_VB, 0, sizeof(NORMALVERTEX) );
	
	
	pd3dDevice->SetIndices( pObj->m_nGL_IB );
	
	//	m1 = mUpdate[i] * mWorld;		// mUpdate에 미리 mWorld를 곱한상태로 와도 될것 같다.
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );
	
	pBlock   = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	while( nMaxMtrl-- )
	{
		// 상태 세팅 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE  );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			}
		} else
			SetState( pBlock, nEffect, nBlendFactor );
		
#ifdef __COLA
		if( dwFillMode != D3DFILL_SOLID )
			pd3dDevice->SetTexture( 0, NULL );
		else
#endif
		{
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				pd3dDevice->SetTexture( 0, NULL );
			else
				pd3dDevice->SetTexture( 0, *pBlkTexture );
		}
		
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );

		// 상태 복원 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, GetLight() );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}
		} else
			ResetState( pBlock, nEffect, nBlendFactor );
#ifdef	__XDEBUG
		g_nMaxTri += pBlock->m_nPrimitiveCount;
#endif
		pBlock ++;
		pBlkTexture ++;
	}
}
#endif // 0

//
// 자체 본애니메이션을 쓰는 배경오브젝트들은 이 함수로 본 매트릭스를 쉐이더에 보낸다.
//
void	CObject3D::SetShader( const MATRIX *mWorld )
{
	int		i;
	if( m_pBaseBone == NULL )	return;

	MATRIX *pmBones = m_pBaseBone;		// 뼈대모션이 로딩된게 없으면 디폴트를 쓴다.
	MATRIX *pmBonesInv = m_pBaseBoneInv ;
	MATRIX m1;
	
	if( m_bSendVS )	// 뼈대개수가 MAX_VS_BONE이하라서 한번에 다 전송한다.
	{
		glEnable( GL_MATRIX_PALETTE_OES );
		glMatrixMode( GL_MATRIX_PALETTE_OES );
		const MATRIX *pmView = GetTransformView();
		int		nMaxBone = m_nMaxBone;
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
	mViewProj = *mWorld * s_mView * s_mProj;
	MATRIXTranspose( &mViewProjTranspose, &mViewProj );
	MATRIXTranspose( &mWorldTranspose, mWorld );
	
	MATRIXInverse( &mInvWorld, NULL, mWorld );
	D3DXVec4Transform( &vLight, &vLight, &mInvWorld );
//	D3DXVec4Transform( &vEyePos, &vEyePos, &mInvWorld );
	D3DXVec4Normalize( &vLight, &vLight );
	
	m_nGLDevice->SetVertexShaderConstantF( 84, (float*)&mViewProjTranspose, 4 );
//		m_nGLDevice->SetVertexShaderConstantF( 88, (float*)&mWorldTranspose, 4 );
//	m_nGLDevice->SetVertexShaderConstantF( 88, (float*)&vEyePos,  1 );
//	m_nGLDevice->SetVertexShaderConstantF( 89, (float*)&fSpecular, 1 );
//	m_nGLDevice->SetVertexShaderConstantF( 90, (float*)&fLightCol, 1 );
	m_nGLDevice->SetVertexShaderConstantF( 92, (float*)&vLight,   1 );
	m_nGLDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
	m_nGLDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
	//		m_nGLDevice->SetVertexShaderConstant( 95, &vConst, 1 );
*/
}

//
//
//
void	CObject3D::Render( GLuint *pnGL_VB, FLOAT fFrameCurrent, int nNextFrame, const MATRIX *mWorld, int nEffect, DWORD dwBlendFactor )
{
	int			i;
	int			nMax = m_pGroup->m_nMaxObject;
	GMOBJECT	*pObj;
	MATRIX	m1;
	MATRIX	*pBone = NULL;

//	m_tmCreate = g_tmCurrent;	// 한번이라도 렌더링 되고 있다면 계속 사용하고 있는것으로 봐야 한다,.

	// 애니메이션이 있거나 외부지정뼈대가 있다면 애니메이트 돌아야 한다.
	if( m_pGroup->_mUpdate )		// maxframe이 있다면 외부본이건 내장본이건 Animate는 돌아야 한다.
		Animate( fFrameCurrent, nNextFrame );	// mUpdate 애니메이션 계산후 갱신된 매트릭스
	if( m_nMaxFrame > 0 )
	{
		if( m_pmExternBone == NULL )
		{
			if( m_pMotion )	
				m_pMotion->AnimateBone( m_pBaseBone, NULL, fFrameCurrent, nNextFrame, 0, FALSE, 0.0f );
		}
	}
	if( m_pBaseBone )	// 모션은 없고 본만 있더라도 쉐이더에 전송해야함.
		SetShader( mWorld );
	
	for( i = 0; i < nMax; i ++ )
	{
		pObj = &m_pGroup->m_pObject[i];
#ifdef	__XDEBUG
	if( pObj->m_nVertexSize == 0 /*|| pObj->m_dwFVF == 0*/ )
		XERROR( "%s : FVF가 기술되지 않았다. VertexSize=%d", m_szFileName, pObj->m_nVertexSize );
#endif

		if( pObj->m_Type == GMT_SKIN )
		{
#ifdef __XDEBUG
			if( pnGL_VB == NULL )
				XERROR( "CObject3D::Render : %s 스킨오브젝트인데 pnGL_VB가 없다.", m_szFileName );
#endif
			RenderSkin( pnGL_VB[i], pObj, mWorld, nEffect, dwBlendFactor );
		} else
		{
			MatrixMultiply( m1, m_pGroup->_mUpdate[i], *mWorld );
			if( pObj->m_bLight && GetNight() == FALSE )	// 라이트 오브젝트는 낮엔 렌더링 되지 않음.
				continue;
			RenderNormal( pObj, &m1, nEffect, dwBlendFactor );
		}
	}
	if( m_nNoTexture == 0 )
	{
		glBindTexture( GL_TEXTURE_2D, 0 );
//		pd3dDevice->SetTexture( 1, NULL );
//		pd3dDevice->SetTexture( 0, NULL );
	}

}

//
//
//
void	CObject3D::RenderBB( const MATRIX *mWorld )
{
//	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );		// Set World Transform 

//	g_Grp3D.RenderAABB( m_vBBMin, m_vBBMax, 0xffffffff );
}



#endif // 0
