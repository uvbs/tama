#include "stdafx.h"
#include "xSkill.h"
#include "XSkillUser.h"
#include "XESkillMng.h"
#include "XLua.h"
#include "Sprite/Sprdef.h"
#include "Game/XEComponents.h"
#ifdef _VER_IPHONE
#undef min
#undef max
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

NAMESPACE_XSKILL_START
xtTargetFilter MakeTargetFilter( const EFFECT *pEffect )
{
	xtTargetFilter tfFilter = xTF_NONE;
	DWORD dwFilter = tfFilter;
	if( pEffect->liveTarget == xTL_LIVE )
		dwFilter |= xTF_LIVE;
	else if( pEffect->liveTarget == xTL_DEAD )
		dwFilter |= xTF_DEAD;
	tfFilter = (xtTargetFilter) dwFilter;
	return tfFilter;
}

void XSkillUser::Destroy() 
{
	XLIST_DESTROY( m_listUseSkill, XSkillObj* );
/*	list<XSkillObj*>::iterator itor;
	for( itor = m_listUseSkill.begin(); itor != m_listUseSkill.end(); )
	{
		XSkillObj* pNode = (*itor);	
		SAFE_DELETE( pNode );
		m_listUseSkill.erase( itor++ );	
	} */
}

// this가 시전자
	/*
XSkillObj* XSkillUser::CreateAddUseSkillByName( LPCTSTR szName )
{
	XSkillDat *pSkillDat;
	XSkillObj *pUseSkill;
	if( pSkillDat = m_prefSkillMng->FindByName( szName ) )
	{
		// 이미 내가 보유하고 있는 스킬이면 걍 그걸 리턴한다
		if( (pUseSkill = FindUseSkillByName( szName )) == NULL )		
		{
			pUseSkill = CreateSkillUseObj( pSkillDat ) ;	// 사용스킬생성은 버추얼로 맡긴다
			if( XASSERT( pUseSkill ) ) 
			{
//				pSkillDat->AddRefCnt();		// 레퍼런스 카운트 올림
				AddUseSkill( pUseSkill );		// 보유스킬목록에 추가
			}
		}
	} else
		XBREAKF( 1, "%s 스킬을 찾지못했습니다. 스킬목록을 확인해주십시요", szName );
	return pUseSkill;
}
	 */
XSkillObj* XSkillUser::CreateSkillUseObj( XSkillDat *pSkillDat ) 
{ 
	return new XSkillObj( pSkillDat ); 
}	

// idSkillDat스킬로 사용스킬객체를 만들어서 보유한다
XSkillObj* XSkillUser::CreateAddUseSkillByID( ID idSkillDat )		
{
	XSkillDat *pSkillDat;
	XSkillObj *pUseSkill = NULL;
	if(( pSkillDat = m_prefSkillMng->FindByID( idSkillDat ) ))
	{
		pUseSkill = CreateSkillUseObj( pSkillDat ) ;	// 사용스킬생성은 버추얼로 맡긴다
		if( XASSERT( pUseSkill ) ) 
		{
//			pSkillDat->AddRefCnt();		// 레퍼런스 카운트 올림
			AddUseSkill( pUseSkill );		// 보유스킬목록에 추가
		}
	} else
		XLOG_ALERT( "ID:%d 스킬을 찾지못했습니다. 스킬목록을 확인해주십시요", idSkillDat );
	return pUseSkill;
}

// this가 시전자
// 식별자로 스킬사용객체를 만들어 보유목록에 추가한다.
XSkillObj* XSkillUser::CreateAddUseSkillByIdentifier( LPCTSTR szIdentifier )
{
	XSkillDat *pSkillDat;
	XSkillObj *pUseSkill = NULL;
	if(( pSkillDat = m_prefSkillMng->FindByIdentifier( szIdentifier ) ))
	{
		// 이미 내가 보유하고 있는 스킬이면 걍 그걸 리턴한다
		if( (pUseSkill = FindUseSkillByIdentifier( szIdentifier )) == NULL )		
		{
			// 사용스킬생성은 버추얼로 맡긴다
			pUseSkill = CreateSkillUseObj( pSkillDat ) ;	
			if( XASSERT( pUseSkill ) ) 
			{
				AddUseSkill( pUseSkill );		// 보유스킬목록에 추가
			}
		}
	} else
		XBREAKF( 1, "%s 스킬을 찾지못했습니다. 스킬목록을 확인해주십시요", szIdentifier );
	return pUseSkill;
}

