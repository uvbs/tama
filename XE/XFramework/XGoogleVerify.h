#pragma once

namespace XE {
	int InappBillingVerify(const char* data, const char* signature, const char* pub_key_id);
	inline int InappBillingVerify( const std::string& strData
																, const std::string& strSignature
																, const std::string& strPublicKey ) {
		return InappBillingVerify( strData.c_str(), strSignature.c_str(), strPublicKey.c_str() );
	}
};