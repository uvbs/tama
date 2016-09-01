#include "stdafx.h"
#include "XGoogle.h"
#include "XStruct.h"
#include "rapidjson/document.h"
#include "XGlobalConst.h"
#include "XFramework/XConsoleMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace xGoogle;

// this유저가 로깅리스트에 있을때만 출력. & TAG가 로깅태그에 있을때만 출력.
#define CONSOLE_ACC( TAG, F, ... ) \
	if( CONSOLE_MAIN->IsLogidAcc( GetidAcc() ) ) { \
	__xLogfTag( TAG, XLOGTYPE_LOG, XTSTR("%s:%s(%d)-", F), __TFUNC__, GetspAcc()->GetstrName(), GetidAcc(), ##__VA_ARGS__); \
		}

xtErrorIAP 
XGoogle::sDoVerifyAfterPurchase( const std::string& strcJson
																, const std::string& strcSignature
																, xInApp* pOut )
{
	if( XBREAK(pOut == nullptr) )
		return xIAPP_NONE;
	xtErrorIAP errCode = xIAPP_NONE;
	do {
		const auto& json = strcJson;
		// 상품명을 따로 패킷으로 받지말고 원본 json에서 빼서 써야 한다.
		if( XBREAK(json.empty()) ) {
			errCode = xIAPP_ERROR_INVALID_RECEIPT;
			break;
		}
		xReceipt receipt;
		if( sParseJsonRecipt( strcJson, &receipt ) ) {
			errCode = sCheckError( receipt );
			if( XBREAK(errCode != xIAPP_NONE) )
				break;
			XBREAK( receipt.AssertValid() == false );
			auto pGoodsInfo = XGC->GetCashItem( receipt.m_idsProduct, XGAME::xPL_GOOGLE_STORE );
			if( XBREAK(pGoodsInfo == nullptr) ) {
				errCode = xIAPP_ERROR_CRITICAL;
				break;
			}
			const XGAME::xInApp inapp( xPL_GOOGLE_STORE
															, pGoodsInfo->m_type
															, pGoodsInfo->m_productID
															, strcSignature
															, strcJson
															, receipt.m_strTime
															, receipt.m_strOrderId
															, receipt.m_strPayload );
			XBREAK( inapp.AssertValid() == false );
			*pOut = inapp;
			XBREAK( pOut->AssertValid() == false );
		} else {
			XBREAKF( 1, "iap", "receipt parse error: %s:%s\n", __TFUNC__, receipt.GetstrLog().c_str() );
			errCode = xIAPP_ERROR_INVALID_RECEIPT;
			break;
		}
		if( receipt.statePurchase != 0 ) {	// purchased
			// 이건 에러아님
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
			XBREAK(1);	// 어떤경우에 이런게 나오는지 확인
			CONSOLE( "invalid IAP state: state=%d", receipt.statePurchase );
#endif
			break;
		}
	} while (0);
	return errCode;
}

/**
 @brief 
*/
bool XGoogle::sParseJsonRecipt( const std::string& json
															, xGoogle::xReceipt* pOut )
{
	try {
		do {
			rapidjson::Document d;
			XBREAK( json.empty() );
			d.Parse<0>(json.c_str());
			XBREAK( d.HasMember("productId") == false );
			const char* 
			cTemp = d["productId"].GetString();		// json에서 상품아이디 빼옴
			pOut->m_idsProduct = C2SZ(cTemp);
			cTemp = nullptr;
			if( d.HasMember("orderId") )				// 결제실패로 재소진을 시도할땐 이 값이 없다.
				cTemp = d["orderId"].GetString();			// 거래 아이디
			pOut->m_strOrderId = (cTemp)? cTemp : "";
			XBREAK( d.HasMember( "purchaseTime" ) == false );
			const auto llTime = d["purchaseTime"].GetUint64();		// 거래 시간
			pOut->m_strTime = XE::Format("%llu", llTime );
			//////////////////////////////////////////////////////////////////////////
			pOut->statePurchase = d["purchaseState"].GetInt();
			XBREAK( d.HasMember( "purchaseToken" ) == false );
			cTemp = d[ "purchaseToken" ].GetString();
			pOut->m_strToken = ( cTemp ) ? cTemp : "";
			XBREAK( d.HasMember( "developerPayload" ) == false );
			cTemp = d[ "developerPayload" ].GetString();			// payload
			pOut->m_strPayload = ( cTemp ) ? cTemp : "";
			if( XBREAK( pOut->AssertValid() == false ) )
				return false;
		} while (0);
	}
	catch( CMemoryException* /*e*/ ) {
		XBREAKF( 1, "IAP: exception: memory" );
		return false;
	}
	catch( CFileException* /*e*/ ) {
		XBREAKF( 1, "IAP: exception: file" );
		return false;
	}
	catch( CException* /*e*/ ) {
		XBREAKF( 1, "IAP: exception: etc" );
		return false;
	} // try/catch
	//
	return true;
}

xtErrorIAP XGoogle::sCheckError( const xReceipt& receipt )
{
	xtErrorIAP errCode = xIAPP_NONE;
	do {
		if( receipt.m_idsProduct.empty() ) {
			errCode = xIAPP_ERROR_NOT_FOUND_PRODUCT;
			break;
		}
		auto pGoodsInfo = XGC->GetCashItem( receipt.m_idsProduct, XGAME::xPL_GOOGLE_STORE );
		if( pGoodsInfo == nullptr ) {
			errCode = xIAPP_ERROR_NOT_FOUND_PRODUCT;
			break;
		}
// 		if( receipt.m_strOrderId.empty() ) {	로그인할때 소진안된거 컨슘할땐 이값이 없더라.
// 			errCode = XGAME::xIAPP_ERROR_INVALID_RECEIPT;
// 			break;
// 		}
		if( receipt.m_strTime.empty() ) {
			errCode = XGAME::xIAPP_ERROR_INVALID_RECEIPT;
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		switch( receipt.statePurchase ) {
		case 0:	break;
		case 1: errCode = XGAME::xIAPP_ERROR_CANCELED_PURCHASE;	break;
		case 2: errCode = XGAME::xIAPP_ERROR_REFUNDED_PURCHASE;	break;
		default:
			XBREAKF( 1, "invalid statePurchase:%d", receipt.statePurchase );
			break;
		}
		if( receipt.m_strToken.empty() ) {
			errCode = XGAME::xIAPP_ERROR_INVALID_RECEIPT;
			break;
		}
		if( receipt.m_strPayload.empty() ) {
			errCode = xIAPP_ERROR_INVALID_PAYLOAD;
			break;
		}
	} while( 0 );
	return errCode;
}