// pTarget을 시전대상으로 했을때 얻어지는 모든 시전대상을 얻는다.
int XSkillUser::GetCastingTargetList( 
					XArrayLinearN<XSkillReceiver*, 100> *pAryOutCastingTarget,			// 시전대상얻기 결과가 담겨짐
					XE::VEC2 *pvOutTarget,		// 시전대상이 바닥일때 바닥좌표를 넣어 되돌려줌
					xtCastTarget castTarget,	// 시전대상타입
					XSkillObj *pUseSkill,				// 스킬사용 오브젝트
					EFFECT *pEffect,					// 효과
					XSkillReceiver **ppOutBaseTarget, 
					const XE::VEC2 *pvPos )					// 선택되어있던 타겟
{
	XBREAK( ppOutBaseTarget == NULL );
//	XBREAK( plistOutCastingTarget->size() > 0 );	// 이미 리스트에 뭔가 있으면 에러다
	switch( castTarget )
	{
	// 시전자 자신
	case xCST_SELF:		
		// 베이스타겟은 시전자로 정해진다.
		*ppOutBaseTarget = GetThisRecv();
		if( pEffect->castTargetRange == xTR_ONE )
		{
			pAryOutCastingTarget->Add( *ppOutBaseTarget );
		} else
		if( pEffect->castTargetRange == xTR_CIRCLE )
		{
			// 코딩만 하고 테스트 아직 안됨
			// 시전반경이 있으면 자동으로 범위로 캐스팅 타겟을 뽑아준다.
			BIT bitSideSearchFilter = GetFilterSideCast( GetThisRecv(),
													castTarget,
													pEffect->castfiltFriendship );
			int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
			GetListObjsRadius( pAryOutCastingTarget,
								*ppOutBaseTarget,	// 원의 중심이 되는 타겟
								XE::VEC2(),			// 원의 중심 좌표
								pEffect->castRange,
								bitSideSearchFilter,
								numApply,
								TRUE );
		} else
		if( pEffect->castTargetRange == xTR_GROUP )
		{
			// 기준타겟으로부터 그룹내 오브젝트들을 의뢰한다.
			(*ppOutBaseTarget)->GetGroupList( pAryOutCastingTarget,
											pUseSkill,
											pEffect );
		}
		// 시전대상:자신의 경우도 IsCastAble을 써야할듯 하다.
// 		if( pEffect->castRange == 0 )
// 			pAryOutCastingTarget->Add( *ppOutBaseTarget );
// 		else
// 		{
// 			// 시전반경이 있으면 자동으로 범위로 캐스팅 타겟을 뽑아준다.
// 			GetCastingTargetList( pAryOutCastingTarget, 
// 									pvOutTarget, 
// 									// 타겟반경개체 로 뽑음
// 									xCST_TARGET_RADIUS_OBJ,	
// 									pUseSkill,
// 									pEffect,
// 									ppOutBaseTarget, 
// 									pvPos );
// 		}
		break;
	// 단일대상
	case xCST_OTHER:	
// 		if( XBREAK( *ppOutBaseTarget == NULL ) )			// IsInvalidCastingTarget()을 이미 거쳤기때문에 여기에서 널이되는경우는 없어야 한다
// 			return 0;
		// 하위 클래스에 요청해서 조건에 맞는 타겟을 찾아서 돌려줘야 한다.
		*ppOutBaseTarget = GetTargetObject( pEffect, pEffect->castTargetCond );
		// 범위가 없으면 베이스타겟이 곧 시전대상이 된다.		
		if( pEffect->castTargetRange == xTR_ONE )
		{
			pAryOutCastingTarget->Add( *ppOutBaseTarget );
		} else
		if( pEffect->castTargetRange == xTR_CIRCLE )
		{
			// 코딩만 하고 테스트 아직 안됨
			// 시전반경이 있으면 자동으로 범위로 캐스팅 타겟을 뽑아준다.
			BIT bitSideSearchFilter = GetFilterSideCast( GetThisRecv(),
													castTarget,
													pEffect->castfiltFriendship );
			int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
			GetListObjsRadius( pAryOutCastingTarget,
								*ppOutBaseTarget,	// 원의 중심이 되는 타겟
								XE::VEC2(),			// 원의 중심 좌표
								pEffect->castRange,
								bitSideSearchFilter,
								numApply,
								TRUE );
		} else
		if( pEffect->castTargetRange == xTR_GROUP )
		{
			// 기준타겟으로부터 그룹내 오브젝트들을 의뢰한다.
			if( *ppOutBaseTarget )
				( *ppOutBaseTarget )->GetGroupList( pAryOutCastingTarget,
													pUseSkill,
													pEffect );
		}

// 		if( pEffect->castRange == 0 )
// 			pAryOutCastingTarget->Add( *ppOutBaseTarget );
// 		else
// 		{
// 			// 시전반경이 있으면 자동으로 범위로 캐스팅 타겟을 뽑아준다.
// 			GetCastingTargetList( pAryOutCastingTarget, 
// 									pvOutTarget, 
// 									// 타겟반경개체 로 뽑음
// 									xCST_TARGET_RADIUS_OBJ,	
// 									pUseSkill,
// 									pEffect,
// 									ppOutBaseTarget, 
// 									pvPos );
// 		}
		break;
	// 타겟반경개체
	case xCST_TARGET_RADIUS_OBJ:
		{
			XBREAK( *ppOutBaseTarget == NULL );
			BIT bitSideSearchFilter = GetFilterSideCast( GetThisRecv(), 
												castTarget, 
												pEffect->castfiltFriendship );
			int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
			GetListObjsRadius( pAryOutCastingTarget, 
								*ppOutBaseTarget,	// 원의 중심이 되는 타겟
								XE::VEC2(),			// 원의 중심 좌표
								pEffect->castRange, 
								bitSideSearchFilter, 
								numApply, 
								TRUE );
		}
		break;
	case xCST_GROUND:			// 바닥
	case xCST_TARGET_GROUND:	// 타겟바닥, 타겟이 서있는 바닥의 좌표
		{
			// 바닥대상 스킬을 받을 바닥객체를 받아온다.
			XSkillReceiver *pGround = GetGroundReceiver();	// virtual
			XBREAK( pGround == NULL );
			pAryOutCastingTarget->Add( pGround );
			// 바닥타겟인데 좌표가 안정해졌을땐 디폴트로 시전자좌표를 사용
			if( pvPos == NULL )
				*pvOutTarget = GetCurrentPosForSkill();
			// 바닥을 타겟으로 할땐 시전범위를 사용하지 않는다.
			break;
		}
	// 바닥반경개체, 
	// 좌표를 파라메터로 받아서 좌표+반경이내의 조건에 맞는 타겟을 추려서 캐스팅 타겟으로 한다.
	case xCST_GROUND_RADIUS_OBJ:	
		{
			if( XBREAK( pvPos == NULL ) )
				return 0;
			BIT bitSideSearchFilter = GetFilterSideCast( GetThisRecv(), 
												castTarget, 
												pEffect->castfiltFriendship );
			int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
			XE::VEC2 vCenter;
			if( pvPos )
				vCenter = *pvPos;
			GetListObjsRadius( pAryOutCastingTarget, 
								NULL,		// 원의 중심이 되는 타겟
								vCenter,			// 원의 중심 좌표
								pEffect->castRange, 
								bitSideSearchFilter, 
								numApply, 
								TRUE );
		}
		break;
	case xCST_PARTY:
		{
			BIT bitSideSearchFilter = GetFilterSideCast( GetThisRecv(), 
												castTarget, 
												pEffect->castfiltFriendship );
			xtTargetFilter tfFilter = MakeTargetFilter( pEffect );
			// sideFilter에 해당하는 파티멤버를 virtual로 받아온다.
			GetListPartyMember( pAryOutCastingTarget, bitSideSearchFilter, tfFilter );
		}
		break;
	default:
		// 이 멤버함수를 상속받아 커스텀정의된 시전대상을 처리하시오
		XBREAKF( 1, "%d: 처리안된 castTarget", castTarget );
	}
	return pAryOutCastingTarget->size();
}

// 스킬발동대상얻기
// this는 시전자
int XSkillUser::GetInvokeTarget( 
						XArrayLinearN<XSkillReceiver*, 100> *plistOutInvokeTarget,		// 결과를 이곳에 받습니다
						XSkillDat *pBuff,
						xtInvokeTarget invokeTarget,		// virtual이라서 사용자쪽에서 어떤변수를 써야하는지 분명히 알게 하기위해 직접 변수를 넘김
						const EFFECT *pEffect,				// 효과.		
						XSkillReceiver *pCastingTarget,		// 타겟(시전대상)
						const XE::VEC2 *pvPos )						// 좌표
{
//	XBREAK( plistOutInvokeTarget->size() > 0 );	// 이미 리스트에 뭔가 있으면 에러다

//	if( m_CastMethod == xPASSIVE )		// 이거 일단 필요없을듯 해서 뺏다
//	if( m_CastMethod == xACTIVE )		
	xtFriendshipFilt filtIvkFriendship = pEffect->invokefiltFriendship;
	if( filtIvkFriendship == xfNONESHIP )
		filtIvkFriendship = pEffect->castfiltFriendship;
	XBREAK( filtIvkFriendship == xfNONESHIP );
	//
	switch( invokeTarget )
	{
	// 시전대상자
	case xIVT_SELF:
		// 발동대상 자신이라도 발동필터는 적용되야 한다
		if( IsInvokeAble( pCastingTarget, pEffect ) )	
			plistOutInvokeTarget->push_back( pCastingTarget );	
		break;
/*	case xAREA:	// 지역-or 연산이 가능
		{
			if( XBREAK( pvPos == NULL ) )
				return 0;
			int friendship = GetFilterSide( pCastingTarget, pEffect->invokeTarget, pEffect->invokefiltFriendship );
			// pvPos를 중심으로 발동반경내의 friendship의 우호를 가진...
			// 이 조건에 맞는 오브젝트들 리스트 invokeNumApply만큼을 버추얼로 요청한다
			// 아직 구현안되어 있음
			GetListObjsInArea( plistOutInvokeTarget, pvPos, pEffect->invokeRadius, (xtFriendshipFilt)friendship, pEffect->invokeNumApply );		
			return plistOutInvokeTarget->size();
		} */
	case xIVT_RADIUS:			// 반경내
	case xIVT_SURROUND:		// 주변
		{
			XSkillUser *pCaster = this;
			XSkillReceiver *pInvoker = pCastingTarget;
			if( pInvoker == NULL || pInvoker->GetCamp() == 0/*xSIDE_NONE*/ )
				pInvoker = pCaster->GetThisRecv();
			XBREAK( pInvoker == NULL );
/*
			DWORD sideSearchFilt = GetFilterSideInvoke( pCaster, 
												pInvoker, 
												pEffect->invokeTarget, 
												pEffect->invokefiltFriendship );
*/
			// 발동대상우호가 시전대상의 입장에서 우호를 비교하던것에서
			// 시전자의 입장에서 우호를 비교하는것으로 바뀜.
			BIT bitSideSearchFilt = GetFilterSideInvoke( pCaster, 
												pCaster->GetThisRecv(), 
												pEffect->invokeTarget, 
												filtIvkFriendship );
												
			BOOL bIncludeCenter = (invokeTarget == xIVT_RADIUS )? TRUE : FALSE;	// RADIUS(반경내)의 경우는 센터타겟을 포함하고 서라운드는 포함하지 않는다
			// 발동자를 중심으로(발동자를 포함하거나/제외하거나) 반경내 sideSearchFilt에 해당하는
			// side편 대로 검색하여 invokeNumApply개의 리스트를 요청한다
			// 아직 구현안되어 있음
			XE::VEC2 vCenter;
			if( pvPos )
				vCenter = *pvPos;
			XSkillReceiver *pCenter = NULL;
			// 캐스팅타겟이 바닥의 경우 포인터와 좌표 둘다 넘어오기때문에 이젠 둘다 넘어오는 처리도 해야함.
//			if( vCenter.IsZero() )
				pCenter = pCastingTarget;
			int numApply = pEffect->invokeNumApply;	// 0이면 제한없음.
			GetListObjsRadius( plistOutInvokeTarget, 
							pCenter,		// 원의 중심이 되는 타겟
							vCenter,			// 원의 중심 좌표
							pEffect->invokeRadius, 
							bitSideSearchFilt, 
							numApply, 
							bIncludeCenter );
			break;
		}
	case xIVT_LINE:
		{
			BIT bitSideSearchFilt = GetFilterSideInvoke( this, 
												pCastingTarget, 
												pEffect->invokeTarget, 
												filtIvkFriendship );
			XBREAK( pvPos == NULL );
			// pvPos가 널이면 상속받는 하위클래스에서 this의 좌표를 직접 구해서 시작
			// 좌표로 써야 한다.
			if( pvPos )
			{
				GetListObjsInRect( plistOutInvokeTarget,
									pvPos,
									pEffect->invokeSize,
									bitSideSearchFilt,
									pEffect->invokeNumApply );
			}
		}
		break;
	case xIVT_PARTY:
		{
			BIT bitSideSearchFilt = GetFilterSideInvoke( this, 
												pCastingTarget, 
												pEffect->invokeTarget, 
												filtIvkFriendship );
			xtTargetFilter tfFilter = MakeTargetFilter( pEffect );
			// sideFilter에 해당하는 파티멤버를 virtual로 받아온다.
			GetListPartyMember( plistOutInvokeTarget, bitSideSearchFilt, tfFilter );
		}
		break;
	// 피격자
	case xIVT_DEFENDER:
		if( m_idEventHitter )
		{
			XDelegateSkill *pDelegate = GetpDelegate();
			if( pDelegate )
			{
				// 피격자가 세팅되어있다면 델리게이터로 실제 포인터를 얻어와서 발동대상 리스트에 넣어준다.
				XSkillReceiver *pTarget = pDelegate->GetTarget( m_idEventHitter );
				if( pTarget )
					plistOutInvokeTarget->Add( pTarget );
			}
			// 한번 사용하면 클리어
			m_idEventHitter = 0;
		}
		break;
	// 피격자
	case xIVT_ATTACKER:
		if( m_idEventAttacker )
		{
			XDelegateSkill *pDelegate = GetpDelegate();
			if( pDelegate )
			{
				// 피격자가 세팅되어있다면 델리게이터로 실제 포인터를 얻어와서 발동대상 리스트에 넣어준다.
				XSkillReceiver *pTarget = pDelegate->GetTarget( m_idEventAttacker );
				if( pTarget )
					plistOutInvokeTarget->Add( pTarget );
			}
			// 한번 사용하면 클리어
			m_idEventAttacker = 0;
		}
		break;
	}
	return plistOutInvokeTarget->size();
}


