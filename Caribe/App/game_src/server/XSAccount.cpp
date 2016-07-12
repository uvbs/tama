#include "stdafx.h"
#include "server/XSAccount.h"
#include "server/XPushNotifyObj.h"
#include "XPacketCG.h"
#include "XGame.h"
#include "XWorld.h"
#include "XPostInfo.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

template<> XPool<XSAccount>* XMemPool<XSAccount>::s_pPool = NULL;

XSAccount::XSAccount() 
{ 
	Init(); 
	// 여기다 멤버변수 메모리 할당시키지 말것. RestorePacket()할때 Destroy()한다. 
}

XSAccount::XSAccount( ID idAccount ) 
	: XAccount( idAccount )
{ 
	Init(); 
	// 여기다 멤버변수 메모리 할당시키지 말것. RestorePacket()할때 Destroy()한다.
}

XSAccount::XSAccount( ID idAccount, LPCTSTR szID )
	: XAccount( idAccount, szID )
{
	Init();
	// 여기다 멤버변수 메모리 할당시키지 말것. RestorePacket()할때 Destroy()한다.
}

void XSAccount::Destroy( void )
{
}


/**
 새계정이 생길때 기본으로 들어있는 계정데이터를 생성한다.
*/
#ifdef _GAME_SERVER
void XSAccount::CreateDefaultAccount( void )
{
	// 임시로 가짜 데이타를 넣음.
	CreateFakeAccount();	
	UpdatePlayTimer( 0 );	// 플레이타이머 초기화
//	UpdateTradeMerchantTimer(0);
//	UpdateShopMerchantTimer(0);
}
// void XSAccount::InitializeTimer()
// {	
// 	GetShopTimer()->Set((float)(XGC->m_shopCallInerval));
// 	SetsecShop((DWORD)GetShopTimer()->GetRemainSec());
// 	//ChangeShopItemList();
// }
BOOL XSAccount::NewAccount( )
{
	BOOL bRet = TRUE;
//	CreateFakeAccount();
	CreateDefaultAccount();

	return bRet;
}
#endif // _GAME_SERVER

//// DB 저장용도
int XSAccount::Serialize( XArchive& p )
{
	XAccount::Serialize( p );
	return 1;
}

#define xVERIFY_ACC( COND ) \
	if( XBREAK( COND ) ) \
	return FALSE;

int XSAccount::DeSerialize( XArchive& p )
{
//	DWORD dw1;
	if( XAccount::DeSerialize( p ) == 0 )
		return 0;

	return 1;
}

void XSAccount::Save( )
{
}

void XSAccount::Load( )
{	
}
// void XSAccount::DeletePostInfo(ID postidx)
// {
// 	XLIST_LOOP(m_listPost, XPostInfo*, pPostinfo)
// 	{
// 		if (pPostinfo->GetIndex() == postidx)
// 		{
// 			SAFE_DELETE(pPostinfo);
// 			break;
// 		}
// 	}END_LOOP;
// }

// BOOL XSAccount::PostItemReceive(ID  postidx, int itempos, int itempropid)
// {
// 	XLIST_LOOP(m_listPost, XPostInfo*, pPostinfo)
// 	{
// 		if (pPostinfo->GetIndex() == postidx)
// 		{
// 			if (pPostinfo->ReceiveItem(itempos, itempropid) > 0)
// 			{				
// 				XPropItem::xPROP *pProp = PROP_ITEM->GetpProp(itempropid);
// 				XBREAK(pProp == nullptr);
// 				int num = 1;				
// 				CreateItemToInven(pProp, num);
// 				//XBaseItem *pBaseItem = sCreateItem(pProp, num);
// 				//AddItem(pBaseItem);
// 				//아이템 받을수 있는 것0
// 				return TRUE;
// 			}
// 			return FALSE;
// 		}
// 	}END_LOOP;
// 	return FALSE;
// }

