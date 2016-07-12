#include "stdafx.h"
#include "XFLevel.h"
#include "XArchive.h"
#include "XFLevelH.h"

#define VER_LEVEL_SERIALIZE		3

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

DWORD XFLevel::GetMaxExp( int level ) 
{
	if( m_pDelegate )
		return m_pDelegate->OnDelegateGetMaxExp( this, level, 0, 0 );
	return XDEFAULT_MAX_EXP;
}

/**
 @brief 현재 레벨까지의 exp총합을 돌려준다.
*/
XINT64 XFLevel::GetExpSum( void )
{
	XINT64 sum = 0;
	for( int i = 1; i < m_Level; ++i )
		sum += GetMaxExp( i );
	sum += m_Exp;
	return sum;
}

BOOL XFLevel::SubExp( DWORD exp )
{
	if( exp <= m_Exp )
		m_Exp -= exp;
	else
		m_Exp = 0;
	return TRUE;
}


BOOL XFLevel::AddExp( DWORD addExp ) 
{
	XBREAK( m_pDelegate == NULL );
	BOOL bLevelUp = FALSE;
	DWORD expCurr = m_Exp;
	XBREAK( m_bAutoLevelup == false );	// 일단 사용안해서 막아둠.
	// 최대레벨에서는 더이상 안오름.
	int maxLevel = m_pDelegate->OnDelegateGetMaxLevel( this, 0, 0 );
	if( m_Level >= maxLevel )
		return FALSE;
	// 한계레벨에 도달하면 더이상 경험치를 얻을 수 없음.
	if( m_lvLimit && m_Level >= m_lvLimit )
		return FALSE;
	while(addExp) {
		expCurr += addExp;
		// 현재 레벨의 최대 exp를 얻는다.
		DWORD expMax = m_pDelegate->OnDelegateGetMaxExp( this, m_Level, 0, 0 );
		if( expMax == 0 )
			return FALSE;
		if( expCurr >= expMax && expMax != XDEFAULT_MAX_EXP ) {
			if( m_bAutoLevelup ) {
				++m_Level;
				m_bLevelUp = bLevelUp = TRUE;
				m_Exp = 0;
				m_pDelegate->OnDelegateLevelUp( this );
				// 레벨업 하고 남은 exp를 계산한다.
				addExp = expCurr - expMax;		
				expCurr = 0;
				int maxLevel = m_pDelegate->OnDelegateGetMaxLevel( this, 0, 0 );
				if( m_Level >= maxLevel ) {
					addExp = 0;
					break;
				}
				if( m_lvLimit && m_Level >= m_lvLimit ) {
					// 현재 exp를 0으로맞추고 남은 exp도 0으로 해서 더이상 경험치를 얻지 못하게 한다.
					addExp = 0;	
					break;
				}
			} else {
				// 수동레벨업 모드.
				// 오버되는 경험치를 따로 받아둠.
				XBREAK( expCurr - expMax > 0xffffffff );
				m_expRemainByAfterLevelUp += (DWORD)(expCurr - expMax);
				// exp는 만땅상태로
				m_Exp = expMax;
				break;
			}
		} else {
			// 아직 만땅이 안됨.
			m_Exp = expCurr;
			// 최대레벨에 도달한상태면 경험치가 더이상 오르지 말아야 한다.
			int maxlevel = m_pDelegate->OnDelegateGetMaxLevel( this, 0, 0 );
			if( m_Level >= maxlevel )
				expCurr = m_Exp = 0;
			break;
		}
	}
	return bLevelUp;
}

BOOL XFLevel::AddExp( XINT64 exp )
{
	if( XBREAK( exp > 0xffffffff ) )
	{

	} else
	{
		return AddExp( (DWORD)exp );
	}
	return FALSE;
}

BOOL XFLevel::Serialize( XArchive& ar ) const
{
	XBREAK( m_Level < 0 );
	ar << (BYTE)VER_LEVEL_SERIALIZE;
	ar << (BYTE)m_Level;
	ar << (BYTE)m_bLevelUp;
// 	ar << xboolToByte(m_bAutoLevelup);
	XBREAK( m_lvLimit > 255 );
	ar << (BYTE)m_lvLimit;
	ar << m_Exp;
//	ar << m_expRemainByAfterLevelUp;
	return TRUE;
}

BOOL XFLevel::DeSerialize( XArchive& ar )
{
	int ver;
	BYTE b0;
//	WORD w0;
	ar >> b0;	ver = b0;
	ar >> b0;	m_Level = (int)b0;
	ar >> b0;	m_bLevelUp = (BOOL)b0;
	ar >> b0; m_lvLimit = b0;
// 		m_bAutoLevelup = xbyteToBool( b0 );
	if( XBREAK( m_Level < 0 || m_Level > 255 ) )	// 뭔가 이상한값이 읽힌것임.
		return FALSE;
	ar >> m_Exp;
// 		ar >> m_expRemainByAfterLevelUp;
	return TRUE;
}

/**
 @brief exp가 만땅상태냐. 수동레벨업 모드에서만 사용한다.

*/
bool XFLevel::IsFullExp() 
{
// 	XASSERT( m_bAutoLevelup == false );
	DWORD expMax = m_pDelegate->OnDelegateGetMaxExp( this, m_Level, 0, 0 );
	return m_Exp == expMax;
}

/**
 @brief 수동레벨업 모드에서 사용하는 레벨업 함수.
*/
// void XFLevel::AddLevelManual( int add )
// {
// 	// 자동레벨업 상태면 안됨
// 	XBREAK( m_bAutoLevelup == true );
// 	++m_Level;
// 	m_Exp = 0;
// 	XBREAK( m_Level > m_pDelegate->OnDelegateGetMaxLevel( this, 0, 0 ) );
// 	// 잔여 exp를 더해준다.
// 	if( m_expRemainByAfterLevelUp > 0 )
// 	{
// 		DWORD expAdd = m_expRemainByAfterLevelUp;
// 		m_expRemainByAfterLevelUp = 0;
// 		AddExp( expAdd );
// 	}
// }

int XFLevel::GetMaxLevel() 
{
	XBREAK( m_pDelegate == nullptr );
	return m_pDelegate->OnDelegateGetMaxLevel( this, 0, 0 );
}