xtError XSkillUser::ApplyEffect( XSkillObj *pUseSkill, 
								XSkillReceiver *pTarget,
								XE::VEC2 *pvPos )
{
	// 효과 사용
	EFFECT_LOOP( pUseSkill, itor, pEffect )
	{
		xtError err;
		if( ( err = UseEffect( pUseSkill, pEffect, pTarget, pvPos ) ) != xOK )
			return err;
	}
	END_LOOP;
	return XSKILL::xERR_OK;
}
/* 중복시전에 관한 여러가지 케이스
	1. 시전대상이 같은 효과가 2개 있을경우 최초버프: 정상
	2. 시전대상이 다른 효과가 2개 있을경우 최초버프: 정상
	3. 시전대상이 같은 효과가 2개있는 스킬을 내가 한번더 쓸때: 시간만 리셋되고 정상시전됨
	4. 시전대상이 다른 효과가 2개있는 스킬을 내가 한번더 쓸때: 상동
	if( 중복불가 )
		3. 시전대상이 같은 효과가 2개 있는경우 다른사람의 같은버프가 이미 걸려있는경우: 그 버프에 시간만 리셋시킬수 있다.
		4. 시전대상이 다른 효과가 2개 있을경우 다른사람의 같은버프가 이미 걸려있는경우: 그 버프에 시간은 리셋되고, 다른대상에 걸리는 버프는 새로 생성된다
	if( 중복가능 )
		5. 시전대상이 같은 효과가 2개 있는경우 다른사람의 같은버프가 이미 걸려있는경우: 내버프가 따로 걸리면서 정상시전됨
		6. 시전대상이 다른 효과가 2개 있을경우 다른사람의 같은버프가 이미 걸려있는경우
		*/
