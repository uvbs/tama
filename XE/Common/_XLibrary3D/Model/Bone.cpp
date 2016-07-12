﻿#include <stdio.h>
#include <string.h>
#include "model.h"
#include "Bone.h"
#include "path.h"


CBonesMng		g_BonesMng;

////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CBones::CBones()
{
	Init();
}

CBones::~CBones()
{
	Destroy();
}

void	CBones::Init( void )
{
	m_nID = 0;
	memset( m_szName, 0, sizeof(m_szName) );
	m_nMaxBone = 0;
	m_pBones = NULL;
	m_nRHandIdx = m_nLHandIdx = m_nRArmIdx = m_nLArmIdx = 0;

	MatrixIdentity( m_mLocalRH );
	MatrixIdentity( m_mLocalLH );
	MatrixIdentity( m_mLocalShield );
	MatrixIdentity( m_mLocalKnuckle );

	memset( m_vEvent, 0, sizeof(m_vEvent) );
	memset( m_nEventParentIdx, 0, sizeof(m_nEventParentIdx) );

}

void	CBones::Destroy( void )
{
	SAFE_DELETE_ARRAY( m_pBones );
}


//
//
//
int		CBones::LoadBone( LPCTSTR szFileName )
{
	int		nNumBone;
	int		i;
	int		nLen;
	int		nVer;

	FILE *fp = fopen( szFileName, "rb" );
	if( fp == NULL )		return	FAIL;

	fread( &nVer, 4, 1, fp );		// version
	if( nVer < VER_BONE )
	{
		XERROR( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_BONE );
		fclose(fp);
		return FAIL;
	}

	// ID읽기를 넣음.
	fread( &m_nID, 4, 1, fp );

#ifdef	__XDEBUG
	if( strlen( GetFileName( szFileName ) ) > sizeof(m_szName) )
	{
		XERROR( "%s : 파일명이 길다.", szFileName );
		fclose(fp);
		return FAIL;
	}
#endif
	// 파일명 카피
	strcpy( m_szName, GetFileName( szFileName ) );

	fread( &nNumBone, 4, 1, fp );			// 본 개수 읽음
	m_nMaxBone = nNumBone;
	m_pBones = new BONE[ nNumBone ];			// 본 개수 만큼 할당
	memset( m_pBones, 0, sizeof(BONE) * nNumBone );		// zero clear

	for( i = 0; i < nNumBone; i ++ )
	{
		fread( &nLen, 4, 1, fp );		// 널 포함길이
		fread( m_pBones[i].m_szName,		nLen, 1, fp );		// bone node 이름 저장
		fread( &m_pBones[i].m_mTM,			sizeof(MATRIX), 1, fp );			// World(Object) TM
		fread( &m_pBones[i].m_mInverseTM,	sizeof(MATRIX), 1, fp );			// Inv NODE TM
		fread( &m_pBones[i].m_mLocalTM,		sizeof(MATRIX), 1, fp );			// LocalTM
		fread( &m_pBones[i].m_nParentIdx,	4, 1, fp );								// parent bone index
		if( strcmpi( m_pBones[i].m_szName + 6, "r hand") == 0 )	// 부모이름이 R Hand면...
			m_nRHandIdx = i;
		if( strcmpi( m_pBones[i].m_szName + 6, "l hand") == 0 )	// 부모이름이 L Hand면...
			m_nLHandIdx = i;
		if( strcmpi(m_pBones[i].m_szName + 6, "l forearm") == 0 )	// 부모이름이 L ForeArm면...
			m_nLArmIdx = i;
		if( strcmpi(m_pBones[i].m_szName + 6, "r forearm") == 0 )	// 부모이름이 R ForeArm면...
			m_nRArmIdx = i;
	}

	fread( &m_bSendVS, 4, 1, fp );
	
	// 오른손 무기쥐는 위치의 행렬
	fread( &m_mLocalRH, sizeof(MATRIX), 1, fp );
	fread( &m_mLocalLH, sizeof(MATRIX), 1, fp );
	fread( &m_mLocalShield, sizeof(MATRIX), 1, fp );
	fread( &m_mLocalKnuckle, sizeof(MATRIX), 1, fp );
	
	fread( m_vEvent, sizeof(Vec3) * MAX_MDL_EVENT, 1, fp );
	fread( m_nEventParentIdx, sizeof(int) * MAX_MDL_EVENT, 1, fp );


	fclose(fp);

	// 부모 포인터를 셋팅
	for( i = 0; i < nNumBone; i ++ )
	{
		if( m_pBones[i].m_nParentIdx == -1 )			// 부모가 없으면 부모포인터는 널
			m_pBones[i].m_pParent = NULL;
		else
			m_pBones[i].m_pParent = &m_pBones[ m_pBones[i].m_nParentIdx ];
	}

	return SUCCESS;
}







