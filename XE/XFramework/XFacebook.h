#pragma once

////////////////////////////////////////////////////////////////
class XDelegateFacebook
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateFacebook() { Init(); }
	virtual ~XDelegateFacebook() { Destroy(); }
	//
	// 페이스북 인증 결과를 돌려받는다. cUserId나 Username이 empty면 인증이 실패
	virtual void DelegateFacebookCertResult( const char *cUserId, const char *cUsername, DWORD param ) {}
//	virtual void DelegateFacebookCall( void );
};

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/08 12:19
*****************************************************************/
class XFacebook
{
public:
	static std::shared_ptr<XFacebook>& sGet();
	static XDelegateFacebook* sGetpDelegate() { return s_pDelegate; };
	static void sSetpDelegate( XDelegateFacebook* pDelegate ) {
		s_pDelegate = pDelegate;
	}
	static bool sIsActivated() {
		return s_spInstance != nullptr;
	}
public:
	XFacebook() { Init(); }
	virtual ~XFacebook() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( const std::string&, strcFbUserId );
	BOOL DoRequestCertification( XDelegateFacebook *pDelegate );
	void GetRequestProfileImage( const char* cUserid );
#ifdef WIN32
	inline void GetRequestProfileImage( const _tstring& strUserId ) {
		const std::string strcUserId = SZ2C(strUserId);
		GetRequestProfileImage( strcUserId.c_str() );
	}
#endif // WIN32
	inline void GetRequestProfileImage( const std::string& strUserId ) {
		GetRequestProfileImage( strUserId.c_str() );
	}
	void OnRecvProfileImage( const std::string& strcFbUserId );
	bool IsRequestingProfileImage( const std::string& strcFbUserId ) const;
	void Process();
	void cbOnRecvProfileImageByFacebook( const std::string& strFbUserId, DWORD* pImage, int w, int h, int bpp );
private:
	struct xResultProfileImage {
		std::string m_strFbUserId;
		DWORD* m_pImg = nullptr;
		XE::POINT m_sizeImg;
		int m_bpp = 0;
		//
		xResultProfileImage() {}
		xResultProfileImage( const std::string& strFbUserId, DWORD *pImg, int w, int h, int bpp ) {
			m_strFbUserId = strFbUserId;
			m_pImg = pImg;
			m_sizeImg.Set( w, h );
			m_bpp = bpp;
		}
		void clear() {
			m_strFbUserId.clear();
			m_pImg = nullptr;
			m_sizeImg.Set( 0, 0 );
			m_bpp = 0;
		}
	};
	static std::shared_ptr<XFacebook> s_spInstance;
	static XDelegateFacebook *s_pDelegate;
	std::string m_strcFbUserId;
	std::map<std::string, bool> m_mapRequestingProfileImage;		// 프로필 사진 요청중인 맵.
	XList4<std::string> m_listRequestProfileImage;			// 프로필사진 요청할 큐
	XLock m_lockProfileImage;
	XList4<xResultProfileImage> m_listResultProfileImage;
private:
	void Init() {}
	void Destroy() {}
	virtual void CallFacebook();
	void ProcessRequestProfileImage();
	bool IsExistRequest( const std::string& strcFbUserId );
private:
}; // class XFacebook