// 시전대상에게 효과시전
xtError XSkillUser::CastEffectToCastingTarget( 
								XSkillObj *pUseSkill, 
								EFFECT *pEffect, 
								XSkillReceiver *pBaseTarget, // 광역일경우 광역의 기준이 되는 중심 타겟
								XSkillReceiver *pCastingTarget, // 실제 광역범위내에서 스킬을 받는 타겟들.
								const XE::VEC2 *pvPos )		
{
	XBREAK( pUseSkill == NULL );
	XBREAK( pEffect == NULL );
	// 지속시간형 타입인가
	if( pUseSkill->GetpDat()->IsBuff(pEffect) )	
	{
		// 지역형 도트데미지(파이어레인 류)는 시전자에게 버프를 걸고 버프프로세스에서 영역에 데미지를 준다
//		if( pEffect->invokeTarget == xAREA )	<<- 이런게 왜 필요함?
//			pCastingTarget = GetThisRecv();			
		
		// 발사체의 형태로 시전대상에게 날아오다 시전대상이 사라지면 대상이 널이 될수 있음
		if( pCastingTarget )
		{
			XBuffObj *pSkillBuff = NULL;
			BOOL bCreateBuff = FALSE;
			// 사용하려는 스킬의 버프객체가 시전대상에 이미 있는지 검사. 
			// 시전자가 다른 같은버프가 중복으로 걸릴수도 있으므로 시전자까지 같아야 한다
			if( NULL == (pSkillBuff = pCastingTarget->FindBuffSkill( pUseSkill->GetidSkill(), this )) )
			{	
				// 시전자도 같고 아이디도 같은 버프는 없다. 그렇다면
				// 시전자는 달라도 아이디는 같은 버프는 있는가?
				pSkillBuff = pCastingTarget->FindBuffSkill( pUseSkill->GetidSkill() );
				if( pSkillBuff )
				{	// 시전자는 다르고 아이디만 같은 경우
					if( pEffect->bDuplicate == FALSE ) {
						// 중복불가의 경우는 이렇게....
						// 이런경우는 기존버프에 시간만 리셋해준다
					}
					else
						// 중복가능한 스킬이면 버프를 새로 생성한다
						bCreateBuff = TRUE;
				} else
					// 아이디 같은 버프도 없다
					bCreateBuff = TRUE;
			}
			// 지속시간형 객체를 생성한다.
			if( bCreateBuff )
			{
				XBREAK( pSkillBuff != NULL );
				// 버프오브젝트만 일단 생성하고 이펙트오브젝트는 시전대상에 맞는것만 따로 추가한다
				pSkillBuff = CreateSkillBuffObj( this, 
												pCastingTarget, 
												pUseSkill->GetpDat(), 
												pvPos );	
				XASSERT( pSkillBuff );	
				// 시전대상의 버프리스트에 추가
				pCastingTarget->AddSkillRecvObj( pSkillBuff );	
				// 시전대상에게 버프가 추가된 직후 이벤트가 발생한다.
				pCastingTarget->OnAddSkillRecvObj( pSkillBuff, pEffect );		
				/*
				공격력증가,아이스발동 등은 발동시점 지속
				아이스처리등도 토글로 처리할게 아니라 보정치처럼 매프레임 클리어후
				재 세팅해야한다.
				영역안에서만 효과를 발휘하는 얼음바닥 같은 처리를 위해서다.
				*/
			}
			XBREAK( pSkillBuff == NULL );
			// 효과를 추가하고 타이머를 작동시킴
			// 이펙트오브젝트를 추가. 이미 효과가 있으면 기존거를 리턴
			EFFECT_OBJ *pEffObj = pSkillBuff->AddEffect( pEffect );			
			// "시전"스크립트 실행
			pSkillBuff->ExecuteScript( GetThisRecv(), pEffect->scriptCast.c_str() );		
			// 시전사운드 플레이
			if( pEffect->idCastSound )
				OnPlaySoundUse( pEffect->idCastSound );		// virtual
			// 타이머 켬
			if( pUseSkill->IsPassive() == FALSE )		// 지속시간 무한대는 타이머가 켜지지 않는다
			{ 
				// 지속시간 타이머를 셋
				if( pEffect->secDuration ) {
					pEffObj->timerDuration.Set( pEffect->secDuration );		
					pEffObj->cntDot = 0;
				}
			}
			// 버프객체에 이벤트 발생
			pSkillBuff->OnCastedEffect( pCastingTarget, pEffObj );
			// DOT효과가 있다면 도트타이머 셋
//			if( pEffect->secInvokeDOT > 0 )
//				pEffObj->timerDOT.Set( pEffect->secInvokeDOT );			
		} // if(pCastingTarget)
	} else
	{	// 즉시발동형(지속시간 0)
		// 발동대상들에게 즉시 효과가 가해진다.
		XArrayLinearN<XSkillReceiver*, 100> listInvokeTarget;
		// 1회성이기때문에 루아도 쓰고 바로 파괴되는걸로 함
		XLuaSkill *pLua = NULL;
		// "시전"스크립트 실행
		if( pEffect->scriptCast.empty() == false ) 
		{	// 스크립이 있을때만
			pLua = CreateScript();	// virtual
			pLua->RegisterScript( this, pCastingTarget );
			pLua->TargetDoScript( GetThisRecv(), pEffect->scriptCast.c_str() );
			SAFE_DELETE( pLua );
		}
		// 발동대상 얻음
		if( GetInvokeTarget( &listInvokeTarget, 
							pUseSkill->GetpDat(), 
							pEffect->invokeTarget, 
							pEffect, 
							pCastingTarget, 
							pvPos ) > 0 )
		{
			// 시작스크립트 실행. 
			if( pEffect->scriptInit.empty() == false ) // 스크립이 있을때만
			{			
				if( pLua == NULL )
				{
					pLua = CreateScript();	// virtual
					pLua->RegisterScript( this, pCastingTarget );
				}
			}
			// 발동대상들에게 효과 적용
			XARRAYLINEARN_LOOP( listInvokeTarget, XSkillReceiver*, pInvokeTarget )
			{
				// 효과적용
				pInvokeTarget->ApplyInvokeEffect( this, GetThisRecv(), NULL, pEffect );
				// SFX생성
				if( pEffect->strInvokeEffect.empty() == false )
				{
					float secLife = 0.f;	// 1회성 스킬이라 반복이 필요없을거 같아서 무조건 ONCE로 작동
					xtPoint pointSfx = pEffect->invokeEffectPoint;
					if( pointSfx == xPT_NONE )
						pointSfx = xPT_TARGET_BOTTOM;
					// 1회성이기때문에 id를받아둘필요는 없다
					pInvokeTarget->OnCreateInvokeSFX( NULL,
												pEffect,
												this,
												pvPos,
												pEffect->strInvokeEffect.c_str(), 
												(pEffect->idInvokeEffect==0)? 1 : pEffect->idInvokeEffect, 
												pointSfx,
												0.f );	
					if( pEffect->idInvokeSound )
						OnSkillPlaySound( pEffect->idInvokeSound );
				}
				// "발동시작"스크립 실행
				if( pLua )
					pLua->InvokeDoScript( pInvokeTarget, pEffect->scriptInit.c_str() );
			} END_LOOP;
		}
		SAFE_DELETE( pLua );
	}
	// 베이스타겟에게 sfx발동
// 	if( pEffect->strTargetEffect.empty() == false )
// 	{
// 		float secLife = 0.f;
// 		if( pEffect->targetEffectPlayMode == xAL_ONCE )
// 			secLife = 0;
// 		// 디폴트는 한번만 플레이하는걸로 바뀜
// 		else if( pEffect->targetEffectPlayMode == xAL_NONE )
// 			secLife = 0;
// 		// 반복으로 지정해두면 지속시간동안 반복된다.
// 		else if( pEffect->targetEffectPlayMode == xAL_LOOP )
// 			secLife = pEffect->secDuration;
// 		xtPoint pointSfx = pEffect->targetEffectPoint;
// 		// 이펙트생성지점이 정해져있지 않으면 디폴트로 타겟 아래쪽에
// 		if( pointSfx == xPT_NONE )
// 			pointSfx = xPT_TARGET_BOTTOM;
// 		// sfx만드는것도 캐릭터,스테이지 다 따로 virtual만드니 코드가 중복된다.
// 		// 델리게이트로 일원화 해야할듯.
// 		XBREAK( pBaseTarget == NULL && pvPos == NULL );
// 		XBREAK( pBaseTarget == NULL );
// 		// 타겟이 좌표인경우 베이스타겟은 없다.
// 		if( pBaseTarget )
// 		{
// 			// 이런경우가 있나?
// 			if( XBREAK( pBaseTarget->GetCurrentPosForSkill().IsZero() == FALSE && pvPos ) )
// 				pvPos = NULL;
// 			pBaseTarget->OnCreateInvokeSFX( NULL,
// 											pEffect,
// 											this,	// caster
// 											pvPos,
// 											pEffect->strTargetEffect.c_str(),
// 											(pEffect->idTargetEffect==0)? 1 : pEffect->idTargetEffect,
// 											pointSfx,
// 											secLife );
// 		}
// 	}


	// 시전대상에게 sfx발동
	if( pEffect->strCastTargetEffect.empty() == false && pCastingTarget )
	{
		float secLife = 0.f;
		// 기본적으로 지속시간이 있는 스킬이면 이펙트도 지속시간동안 루핑된다.
//		if( pEffect->secDuration > 0 )	
//			secLife = pEffect->secDuration;
		// 직접 반복금지라고 지정하면 지속시간이 있어도 한번만 플레이 한다.
		if( pEffect->castTargetEffectPlayMode == xAL_ONCE )
			secLife = 0;
		// 디폴트는 한번만 플레이하는걸로 바뀜
		else if( pEffect->castTargetEffectPlayMode == xAL_NONE )
			secLife = 0;
		// 반복으로 지정해두면 지속시간동안 반복된다.
		else if( pEffect->castTargetEffectPlayMode == xAL_LOOP )
			secLife = pEffect->secDuration;
		xtPoint pointSfx = pEffect->castTargetEffectPoint;
		if( pointSfx == xPT_NONE )
			pointSfx = xPT_TARGET_BOTTOM;
/*
		XSkillReceiver, XSkillUser를 한단계 더 상속받아 바닥전용 클래스를 만들고
		스테이지를 이것으로 상속받아 좀더 전용으로 쓸수 있게 한다.
		그다음. 리시버에게 자기 좌표를 요청해서 제로가 돌아오면 pvPos에 생성할수 있도록
		해주고, 리시버가 자기 좌표를 가지고 있고 pvPos가 널이면 리시버에 생성될수 있도록
		한다. 리시버가 자기좌표를 가지고 있고 pvPos도 갖고 있는 경우도 있나?
*/
		XE::VEC2 vPos;
		if( pvPos )
			vPos = *pvPos;
		XE::VEC2 vPosTarget = pCastingTarget->GetCurrentPosForSkill();
		// 좌표도 따로 주어지고 시전대상도 자기좌표를 갖고 있을땐 시전대상의 좌표로 생성하게 한다.
		if( pvPos && vPosTarget.IsZero() == FALSE )
			pvPos = NULL;
		XBREAK( vPosTarget.IsZero() && 
				pvPos == NULL &&
				pointSfx != xPT_WINDOW_CENTER &&
				pointSfx != xPT_WINDOW_CENTER_BOTTOM );
		// sfx만드는것도 캐릭터,스테이지 다 따로 virtual만드니 코드가 중복된다.
		// 델리게이트로 일원화 해야할듯.
		pCastingTarget->OnCreateInvokeSFX( NULL,
											pEffect,
											this,	// caster
											pvPos,
											pEffect->strCastTargetEffect.c_str(),
											(pEffect->idCastTargetEffect==0)? 1 : pEffect->idCastTargetEffect,
											pointSfx,
											secLife );
	}
	if( pEffect->idCastSound )
		OnSkillPlaySound( pEffect->idCastSound );

	return xOK;
}
// 효과발동시도
xtError	XSkillUser::TryCastEffect( XSkillObj *pUseSkill, 
								EFFECT *pEffect, 
								XSkillReceiver *pBaseTarget, 
								XSkillReceiver *pCastingTarget, 
								const XE::VEC2 *pvPos )	
{
	// 발사체방식인가
	if( XE::IsHave( pEffect->strShootObj.c_str() ) || pEffect->idShootObj )
	{
		// 타겟까지 날아가서 시전자->스킬발동()을 한다
		// virtual. 발사체오브젝트를 생성하고 오브젝트 매니저에 등록한다
		XSkillUser *pShootObj = CreateAndAddToWorldShootObj( pUseSkill, 
															pEffect, 
															pCastingTarget, 
															this, 
															pvPos );		
		// 타겟에 도달하면 CastEffectToCastingTarget( 넘겨줬던 파라메터 )를 해준다.
		XASSERT( pShootObj );
	} else
	{	// 즉시발동 or 버프방식
		CastEffectToCastingTarget( pUseSkill, pEffect, pBaseTarget, pCastingTarget, pvPos );
	}
	return xOK;
}

