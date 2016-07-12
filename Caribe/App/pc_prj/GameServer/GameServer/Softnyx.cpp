/********************************************************************
	@date:	2016/06/08 19:51
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\pc_prj\GameServer\GameServer\Softnyx.cpp
	@author:	xuzhu
	
	@brief:	서버는 원빌드로 하기위해 디파인을 쓰지 않음.
*********************************************************************/
#include "stdafx.h"
#include "Softnyx.h"
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

XGAME::xtErrorIAP 
XSoftnyx::sDoVerifyAfterPurchase( const std::string& strcJson
																, const std::string& strPayload
																, const _tstring& idsProduct
																, xInApp* pOut )
{
	if( XBREAK(pOut == nullptr) )
		return xIAPP_NONE;
	xtErrorIAP errCode = xIAPP_NONE;
	do {
		if( XBREAK( idsProduct.empty() ) ) {
			errCode = xIAPP_ERROR_NOT_FOUND_PRODUCT;
			break;
		}
		auto pGoodsInfo = XGlobalConst::sGet()->GetCashItem( idsProduct, xPL_SOFTNYX );
		if( XBREAK(pGoodsInfo == nullptr) ) {
			errCode = xIAPP_ERROR_NOT_FOUND_PRODUCT;
			break;
		}
		// json parsing
		const auto& json = strcJson;
		if( json.empty() ) {
			errCode = xIAPP_ERROR_INVALID_RECEIPT;
			break;
		}
		rapidjson::Document d;
		d.Parse<0>( json.c_str() );
		const char*
			cReturnCode = d[ "returnCode" ].GetString();
		int returnCode = atoi( cReturnCode );
		XBREAK( returnCode != 0 );
		const char*
			cPoint = d[ "point" ].GetString();
		int point = atoi( cPoint );
		const char*
			cToken = d[ "paymentToken" ].GetString();
		const char*
			cOrderId = d[ "orderID" ].GetString();
		XBREAK( cOrderId == nullptr );
		const XGAME::xInApp inapp( xPL_SOFTNYX
														, pGoodsInfo->m_type
														, pGoodsInfo->m_productID
														, std::string()
														, strcJson
														, std::string()
														, std::string( cOrderId )
														, strPayload );
		*pOut = inapp;
	} while (0);

	return errCode;
}

