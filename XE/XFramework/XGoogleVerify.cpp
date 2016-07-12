// googleVerify.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <memory>
#include "XGoogleVerify.h"
//#include <boost/shared_ptr.hpp>

/* 
   base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

//#include "base64.h"
#include <iostream>

static const std::string base64_chars = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

int XE::InappBillingVerify(const char* data, const char* signature, const char* pub_key_id)
{
	OpenSSL_add_all_digests();	// 스레드 세이프 하지 않으므로 멀티스레드에서 쓴다면 밖으로 빼낼것.
										// OpenSSl 초기화 할때 한번만 Call
	//
	std::shared_ptr<EVP_MD_CTX> mdctx =
		std::shared_ptr<EVP_MD_CTX>(EVP_MD_CTX_create(), EVP_MD_CTX_destroy);
	const EVP_MD* md = EVP_get_digestbyname("SHA1");

	EVP_VerifyInit_ex(mdctx.get(), md, NULL);

	EVP_VerifyUpdate(mdctx.get(), (void*)data, strlen(data));

	std::shared_ptr<BIO> b64 = std::shared_ptr<BIO>(BIO_new(BIO_f_base64()), BIO_free);
	BIO_set_flags(b64.get(),BIO_FLAGS_BASE64_NO_NL);

	std::shared_ptr<BIO> bPubKey = std::shared_ptr<BIO>(BIO_new(BIO_s_mem()), BIO_free);
	BIO_puts(bPubKey.get(),pub_key_id);
	BIO_push(b64.get(), bPubKey.get());

	std::shared_ptr<EVP_PKEY> pubkey = 
		std::shared_ptr<EVP_PKEY>(d2i_PUBKEY_bio(b64.get(), NULL), EVP_PKEY_free);
	std::string decoded_signature = base64_decode(std::string(signature));

	int result = EVP_VerifyFinal(mdctx.get(), (unsigned char*)decoded_signature.c_str(), 
		decoded_signature.length(), pubkey.get());

	EVP_cleanup();	// 스레드 세이프 하지 않으므로 멀티스레드에서 쓴다면 밖으로 빼낼것.
	return result;
}
/*
int _tmain(int argc, _TCHAR* argv[])
{
	const char* purchase_data =
		"{"
		"\"orderId\":\"12999763169054705758.1371079406387615\","
		"\"packageName\":\"com.example.app\","
		"\"productId\":\"exampleSku\","
		"\"purchaseTime\":1358227642000,"
		"\"purchaseState\":0,"
		"\"developerPayload\":\"bGoa+V7g/yqDXvKRqq+JTFn4uQZbPiQJo4pf9RzJ\","
		"\"purchaseToken\":\"rojeslcdyyiapnqcynkjyyjh\""
		"}";

	const char* public_key = "MIIBIjANBgkqhkiG9w0BAQEFAAO...";
	const char* signature = "DW0nEjsBhqHpECrS5Tcq6Y51vM...";

	OpenSSL_add_all_digests();
	std::cout << InappBillingVerify(purchase_data, signature, public_key) << std::endl;
	EVP_cleanup();
	return 0;
}

*/