// 시전대상에게 효과사용
// 시전대상이 오브젝트가 아니라 지역형태라면 스테이지객체를 시전대상으로 해야한다.
// 스테이지 객체는 물론 XSkillReceiver를 상속받아야 한다.
xtError XSkillUser::UseEffect( XSkillObj *pUseSkill, 
								EFFECT *pEffect, 
								XSkillReceiver *pBaseTarget, 
								const XE::VEC2 *pvPos )
{
	// 시전자 이펙트(시전시작 시점에 발생되는 이펙트)
	if( pEffect->strCasterEffect.empty() == false )
	{
		xtPoint pointSfx = pEffect->casterEffectPoint;
		if( pointSfx == xPT_NONE )
			pointSfx = xPT_TARGET_BOTTOM;
		xRPT_TYPE playMode = xRPT_1PLAY;
		if( pEffect->secDuration > 0 )
			playMode = xRPT_LOOP;
		OnCreateCasterSfx( pEffect,
							pointSfx,
							pEffect->strCasterEffect.c_str(),
							(pEffect->idCasterEffect==0)? 1 : pEffect->idCasterEffect,
							// 만약 시전시간이 있다면 시전시간동안 루핑으로 돌려야 한다.
							pEffect->secDuration );	
	}

	XArrayLinearN<XSkillReceiver*, 100> listCastingTargets;		// 시전대상
	XE::VEC2 vPosTarget;
	// 캐스팅 대상을 얻는다.
	if( GetCastingTargetList( &listCastingTargets, 
								&vPosTarget,
								pEffect->castTarget,
								pUseSkill, 
								pEffect, 
								&pBaseTarget, 
								pvPos ) > 0 )
//								pvPos ) == 1 )
	{
		// "사용"스크립트 실행
		if( XE::IsHave( pEffect->scriptUse.c_str() ) )	// 스크립이 있을때만
		{			
			XLuaSkill *pLua =  CreateScript();	// virtual
			pLua->RegisterScript( this, NULL );
			pLua->TargetDoScript( GetThisRecv(), pEffect->scriptUse.c_str() );
			SAFE_DELETE( pLua );
		}  
		// 캐스팅타겟을 얻으며 타겟좌표가 바뀔수도 있다.
		if( pvPos )
			vPosTarget = *pvPos;
		if( pEffect->strTargetEffect.empty() == false )
		{
			float secLife = 0.f;
			if( pEffect->targetEffectPlayMode == xAL_ONCE )
				secLife = 0;
			// 디폴트는 한번만 플레이하는걸로 바뀜
			else if( pEffect->targetEffectPlayMode == xAL_NONE )
				secLife = 0;
			// 반복으로 지정해두면 지속시간동안 반복된다.
			else if( pEffect->targetEffectPlayMode == xAL_LOOP )
				secLife = pEffect->secDuration;
			xtPoint pointSfx = pEffect->targetEffectPoint;
			// 이펙트생성지점이 정해져있지 않으면 디폴트로 타겟 아래쪽에
			if( pointSfx == xPT_NONE )
				pointSfx = xPT_TARGET_BOTTOM;
			// sfx만드는것도 캐릭터,스테이지 다 따로 virtual만드니 코드가 중복된다.
			// 델리게이트로 일원화 해야할듯.
			XBREAK( pBaseTarget == NULL && pvPos == NULL );
			XBREAK( pBaseTarget == NULL );
			// 타겟이 좌표인경우 베이스타겟은 없다.
			if( pBaseTarget )
			{
				// 이런경우가 있나?
				if( XBREAK( pBaseTarget->GetCurrentPosForSkill().IsZero() == FALSE && pvPos ) )
					pvPos = NULL;
				pBaseTarget->OnCreateInvokeSFX( NULL,
					pEffect,
					this,	// caster
					pvPos,
					pEffect->strTargetEffect.c_str(),
					( pEffect->idTargetEffect == 0 ) ? 1 : pEffect->idTargetEffect,
					pointSfx,
					secLife );
			}
		}
		// 캐스팅 대상들의 루프
		XARRAYLINEARN_LOOP( listCastingTargets, XSkillReceiver*, pCastingTarget )
		{
			TryCastEffect( pUseSkill, pEffect, pBaseTarget, pCastingTarget, 
							(vPosTarget.IsZero())? NULL: &vPosTarget );		// 효과 발동 시도
		} END_LOOP
	} else
		return xERR_NOT_FOUND_CASTING_TARGET;		// 시전대상을 찾지 못함
	return xOK;

}

// 스킬 최초 사용은 이걸로부터
xtError	XSkillUser::UseSkill( XSkillObj *pUseSkill )
{
	return xOK;
}

// 스킬사용
xtError	XSkillUser::UseSkill( XSkillObj *pUseSkill, 
	XSkillReceiver *pTouchTarget,	// 외부에서 커서로 타겟을 찍었을 경우
	XE::VEC2 *pvTouchPos )			// 외부에서 커서로 좌표를 찍었을 경우
{
	return UseSkill( pUseSkill, 
					pUseSkill->GetpDat()->GetCastMethod(), 
					pTouchTarget,
					pvTouchPos );
}

