#include "stdafx.h"
#include "XGoogle.h"
#include "XStruct.h"
#include "rapidjson/document.h"
#include "XGlobalConst.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace xGoogle;

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
		if( json.empty() ) {
			errCode = xIAPP_ERROR_INVALID_RECEIPT;
			break;
		}
		xReceipt receipt;
		if( sParseJsonRecipt( strcJson, &receipt ) ) {
			errCode = sCheckError( receipt );
			if( errCode != xIAPP_SUCCESS )
				break;
			auto pGoodsInfo = XGC->GetCashItem( receipt.m_idsProduct, XGAME::xPL_GOOGLE_STORE );
			if( pGoodsInfo == nullptr ) {
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
			*pOut = inapp;
		} else {
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
			d.Parse<0>(json.c_str());
			const char* 
			cTemp = d["productId"].GetString();		// json에서 상품아이디 빼옴
			pOut->m_idsProduct = C2SZ(cTemp);
			cTemp = d["orderId"].GetString();			// 거래 아이디
			pOut->m_strOrderId = (cTemp)? cTemp : "";
			const auto llTime = d["purchaseTime"].GetUint64();		// 거래 시간
			pOut->m_strTime = XE::Format("%llu", llTime );
			//////////////////////////////////////////////////////////////////////////
			pOut->statePurchase = d["purchaseState"].GetInt();
			cTemp = d[ "purchaseToken" ].GetString();
			pOut->m_strToken = ( cTemp ) ? cTemp : "";
			cTemp = d[ "developerPayload" ].GetString();			// payload
			pOut->m_strPayload = ( cTemp ) ? cTemp : "";
		} while (0);
	}
	catch( CMemoryException* /*e*/ ) {
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
		XBREAKF( 1, "IAP: exception: memory" );
#endif
		return false;
	}
	catch( CFileException* /*e*/ ) {
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
		XBREAKF( 1, "IAP: exception: file" );
#endif
		return false;
	}
	catch( CException* /*e*/ ) {
#if _DEV_LEVEL <= DLV_DEV_EXTERNAL
		XBREAKF( 1, "IAP: exception: etc" );
#endif
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
		if( receipt.m_strOrderId.empty() ) {
			errCode = XGAME::xIAPP_ERROR_INVALID_RECEIPT;
			break;
		}
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