////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CBonesMng::CBonesMng()
{
	Init();
}

CBonesMng::~CBonesMng()
{
	Destroy();
}


void	CBonesMng::Init( void )
{
	int		i;

	m_nSize = 0;
	m_nMax = 0;
	for( i = 0; i < MAX_BONES; i ++ )	m_pBonesAry[i] = NULL;
}

void	CBonesMng::Destroy( void )
{
	int		i;
	CBones	**pAry = m_pBonesAry;
	CBones	*pBone;

	for( i = 0; i < MAX_BONES; i ++ )
	{
		pBone = *pAry++;
		if( pBone )
		{
			SAFE_DELETE( pBone );
		}
	}

	Init();
}

// 본 파일Load요청이 들어오면 파일을 읽어 메모리에 적재한다.
// 적재할땐 중복된것은 이미 로딩되었던 본의 포인터를 리턴한다.
// 파일의 첫머리에 고유 아이디를 넣어서 검색할때 파일이름으로 하지말고 아이디로 하도록 한다.
CBones *CBonesMng::LoadBone( LPCTSTR szFileName )
{
	int		i;
	CBones **pAry = m_pBonesAry;
	CBones *pBones;
	int		nIdx = -1;

	// 이미 메모리에 적재 되었는지 검색, 동시에 빈곳도 검색
	for( i = 0; i < MAX_BONES; i ++ )
	{
		pBones = *pAry++;
		if( pBones )				// 널이 아닌것은 무슨 본이든 로딩되어 있다는 뜻
		{
			if( strcmpi(pBones->m_szName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
			{
				return pBones;
			}
		} else
		{
			if( nIdx == -1 )		nIdx = i;			// 빈곳이 있으면 가장처음 빈곳이 나온곳을 기억해둠
		}
	}

	pBones = new CBones;
	// 로딩된게 아니었다면.  실제로 데이타 읽음.
	if( pBones->LoadBone( szFileName ) == FAIL )
	{
		return NULL;
	}

	// 읽은 본포인터를 리스트에 등록
	m_pBonesAry[ nIdx ] = pBones;

	m_nMax ++;			// 가진 본 갯수 증가

	return pBones;		// 읽은 본 포인터 리턴
}

////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CMotionMng		g_MotionMng;


CMotionMng::CMotionMng()
{
	Init();
}

CMotionMng::~CMotionMng()
{
	Destroy();
}

void	CMotionMng::Init( void )
{
	int		i;
	m_nSize = 0;
	m_nMax = 0;
	m_nCachePos = 0;

	for( i = 0; i < MAX_MOTION; i ++ )
		m_pMotionAry[i] = NULL;
	
	for( i = 0; i < MAX_MOTION_CACHE; i ++ )
		m_pCache[i] = NULL;
}

void	CMotionMng::Destroy( void )
{
	int		i;
	CMotion		**pAry = m_pMotionAry;
	CMotion		*pMotion;

	for( i = 0; i < MAX_MOTION; i ++ )
	{
		pMotion = *pAry++;
		if( pMotion )
		{
			SAFE_DELETE( pMotion );
		}
	}

	Init();
}

//
//
//
CMotion *CMotionMng::LoadMotion( LPCTSTR szFileName )
{
	int		i;
	CMotion **pAry, *pMotion;
	int		nIdx = -1;

	// 이미 메모리에 적재 되었는지 검색, 동시에 빈곳도 검색
	// 일단 캐쉬를 검색
	pAry = m_pCache;
	for( i = 0; i < MAX_MOTION_CACHE; i ++ )
	{
		pMotion = *pAry++;
		if( pMotion )
		{
			if( strcmpi(pMotion->m_szName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
			{
				return pMotion;
			}
		}
	}

	// 캐쉬에서 못찾았으면 리스트에서 검색
	pAry = m_pMotionAry;
	for( i = 0; i < MAX_MOTION; i ++ )
	{
		pMotion = *pAry++;
		if( pMotion )				// 널이 아닌것은 무슨 모션이든 로딩되어 있다는 뜻
		{
			if( strcmpi(pMotion->m_szName, szFileName) == 0 )		// 같은걸 찾았으면 그걸 리턴
			{
				return pMotion;
			}
		} else
		{
			if( nIdx == -1 )		nIdx = i;			// 빈곳이 있으면 가장처음 빈곳이 나온곳을 기억해둠
		}
	}
	if( nIdx == -1 )	
	{
		XERROR( "%s : 읽을 수 있는 한계를 초과했다", szFileName );
	}

	pMotion = new CMotion;
	// 로딩된게 아니었다면.  실제로 데이타 읽음.
	if( pMotion->LoadMotion( szFileName ) == FAIL )
	{
		delete pMotion;
		return NULL;
	}
	// 읽은 모션포인터를 리스트에 등록
	m_pMotionAry[ nIdx ] = pMotion;
	
	// 캐시에도 넣음
	m_pCache[ m_nCachePos++ ] = pMotion;	
	if( m_nCachePos >= MAX_MOTION_CACHE )		m_nCachePos = 0;

	m_nMax ++;			// 가진 모션 갯수 증가

	return pMotion;		// 읽은 모션 포인터 리턴
}


////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////

CMotion::CMotion()
{
	Init();
}

CMotion::~CMotion()
{
	Destroy();
}

void	CMotion::Init( void )
{
	m_pMotion = NULL;
	m_pPath = NULL;
	m_pBoneFrame = NULL;
	m_fPerSlerp = 0.5f;
	m_nMaxEvent = 0;
	m_pBoneInfo = NULL;
	memset( m_vEvent, 0, sizeof(m_vEvent) );
	m_pAttr = NULL;
	m_pBoneInfo = NULL;
}

void	CMotion::Destroy( void )
{
	int		i;

	SAFE_DELETE_ARRAY( m_pAttr );
	SAFE_DELETE_ARRAY( m_pMotion );
	SAFE_DELETE_ARRAY( m_pPath );
	if( m_pBoneFrame )
	{
		for( i = 0; i < m_nMaxBone; i ++ )
			m_pBoneFrame[i].m_pFrame = NULL;
	}
	SAFE_DELETE_ARRAY( m_pBoneFrame );
	SAFE_DELETE_ARRAY( m_pBoneInfo );

	Init();
}

//
//
//
int		CMotion::LoadMotion( LPCTSTR szFileName )
{
	int		nNumBone;
	int		nNumFrame;
//	int		nNumSize;
//	int		i, j;
	int		nVer;

	FILE *fp = fopen( szFileName, "rb" );
	if( fp == NULL )	
	{
		XERROR( "%s : 찾을 수 없음", szFileName );
		fclose(fp);
		return FAIL;
	}

	fread( &nVer, 4, 1, fp );		// version
	if( nVer != VER_MOTION )
	{
		XERROR( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_MOTION );
		fclose(fp);
		return FAIL;
	}

	// ID읽기를 넣음.
	fread( &m_nID, 4, 1, fp );

#ifdef	__XDEBUG
	if( strlen( GetFileName( szFileName ) ) > sizeof(m_szName) )
	{
		XERROR( "%s : 파일명이 길다.", szFileName );
		fclose(fp);
		return FAIL;
	}
#endif
	// 파일명 카피
	strcpy( m_szName, GetFileName( szFileName ) );
	
	fread( &m_fPerSlerp, sizeof(float), 1, fp );		// 
	fseek( fp, 32, SEEK_CUR );		// reserved

	fread( &nNumBone, 4, 1, fp );			// 뼈대 갯수 읽음
	fread( &nNumFrame, 4, 1, fp );		// 애니메이션 프레임 개수 읽음
	m_nMaxFrame = nNumFrame;
	m_nMaxBone = nNumBone;

	if( nNumFrame <= 0 )
	{
#ifdef __XDEBUG
		XERROR( "CMotion::LoadMotion : %s read MaxFrame = %d", szFileName, nNumFrame );
#endif
		fclose(fp);
		return FAIL;
	}

	// path 
	int nTemp;
	fread( &nTemp, 4, 1, fp );	// path정보가 있는가?
	if( nTemp )
	{
		m_pPath = new Vec3[ nNumFrame ];
		fread( m_pPath, sizeof(Vec3) * nNumFrame, 1, fp );		// nNumFrame만큼 한방에 읽어버리기.
	}

	//
	// TM_Animation 데이타 읽음.
	ReadTM( fp, nNumBone, nNumFrame );
	
	// 프레임 속성 읽음.
	fread( m_pAttr, sizeof(MOTION_ATTR) * nNumFrame, 1, fp );

	fread( &m_nMaxEvent, 4, 1, fp );	// 이벤트 좌표 저장
	if( m_nMaxEvent > 0 )
		fread( m_vEvent, sizeof(Vec3) * m_nMaxEvent, 1, fp );
	

	fclose(fp);

	return SUCCESS;
}

//
//  Motion의 core부분을 읽음. 외부에서 단독 호출 가능.
//
void	CMotion::ReadTM( FILE *fp, int nNumBone, int nNumFrame )
{
	int		nNumSize;
	int		nFrame;
	int		i;

	m_nMaxBone = nNumBone;		// LoadMotion()에서 불려졌다면 이부분은 필요없으나 ReadTM만 따로 불릴 일이 있으면 이게 필요하다.
	m_nMaxFrame = nNumFrame;

	//--- 뼈대구성정보 읽음.
	m_pBoneInfo = new BONE[ nNumBone ];			// 본 개수 만큼 할당
	memset( m_pBoneInfo, 0, sizeof(BONE) * nNumBone );		// zero clear
	
	int		nLen;
	for( i = 0; i < nNumBone; i ++ )
	{
		fread( &nLen, 4, 1, fp );
#ifdef	__XDEBUG
		if( nLen > 32 )		XERROR("CMotion::ReadTM - %s 뼈대이름이 너무 길다", m_szName );
#endif
		fread( m_pBoneInfo[i].m_szName,	nLen, 1, fp );		// bone node 이름
		fread( &m_pBoneInfo[i].m_mInverseTM,	sizeof(MATRIX), 1, fp );			// Inv NODE TM
		fread( &m_pBoneInfo[i].m_mLocalTM,		sizeof(MATRIX), 1, fp );			// LocalTM
		fread( &m_pBoneInfo[i].m_nParentIdx,	4, 1, fp );								// parent bone index
	}
	
	// 부모 포인터를 셋팅
	for( i = 0; i < nNumBone; i ++ )
	{
		if( m_pBoneInfo[i].m_nParentIdx == -1 )			// 부모가 없으면 부모포인터는 널
			m_pBoneInfo[i].m_pParent = NULL;
		else
			m_pBoneInfo[i].m_pParent = &m_pBoneInfo[ m_pBoneInfo[i].m_nParentIdx ];
	}
	

	fread( &nNumSize, 4, 1, fp );			// 프레임 사이즈 읽음 - 메모리 풀 사이즈
	//--- 모션 읽음.
	m_pMotion		= new TM_ANIMATION[ nNumSize ];		// 메모리 풀
	m_pBoneFrame	= new BONE_FRAME[ nNumBone ];
	m_pAttr			= new MOTION_ATTR[ nNumFrame ];
	memset( m_pAttr, 0, sizeof(MOTION_ATTR) * nNumFrame );	// nNumSize였는데 nNumFrame이 맞는거 같다.
	TM_ANIMATION	*p = m_pMotion;
	int		nCnt = 0;
	
	// 뼈대 수 만큼 루프
	for( i = 0; i < nNumBone; i ++ )
	{
		fread( &nFrame, 4, 1, fp );
		if( nFrame == 1 )		// 1이면 현재 뼈대에 프레임 있음
		{
			m_pBoneFrame[i].m_pFrame = p;
			fread( m_pBoneFrame[i].m_pFrame, sizeof(TM_ANIMATION) * nNumFrame, 1, fp );		// 한방에 읽어버리기.
			p += nNumFrame;
			nCnt += nNumFrame;
		} else			// 현재 뼈대에 프레임 없음
		{
			fread( &(m_pBoneFrame[i].m_mLocalTM), sizeof(MATRIX), 1, fp );			// 프레임이 없으면 LocalTM만 읽고
			m_pBoneFrame[i].m_pFrame = NULL;
			// m_mLocalTM에 넣었으므로 메모리 풀에는 넣을필요 없다.
		}
	}
	
#ifdef	__XDEBUG
	if( nCnt != nNumSize )
	{
		XERROR( "%s : 프레임 사이즈가 맞지 않다", m_szName );
	}
#endif
	
}

//
// 뼈대의 애니메이션
// 
void	CMotion::AnimateBone( MATRIX *pmUpdateBone, CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight )
{
	int		i;
	BONE	*pBone = m_pBoneInfo;		// 이 모델의 뼈대 시작 포인터.
	TM_ANIMATION	*pFrame = NULL;					// 현재 프레임
	TM_ANIMATION	*pNext = NULL;					// 다음 프레임
	TM_ANIMATION	*pFrameOld = NULL;				// 이전모션의 프레임
//	MATRIX	*pMatBone = m_mUpdateBone;
	MATRIX	*pMatBones = pmUpdateBone;			// 업데이트 매트릭스 리스트 시작 포인터
	int			nCurrFrame;
	float		fSlp;
	QUATERNION		qSlerp;
	Vec3			vSlerp;
	BONE_FRAME	*pBoneFrame = NULL;
	BONE_FRAME	*pBoneFrameOld = NULL;		// 본의 계층구조
	MATRIX	m1, m2;

	if( pMotionOld == NULL )	// pMotionOld-> 할때 널참조 에러만 안나게함.
		pMotionOld = this;

	// 애니메이션 동작이 있는 경우
	i = m_nMaxBone;
	// 보간을 위한 Slerp 계산.
	nCurrFrame = (int)fFrameCurrent;				// 소숫점 떼내고 정수부만..
	fSlp = fFrameCurrent - (float)nCurrFrame;	// 소숫점 부분만 가져옴
	/////////////////////////////

//	int	nNextFrame = GetNextFrame();
	pBoneFrame	  = m_pBoneFrame;
	pBoneFrameOld = pMotionOld->m_pBoneFrame;		// 이전동작의 뼈대 계층
	// 뼈대 갯수만큼 루프 돈다
	while( i-- )
	{
		if( pBoneFrameOld->m_pFrame )
			pFrameOld = &pBoneFrameOld->m_pFrame[nFrameOld];		// 일단은 0으로 한다.  이전모션의 nCurrFrame도 기억하고 있어야 한다.
		else
			pFrameOld = NULL;		// 일단은 0으로 한다.  이전모션의 nCurrFrame도 기억하고 있어야 한다.
		if( pBoneFrame->m_pFrame )		// 이 뼈대에 프레임이 있을때
		{
			pFrame = &pBoneFrame->m_pFrame[ nCurrFrame ];		// 현재 프레임 포인터 받음
			pNext  = &pBoneFrame->m_pFrame[ nNextFrame ];		// 다음 프레임 포인터 받음

			MatrixQuaternionSlerp( qSlerp, pFrame->m_qRot, pNext->m_qRot, fSlp );	// 보간된 쿼터니언 구함
			MatrixVec3Lerp( vSlerp, pFrame->m_vPos, pNext->m_vPos, fSlp );		// 보간된 Pos벡터 구함

			if( bMotionTrans )	// 이전 모션과 블렌딩 해야 하는가
			{
				float	fBlendSlp = fBlendWeight;
				if( pFrameOld )
				{
					MatrixQuaternionSlerp( qSlerp, pFrameOld->m_qRot, qSlerp, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					MatrixVec3Lerp( vSlerp, pFrameOld->m_vPos, vSlerp, fBlendSlp );		// 보간된 Pos벡터 구함
				} else
				{
					QUATERNION	qLocal;
					Vec3		vLocal;
					MATRIX		*pmLocalTM = &pBoneFrameOld->m_mLocalTM;
					QuaternionRotationMatrix( qLocal, *pmLocalTM );
					vLocal.x = pmLocalTM->f[_41];
					vLocal.y = pmLocalTM->f[_42];
					vLocal.z = pmLocalTM->f[_43];
					MatrixQuaternionSlerp( qSlerp, qLocal, qSlerp, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					MatrixVec3Lerp( vSlerp, vLocal, vSlerp, fBlendSlp );		// 보간된 Pos벡터 구함
				}
			}

			// matAniTM계산
			// 이동행렬 만듦
			MatrixTranslation( m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
		
			// 쿼터니온 보간된 회전키값을 회전행렬로 만듦
			MatrixRotationQuaternion( m2, qSlerp );
			MatrixMultiply( m2, m2, m1 );	// 이동행렬 X 회전행렬 = 애니행렬
			if( pBone->m_pParent )
				MatrixMultiply( m2, m2, pMatBones[ pBone->m_nParentIdx ] );			// 부모가 없다면 루트인데 항등행렬과 곱할 필요 없어서 생략했다.
//				m2 *= pMatBones[ pBone->m_nParentIdx ];//				else
			
			// WorldTM = LocalTM * ParentTM;
			// 최종 변환된 월드매트릭스를 멤버에 넣어서 Render()에서 SetTransfrom함.
		} else
		// 이 뼈대에 프레임이 없을때.
		{
			if( bMotionTrans )	// 이전 모션과 블렌딩 해야 하는가
			{
				float	fBlendSlp = fBlendWeight;
				QUATERNION	qLocal;
				Vec3		vLocal;
				MATRIX		*pmLocalTM = &pBoneFrame->m_mLocalTM;
				QuaternionRotationMatrix( qLocal, *pmLocalTM );
				vLocal.x = pmLocalTM->f[_41];
				vLocal.y = pmLocalTM->f[_42];
				vLocal.z = pmLocalTM->f[_43];	// 현재프레임의 LocalTM을 쿼터니온과 벡터로 빼냄.

				if( pFrameOld )
				{
					MatrixQuaternionSlerp( qSlerp, pFrameOld->m_qRot, qLocal, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					MatrixVec3Lerp( vSlerp, pFrameOld->m_vPos, vLocal, fBlendSlp );		// 보간된 Pos벡터 구함
				} else
				{
					QUATERNION	qLocal2;
					Vec3		vLocal2;
					MATRIX		*pmLocalTM2 = &pBoneFrameOld->m_mLocalTM;
					QuaternionRotationMatrix( qLocal2, *pmLocalTM2 );
					vLocal2.x = pmLocalTM2->f[_41];
					vLocal2.y = pmLocalTM2->f[_42];
					vLocal2.z = pmLocalTM2->f[_43];
					MatrixQuaternionSlerp( qSlerp, qLocal2, qLocal, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					MatrixVec3Lerp( vSlerp, vLocal2, vLocal, fBlendSlp );		// 보간된 Pos벡터 구함
				}
				MatrixTranslation( m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
			
				// 쿼터니온 보간된 회전키값을 회전행렬로 만듦
				MatrixRotationQuaternion( m2, qSlerp );
				MatrixMultiply( m2, m2, m1 );
//				m2 *= m1;			// 이동행렬 X 회전행렬 = 애니행렬
			} else
				m2 = pBoneFrame->m_mLocalTM;

			if( pBone->m_pParent )
				MatrixMultiply( m2, m2, pMatBones[ pBone->m_nParentIdx ] );
//				m2 *= pMatBones[ pBone->m_nParentIdx ];
		}

		*pmUpdateBone = m2;		// 이 뼈대의 최종 월드 변환 매트릭스

		pBoneFrame ++;
		pBoneFrameOld ++;
		pBone ++;					// 본 ++
		pmUpdateBone ++;			// 본 트랜스폼 매트릭스 ++
	}
}