// 스킬사용
xtError	XSkillUser::UseSkill( XSkillObj *pUseSkill, 
							xCastMethod castMethod,
							XSkillReceiver *pTouchTarget,	// 외부에서 커서로 타겟을 찍었을 경우
							XE::VEC2 *pvTouchPos )			// 외부에서 커서로 좌표를 찍었을 경우
{
	if( XBREAK( pUseSkill == NULL ) )
		return xERR_CRITICAL_ERROR;
	// 쿨타임 검사
	if( pUseSkill->GettimerCool().IsOn() )				// 쿨타임이 꺼져있으면 쿨타임 다 돈거니까 걍 통과
	{
		if( pUseSkill->GettimerCool().IsOver() == FALSE )		// 쿨링이 아직 안끝났으면
			return xERR_READY_COOLTIME;			// 재사용 대기중입니다
		pUseSkill->GettimerCool().Off();	// 쿨링타이머 끔
	}
	XSkillDat *pSkillDat = pUseSkill->GetpDat();
	// 이미 시전된 패시브는 다시 시전하지 않게 한다
	if( castMethod == xPASSIVE ) {
		if( pUseSkill->GetbAlreadyCast() == TRUE )
			return xERR_ALREADY_APPLY_SKILL;			// 이미 시전된 스킬입니다
	}
	XSkillReceiver *pTarget = NULL;	// 시전기준타겟
	XE::VEC2 *pvPos = NULL;
	xtError err;
	// 타겟이나 좌표를 효과에 맞게 제대로 호출했는지 검사해준다.
	// 효과가 여러개이기때문에 시전대상유효성을 모두 통과해야 효과발동시도로 넘어간다
	/*
	타겟을 직접 찍는 방식의 게임에서는 게이머가 대상을 잘못찍을수 있기때문에 여기서
	걸러서 적당한 에러메시지를 내줘야 하지만, 자동으로 타겟을 지정하는 게임에서는
	프로그래머가 실수하지 않는이상 아래 검사는 의미가 없다.
	*/
	if( pTouchTarget || pvTouchPos )
	{
		pTarget = pTouchTarget;
		pvPos = pvTouchPos;
		// 외부에서 직접 타겟을 찍었을때만 유효여부를 검사해준다.
		EFFECT_LOOP( pUseSkill, itor, pEffect )
		{
			err = IsValidCastingTarget( pUseSkill, pEffect, pTouchTarget, pvTouchPos );
			if( err != xOK )
				return err;
		} END_LOOP;
	} else
	{
		// 모두 널로 왔으면 스킬AI가 모두 자동으로 타겟이나 좌표를 지정한다.
	}
	
	// 쿨타이머 작동-
	// 쿨타이머셋이 UseEffect보다 먼저 있는 이유: 
	// 만약 지역마법을 썼는데 그자리에 아무도 없었다면 캐스팅타겟 없음으로 해서 UseEffect가 리턴된다 
	// 그러나 이경우에도 쿨타이머는 돌아가야 하기때문이다
	if( pSkillDat->GetfCoolTime() != 0 )
		pUseSkill->GettimerCool().Set( pSkillDat->GetfCoolTime() );
	// 하위 클래스에 스킬사용 성공을 알려 mp깎기등의 처리를 할수 있도록 한다.
	OnSuccessUseSkill( pUseSkill );		// virtual;
	// 효과 사용
	EFFECT_LOOP( pUseSkill, itor, pEffect )
	{
		if( (err = UseEffect( pUseSkill, pEffect, pTarget, pvPos )) != xOK )
			return err;
	}
	END_LOOP;
	/*
		XSkillUser는 mp를 가질수 없다. 그러므로 Addmp함수도 가질수 없다
		CalcAdjParam()을 써야한다는 누군가에게 adj보정을 받았다는 얘기고 그건 XSkillReceiver성질에 해당하므로 이것을 상속받아야 한다.
		만약 트랩이 마법을 쓰고 mp같은걸 필요로 한다면 온리 XSkillUser만 상속받게 하고 mp는 상속받은 하위클래스에서 구현해주던가 아니면 최상위 XSkillUser/Recv가 모두 상속받는 공통부모 XBaseSkill클래스를 만들어서 그곳에 mp를 넣어두고 Adj는 하지못하게 한다. adj는 XSkillReceiver에 구현되어 있다
		AddMp()의 구현코드를 시스템에 포함시키려면 XSkillUser/Recv 둘다를 상속받는 XObjSkill클래스를 만들고 이곳에서 XSkillUser쪽의 UseSkill을 호출하고 리턴되면 XSkillReceiver쪽의 MP를 빼주는 식으로 해야할듯.
	*/
	// 패시브 시전이 성공하면 플래그 바꿈
	if( pUseSkill->GetpDat()->GetCastMethod() == xPASSIVE )
			pUseSkill->SetbAlreadyCast( TRUE );
	return xOK;
}

// 시전대상 유효성 검사
xtError XSkillUser::IsValidCastingTarget( XSkillObj *pUseSkill, 
										EFFECT *pEffect, 
										XSkillReceiver *pTarget, 
										XE::VEC2 *pvPos  )
{
	// 지역에 시전하는 경우
	if( pEffect->castTarget == xCST_GROUND ||
		pEffect->castTarget == xCST_GROUND_RADIUS_OBJ )		
		if( pvPos == NULL )					// 좌표가 지정되어있지 않으면
		{
			XLOGXN( "xERR_MUST_MAKE_AREA" );
			return xERR_MUST_MAKE_AREA;		// 영역을 지정해주십시요
		}
	if( pTarget )	
	{
		// 시전대상은 "자신"인데 타겟넘어온건 자신이 아닐때
		if( pEffect->castTarget == xCST_SELF )	
		{
			if( this->GetThisRecv() != pTarget )		
			{
				XLOGXN( "xERR_MUST_CAST_SELF" );
				return xERR_MUST_CAST_SELF;		// 자신에게 써야합니다.
			}
		} else
		// 타겟에게 시전하는 방식의 경우
		if( pEffect->castTarget == xCST_OTHER )			
		{
			// 적에게는 쓸수 없는스킬인데
			if( (pEffect->castfiltFriendship & xfHOSTILE) == 0 )
			{
				// 적에게는 쓸수 없는스킬인데
				// 타겟우호가 나랑다르고 타겟이 중립도 아닐때는 적용금지
				if( XECompCamp::sIsNotFriendly( pTarget->GetCamp(), GetCampUser() ) &&
					XECompCamp::sIsNeutrality( GetCampUser() ) == FALSE )
				{
					XLOGXN( "xERR_MUSTNOT_CAST_ENEMY" );
					return xERR_MUSTNOT_CAST_ENEMY;		// 적에겐 사용할수 없습니다
				}
/*
				// 타겟우호가 나랑다르고 타겟이 중립이 아닐때
				if( (GetCamp() != pTarget->GetCamp()) && 
					(pTarget->GetCamp() != xSIDE_NEUTRALITY) )			
				{
					XLOGXN( "xERR_MUSTNOT_CAST_ENEMY" );
					return xERR_MUSTNOT_CAST_ENEMY;		// 적에겐 사용할수 없습니다
				}
*/
			}
			// 아군에게는 쓸수 없는스킬인데
			if( (pEffect->castfiltFriendship & xfALLY) == 0 )
			{
				// 아군에게는 쓸수 없는스킬인데
				// 대상이 아군이면 적용금지
				if( XECompCamp::sIsFriendly( pTarget->GetCamp(), GetCampUser() ) )
				{
					XLOGXN( "xERR_MUSTNOT_CAST_FRIENDLY" );
					return xERR_MUSTNOT_CAST_FRIENDLY;	// 아군에게는 쓸수 없습니다.
				}
/*
				// 시전자와 타겟이 같은편이다면
				if( GetCamp() == pTarget->GetCamp() )		
				{
					XLOGXN( "xERR_MUSTNOT_CAST_FRIENDLY" );
					return xERR_MUSTNOT_CAST_FRIENDLY;	// 아군에게는 쓸수 없습니다.
				}
*/
			}
			// 중립에게는 쓸수 없는스킬인데
			if( (pEffect->castfiltFriendship & xfNEUTRALITY) == 0 )
			{
				// 중립에게는 쓸수 없는스킬인데 
				// 타겟이 중립이면 적용금지.
				if( XECompCamp::sIsNeutrality( GetCampUser() ) )
				{
					XLOGXN( "xERR_MUSTNOT_CAST_NEUTRALITY" );
					return xERR_MUSTNOT_CAST_NEUTRALITY;	// 중립에겐 사용할수 없습니다
				}
/*
				// 타겟이 중립이면
				if( xSIDE_NEUTRALITY == pTarget->GetCamp() )			
				{
					XLOGXN( "xERR_MUSTNOT_CAST_NEUTRALITY" );
					return xERR_MUSTNOT_CAST_NEUTRALITY;	// 중립에겐 사용할수 없습니다
				}
*/
			}
		}
	} else
	{	// pTarget == NULL
		if( pEffect->castTarget == xCST_OTHER )
		{
			XLOGXN( "xERR_MUST_SELECT_TARGET" );
			return xERR_MUST_SELECT_TARGET;		// 타겟을 지정해야합니다
		}
	}
	return xOK;
}