/**
 @brief snPost메일의 첨부템들을 모두 수령함.
*/
// BOOL XSAccount::PostItemReceiveAll(ID snPost) //수령하지 않은 모든 아이템 수령
// {
// 	XLIST_LOOP(m_listPost, XPostInfo*, pPostinfo)
// 	{
// 		if (pPostinfo->GetIndex() == snPost)
// 		{
// 			XList<XPostItem*>* pItemlist = pPostinfo->GetlistPostItems();
// 			int _max = pItemlist->size();
// 			XList<XPostItem*>::Itor _itor = pItemlist->GetNextClear2();
// 			for (int _i = 0; _i < _max; ++_i)
// 			{
// 				XPostItem* pItems = pItemlist->GetNext2(_itor);
// 				
// 				if (pItems)
// 				{
// 					if (pItems->GetItemType() == XGAME::xtPOSTResource::xPOSTRES_ITEMS )
// 					{
// 						XPropItem::xPROP *pProp = PROP_ITEM->GetpProp(pItems->GetItemID());
// 						XBREAK(pProp == nullptr);
// 
// 						XBaseItem *pBaseItem = sCreateItem(pProp, pItems->GetnCount());
// 
// 						CreateItemToInven(pProp, pItems->GetnCount());
// 					}
// 					else if (pItems->GetItemType() == XGAME::xtPOSTResource::xPOSTRES_GEM)
// 					{
// 						AddCashtem(pItems->GetnCount());
// 					}
// 					else if (pItems->GetItemType() == XGAME::xtPOSTResource::xPOSTRES_GOLD)
// 					{
// 						AddGold(pItems->GetnCount());
// 					}
// 					else if (pItems->GetItemType() == XGAME::xtPOSTResource::xPOSTRES_RESOURCE)
// 					{
// 						AddResource((XGAME::xtResource)pItems->GetItemID(), pItems->GetnCount());
// 					} 
// 					else if (pItems->GetItemType() == XGAME::xtPOSTResource::xPOSTRES_GUILD_COIN )
// 					{
// 						AddGuildPoint( pItems->GetnCount() );
// 					}
// 				}			
// 			}
// 			return TRUE;
// 		}
// 	}END_LOOP;
// 	return FALSE;
// }
//void XSAccount::ChangePostStatus(int idx, XPostInfo::POST_STATUS e)
//{
//	XLIST_LOOP(m_listPost, XPostInfo*, pPostinfo)
//	{
//		if (pPostinfo->GetIndex() == idx)
//		{
//			if (pPostinfo->GetStatus() >= XPostInfo::_POST_STATUS_RECEIVE_ITEM)
//			{
//				pPostinfo->SetStatus(XPostInfo::_POST_STATUS_DELETE);
//				break;
//			}
//		}
//	}END_LOOP
//}

// void XSAccount::SerializePostUpdate(XArchive& ar)
// {
// 	int nCount = 0;
// 
// 	ar << (int)m_listPost.size();
// 	XLIST_LOOP(m_listPost, XPostInfo*, pPostinfo)
// 	{
// 		nCount++;
// 		pPostinfo->Serialize(ar);
// 	}END_LOOP
// }
// 
// void XSAccount::DeSerializePostUpdate(XArchive& ar)
// {
// 	int nCount = 0;
// 
// 	ar >> nCount;
// 
// 	if (nCount > 0)
// 	{
// 		for (int n = 0; n < nCount; n++ )
// 		{
// 			XPostInfo* pPost = new XPostInfo;
// 			pPost->DeSerialize(ar);
// 			if (AddPostInfo(pPost) == nullptr)
// 			{
// 				delete pPost;
// 				pPost = NULL;
// 			}
// 		}
// 	}	
// }

void XSAccount::AsyncPushNotify()
{
}

void XSAccount::ClearPushNotify()
{	
	std::list<XPushNotifyObj*>::iterator begin = m_PushNotifyList.begin();
	while (begin != m_PushNotifyList.end())
	{
		begin++;
	}
}

bool XSAccount::AddPushNotify(int type, DWORD sendtime, DWORD sendendtime, _tstring strMessage, XArchive& ar)
{
	ID existid = FindPushNotify(type);

	return true;
}
bool XSAccount::DelPushNotify(ID snNum, int type)
{
	std::list<XPushNotifyObj*>::iterator begin = m_PushNotifyList.begin();
	while (begin != m_PushNotifyList.end())
	{
		XPushNotifyObj* pObj = (*begin);
		if (pObj)
		{
			if (snNum == 0)
			{
				if (pObj->GetnNotifyType() == type)
				{
					delete pObj;
					pObj = nullptr;
					return true;
				}
			}
			else if (pObj->GetnNotifyType() == type && pObj->GetsnNum() == snNum)
			{
				delete pObj;
				pObj = nullptr;
				return true;
			}				
		}
		begin++;
	}	
	return false;
}
ID	XSAccount::FindPushNotify(int type)
{
	std::list<XPushNotifyObj*>::iterator begin = m_PushNotifyList.begin();
	while (begin != m_PushNotifyList.end())
	{
		XPushNotifyObj* pObj = (*begin);
		if (pObj)
		{
			if (pObj->GetnNotifyType() == type) 
				return pObj->GetsnNum();
		}
		begin++;
	}
	return 0;
}

// /**
//  @brief DB에서 데이타를 받은 후 업데이트를 한번 한다.
// */
// void XSAccount::UpdateAfterDeserialize()
// {
// 	// 무기점 아이템 갱신시간이 지났는가.
// 	const auto secReset = XGC->m_shopCallInerval;
// 	if( GettimerShop().IsOver( secReset ) ) {
// 
// 	}
// }

bool XSAccount::IsExistUserSpot( ID idacc )
{
	return GetpWorld()->IsExistUserSpot( idacc );
}

