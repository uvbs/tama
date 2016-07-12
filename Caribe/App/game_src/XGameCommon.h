#pragma once

////////////////////////////////////////////////////////////////
class XGameLua;
/**
 클라/서버 공통 컨텐츠 객체
*/
class XGameCommon
{
	XGameLua *m_pLua;
	void Init() {
		m_pLua = nullptr;
	}
	void Destroy();
public:
	XGameCommon();
	virtual ~XGameCommon() { Destroy(); }
	//
	GET_ACCESSOR( XGameLua*, pLua );
	//
	void Create();
	void CreateCommon();
	void DestroyCommon();
	BOOL LoadTextTable();
	void LoadConstant();
	void AddConstant();
	virtual void OnAfterPropSerialize();
#ifdef _XPROP_SERIALIZE
	void ArchivingProp( XArchive& ar );
#endif // _XPROP_SERIALIZE
	void LoadPropLegion();
};