int XSkillUser::FrameMove( float dt )
{
	XLIST_LOOP( m_listUseSkill, XSkillObj*, pUseSkill )
	{
		// 액티브형을 제외한 패시브나 기타 어떤 조건에 의해 발동되는 스킬들은 여기서 발동시킨다.
		if( pUseSkill->IsCastCondition() == 1 )		{
			UseSkill( pUseSkill, NULL, NULL ); 
		}
		// 쿨타임 오버를 검사
		if( pUseSkill->GettimerCool().IsOver() ) {		// 쿨링시간이 끝나면
			OnCoolTimeOver( pUseSkill );				// virtual. 쿨타임시간이 끝나면 호출된다
			pUseSkill->GettimerCool().Off();			// 타이머 오프시킴
		}
	}
	END_LOOP;
	return 1;
}

// 패시브스킬을 실행(발동)시킨다.
void XSkillUser::InvokePassiveSkill( void )
{
	XLIST_LOOP( m_listUseSkill, XSkillObj*, pUseSkill )
	{
		// 액티브형을 제외한 패시브나 기타 어떤 조건에 의해 발동되는 스킬들은 여기서 발동시킨다.
		if( pUseSkill->IsPassive() )		
		{
			UseSkill( pUseSkill, NULL, NULL ); 
		}
	}
	END_LOOP;
}


// 시전대상우호가 시전대상에 비교하던것을 시전자와 비교하는것으로 바뀌었다.
BIT XSkillUser::GetFilterSideCast( XSkillReceiver *pCaster,	// XSkillUser로 바꿔야함.
									xtCastTarget targetType, 
									xtFriendshipFilt friendshipFilter )
{
	XBREAK( pCaster == NULL );
	BIT side = 0;
	if( targetType == xCST_SELF )	// 시전자가 자신이면
		side = pCaster->GetCamp();	// 시전자(자신)의 프렌드쉽을 검색한다
	else 
	{
		if( XECompCamp::sIsNeutrality( pCaster->GetCamp() ) )	// 시전자가 중립일때
		{
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				side = XECompCamp::sGetNeutralitySideFilter();
			// 중립의 적은 원래 없는데 표현이 애매해서 중립이 아닌편은 모두 적으로 간주
			if( friendshipFilter & xfHOSTILE )
				side = XECompCamp::sGetOtherSideFilter( pCaster->GetCamp() );
		} else
		{
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				side = XECompCamp::sGetSide( pCaster->GetCamp() );	// 시전자와 같은편 필터를 만든다.
			if( friendshipFilter & xfHOSTILE )
				side = XECompCamp::sGetOtherSideFilter( pCaster->GetCamp() ); // 시전자와 반대되는 Side필터를 만든다.
		}
		
// 		if( pCaster->GetCamp() == xSIDE_FRIEND ) 
// 		{	// 시전자가 아군
// 			if( friendshipFilter & xfALLY )		// 필터가 아군이면
// 				side |= xSIDE_FRIEND;			
// 			if( friendshipFilter & xfHOSTILE )
// 				side |= xSIDE_ENEMY;
// 		} else 
// 		if( pCaster->GetCamp() == xSIDE_ENEMY ) 	// 시전자가 적일때
// 		{
// 			if( friendshipFilter & xfALLY )	// 필터가 아군이면
// 				side |= xSIDE_ENEMY;
// 			if( friendshipFilter & xfHOSTILE )	// 필터가 적군이면
// 				side |= xSIDE_FRIEND;		// 플레이어가 적군이 된다 
// 		} else
// 		if( pCaster->GetCamp() == xSIDE_NEUTRALITY ) 	// 시전자가 중립일때
// 		{
// 			if( friendshipFilter & xfALLY )	// 필터가 아군이면
// 				side |= xSIDE_NEUTRALITY;			
// 			if( friendshipFilter & xfHOSTILE )
// 				side = 0;		// 중립의 적은 일단 없는걸로
// 		}
	}
	return side;
}
/*
// pTarget에 effectFilter로 시전 될때 검색해야할 대상의 프렌드쉽 플래그
DWORD XSkillUser::GetFilterSideCast( XSkillReceiver *pInvoker, 
									xtCastTarget targetType, 
									xtFriendshipFilt friendshipFilter )
{
	XBREAK( pInvoker == NULL );
	int side = 0;
	if( targetType == xCST_SELF )	// 시전대상이 자신이면
		side = pInvoker->GetFriendship();	// 시전대상(자신)의 프렌드쉽을 검색한다
	else {
		if( pInvoker->GetFriendship() == xSIDE_FRIEND ) 
		{	// 시전대상이 아군
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				side |= xSIDE_FRIEND;			
			if( friendshipFilter & xfHOSTILE )
				side |= xSIDE_ENEMY;
		} else 
		if( pInvoker->GetFriendship() == xSIDE_ENEMY ) {	// 시전대상이 적일때
			if( friendshipFilter & xfALLY )	// 필터가 아군이면
				side |= xSIDE_ENEMY;
			if( friendshipFilter & xfHOSTILE )	// 필터가 적군이면
				side |= xSIDE_FRIEND;		// 플레이어가 적군이 된다 
		} else
		if( pInvoker->GetFriendship() == xSIDE_NEUTRALITY ) {		// 시전대상이 중립일때
			if( friendshipFilter & xfALLY )	// 필터가 아군이면
				side |= xSIDE_NEUTRALITY;			
			if( friendshipFilter & xfHOSTILE )
				side = 0;		// 중립의 적은 일단 없는걸로
		}
	}
	return side;
}
*/

// 발동자(pInvoker)를 기준으로 sideFilter를 적용했을때 검색해야 하는 필터를 돌려준다.
// 발동자가 적일 경우 sideFilter가 "아군"이면 필터가 반대로 반대로 돌아가야하므로...
// this는 invoker
BIT XSkillUser::GetFilterSideInvoke( XSkillUser *pCaster, 
									XSkillReceiver *pInvoker, 
									xtInvokeTarget targetType, 
									xtFriendshipFilt friendshipFilter )
{
	XBREAK( pInvoker == NULL );
	XBREAK( pCaster == NULL );
	BIT bitSide = 0;
	// 발동자기준으로 비교하는것에서 시전자기준으로 우호를 비교하는것으로 바뀜
	if( targetType == xIVT_SELF )	// 시전자가 자신이면
		bitSide = pCaster->GetCampUser();	// 시전자(자신)의 프렌드쉽을 검색한다
	else
	{
		if( XECompCamp::sIsNeutrality( pCaster->GetCampUser() ) )	// 시전자가 중립일때
		{
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				bitSide = XECompCamp::sGetNeutralitySideFilter();
			// 중립의 적은 원래 없는데 표현이 애매해서 중립이 아닌편은 모두 적으로 간주
			if( friendshipFilter & xfHOSTILE )
				bitSide = XECompCamp::sGetOtherSideFilter( pCaster->GetCampUser() );
		}
		else
		{
			if( friendshipFilter & xfALLY )		// 필터가 아군이면
				bitSide = XECompCamp::sGetSide( pCaster->GetCampUser() );	// 시전자와 같은편 필터를 만든다.
			if( friendshipFilter & xfHOSTILE )
				bitSide = XECompCamp::sGetOtherSideFilter( pCaster->GetCampUser() ); // 시전자와 반대되는 Side필터를 만든다.
		}
	}
// 	xtSide capmCaster = pCaster->GetCamp();
// 	// 시전자가 지역같이 아군적군 소속이 없을땐 
// 	if( capmCaster == xSIDE_NONE )
// 		return xSIDE_ALL;		// 모든 우호대상이 타겟이 된다.
// 
// 	if( targetType == xIVT_SELF )	// 시전자가 자신이면
// 		side = capmCaster;	// 시전자(자신)의 프렌드쉽을 검색하도록 한다.
// 	else 
// 	{
// 		if( capmCaster == xSIDE_FRIEND ) {	// 시전자가 플레이어편
// 			if( friendshipFilter & xfALLY )		// 필터가 아군대상이면
// 				side |= xSIDE_FRIEND;		// 플레이어편을 검색해라.
// 			if( friendshipFilter & xfHOSTILE )	// 필터가 적군대상이면
// 				side |= xSIDE_ENEMY;		// 적편을 검색해라.
// 		} else 
// 		if( capmCaster == xSIDE_ENEMY ) {	// 시전자가 적일때
// 			if( friendshipFilter & xfALLY )	// 필터가 아군대상이면
// 				side |= xSIDE_ENEMY;			// 지네편(적편)을 검색해라.
// 			if( friendshipFilter & xfHOSTILE )	// 필터가 적군대상이면
// 				side |= xSIDE_FRIEND;		// 플레이어가 적군이 된다 
// 		} else
// 		if( capmCaster == xSIDE_NEUTRALITY ) {		// 시전자가 중립일때
// 			if( friendshipFilter & xfALLY )		// 필터가 아군이면
// 				side |= xfNEUTRALITY;		// 같은 중립편을 검색해라.	
// 			if( friendshipFilter & xfHOSTILE )	// 필터가 적이면
// 				side = xSIDE_FRIEND | xSIDE_ENEMY;	// 중립의 적은 모두가 적이다.
// 		}
// 
// 	}
	return bitSide;
}

// this가 pCstTarget에게 발동이 가능한지 검사
BOOL XSkillUser::IsInvokeAble( XSkillReceiver *pCstTarget, const EFFECT *pEffect )
{
/*
#pragma message( "발동대상 플레이어/AI는 발동대상진영 같은걸 따로 만들어서 해야할듯" )
	if( pEffect->invokefiltFriendship == xfAI ) {			// AI에게쓰는 스킬인데
		if( xAI != GetFriendship() )			// 발동타겟(this)이 AI가 아니면 실패
			return FALSE;					
		return TRUE;		// AI/PLAYER는 프렌드쉽을 검사할필요가 없음
	} else
	if( pEffect->invokefiltFriendship == xfPLAYER ) {	
		if( xPLAYER != GetFriendship() )			
			return FALSE;						
		return TRUE;
	}
*/
/*
	if( (pEffect->invokefiltPlayerType & xfAI) == 0 ) {			// AI에게 쓸수 없는 스킬인데
		if( xAI == GetPlayerType() )			// 타겟이 AI면 실패
			return FALSE;						
	} else
	if( (pEffect->invokefiltPlayerType & xfHUMAN ) == 0 ) {			// 인간플레이어에게 쓸수 없는 스킬인데
		if( xHUMAN == GetPlayerType() )			// 타겟이 인간이면 실패
			return FALSE;						
	}
*/
	xtFriendshipFilt invokefiltFriendship = pEffect->invokefiltFriendship;
	// 발동대상우호가 지정되지 않았으면 시전대상우호를 가져다 쓴다.
	// 스킬로드시에 미리 보정하도록 바뀜
//	if( invokefiltFriendship == xfNONESHIP )
//		invokefiltFriendship = pEffect->castfiltFriendship;
	// AI/PLAYER검사를먼저 하는게 좋을듯 하다. AI가 발동대상우호 AI같은거일때 아래에서 실패걸릴듯
	if( (invokefiltFriendship & xfHOSTILE) == 0 )	// 적에게는 쓸수 없는스킬인데
		// 시전대상이 중립도 아닌데 나랑 우호가 다르면(적) 쓸수 없음.
		if( XECompCamp::sIsNotFriendly( pCstTarget->GetCamp(), GetCampUser()) && 
			XECompCamp::sIsNeutrality( pCstTarget->GetCamp() ) )
			return FALSE;							// 적에겐 사용할수 없습니다
	if( (invokefiltFriendship & xfALLY) == 0 )		// 아군에게는 쓸수 없는스킬인데
		// 시전자와 타겟이 같은편이면 쓸수 없음.
		if( XECompCamp::sIsFriendly( pCstTarget->GetCamp(), GetCampUser() ) )
			return FALSE;					// 아군에게는 쓸수 없습니다.
	if( XECompCamp::sIsNeutrality(invokefiltFriendship ) )	// 중립에게는 쓸수 없는스킬인데
		// 발동타겟(this)이 중립이면 사용할수 없음.
		if( XECompCamp::sIsNeutrality( GetCampUser() ) )		
			return FALSE;						

	return TRUE;
}

XSkillObj* XSkillUser::FindUseSkillByID( ID idSkillDat ) 
{
	XLIST_LOOP( m_listUseSkill, XSkillObj*, pUseSkill )
	{
		if( pUseSkill->GetidSkill() == idSkillDat )
			return pUseSkill;
	}
	END_LOOP;
	return NULL;
}
/*
XSkillObj* XSkillUser::FindUseSkillByName( LPCTSTR szName ) 
{
	XSkillDat *pSkillDat;
	pSkillDat = m_prefSkillMng->FindByName( szName );
	if( XBREAKF( pSkillDat == NULL, "%s: skill not found!", szName ) )
		return NULL;
	XSkillObj *pUseSkill = FindUseSkillByID( pSkillDat->GetidSkill() );
	return pUseSkill;
}
*/
XSkillObj* XSkillUser::FindUseSkillByIdentifier( LPCTSTR szIdentifier ) 
{
	XSkillDat *pSkillDat;
	pSkillDat = m_prefSkillMng->FindByIdentifier( szIdentifier );
	if( XBREAKF( pSkillDat == NULL, "%s: skill not found!", szIdentifier ) )
		return NULL;
	XSkillObj *pUseSkill = FindUseSkillByID( pSkillDat->GetidSkill() );
	return pUseSkill;
}

xtError XSkillUser::UseSkillByID( XSkillObj **ppOutUseSkill, 
										ID idSkill, 
										XSkillReceiver *pTarget, 
										XE::VEC2 *pvPos ) 
{ 
	if( ppOutUseSkill ) 
		*ppOutUseSkill = NULL;
	XSkillObj *pUseSkill = FindUseSkillByID( idSkill );
	XASSERTF( pUseSkill, "idSkill:%d 스킬을 보유스킬목록에서 찾지못했습니다.", idSkill );
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	return UseSkill( pUseSkill, pTarget, NULL );
}
/*
xtError XSkillUser::UseSkillByName( XSkillObj **ppOutUseSkill, 
										LPCTSTR szSkillName, 
										XSkillReceiver *pTarget, 
										XE::VEC2 *pvPos ) 
{
	if( ppOutUseSkill ) 
		*ppOutUseSkill = NULL;
	XSkillObj *pUseSkill = FindUseSkillByName( szSkillName );
	XASSERTF( pUseSkill, "Skill:%s 스킬을 보유스킬목록에서 찾지못했습니다.", szSkillName );
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	return UseSkill( pUseSkill, pTarget, NULL );
}
*/
// 식별자로 스킬을 사용케한다. 만약 스킬이 없다면 스킬사용 오브젝트를 새로만들어
// 등록시키고 사용한다. 스킬발동에 사용한다.
xtError XSkillUser::UseSkillByIdentifier( LPCTSTR szIdentifier, 
										XSkillReceiver *pTarget, 
										XE::VEC2 *pvPos,
										XSkillObj **ppOutUseSkill ) 
{
	if( ppOutUseSkill ) 
		*ppOutUseSkill = NULL;
	
	XSkillObj *pUseSkill = CreateAddUseSkillByIdentifier( szIdentifier );
	if( XBREAK( pUseSkill == NULL ) )
		return xERR_CRITICAL_ERROR;
	if( ppOutUseSkill ) 
		*ppOutUseSkill = pUseSkill;
	// 액티브로 실행시킴. 스킬발동에 사용
	return UseSkill( pUseSkill, XSKILL::xACTIVE, pTarget, pvPos );
}

// 첫번째 보유스킬을 사용한다
xtError XSkillUser::UseSkill( XSkillReceiver *pTarget, 
										XE::VEC2 *pvPos ) 
{		
	if( XBREAK(m_listUseSkill.size() <= 0) )
		return xCANCEL;
	XSkillObj *pUseSkill = m_listUseSkill.GetFirst();
	return UseSkill( pUseSkill, pTarget, NULL );
}

NAMESPACE_XSKILL_END
