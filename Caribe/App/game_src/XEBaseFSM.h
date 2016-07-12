#pragma once

//////////////////////////////////////////////////////////////////////////
class XEControllerFSM;
class XEBaseFSM
{
	XEControllerFSM *m_pController;
	ID m_idFSM;			///< 상태 고유 아이디
	ID m_idNextFSM;		///< 현재상태를 끝내고 난 후 전환해야할 상태가 있으면 미리 등록해둔다.
	ID m_idPrevFSM;		///< 이전 상태

private:
	void _Init() {
		m_idFSM = 0;
		m_pController = nullptr;
		m_idNextFSM = 0;
		m_idPrevFSM = 0;
	}
	void Destroy();
	SET_ACCESSOR( ID, idPrevFSM );

public:
	XEBaseFSM( ID idFSM, XEControllerFSM *pController );
	virtual ~XEBaseFSM() { Destroy(); }
	//
	GET_ACCESSOR( ID, idFSM );
	GET_ACCESSOR( XEControllerFSM*, pController );
	GET_SET_ACCESSOR( ID, idNextFSM );
	GET_ACCESSOR( ID, idPrevFSM );

	virtual int FrameMove( float dt ) = 0;
	virtual void Init( void ) {}
	virtual void Release() = 0;
	/**
	 @brief 디버깅 메시지 출력할게 있으면 str에 더해준다.
	*/
	virtual void AddDebugStr( _tstring& str ) {}
	/**
	 @brief 상태가 전환될때 마지막으로 호출된다.
	*/
	virtual void Uninit( void ) {
		SetidNextFSM( 0 );	// 상태벗어날때는 초기화값으로 돌림
	}
	//
	XEBaseFSM* ChangeFSM( ID idFSM );
friend class XEControllerFSM;
};

/****************************************************************
* @brief 
* @author xuzhu
* @date	2014/09/15 18:37
*****************************************************************/
class XEControllerFSM
{
	XEBaseFSM *m_pFSM;								///< 현재 FSM
	XArrayLinearN<XEBaseFSM*, 16> m_aryStates;		///< 모든 FSM객체의 어레이
	void Init() {
		m_pFSM = nullptr;
	}
	void Destroy();

protected:
	SET_ACCESSOR( XEBaseFSM*, pFSM );

public:
	XEControllerFSM();
	virtual ~XEControllerFSM() { Destroy(); }
	//
	//GET_ACCESSOR( XEBaseFSM*, pFSM );
	XEBaseFSM* GetpBaseFSM() const {
		return m_pFSM;
	}
	//
	XEBaseFSM* RegisterFSM( XEBaseFSM *pFSM ) {
		m_aryStates.Add( pFSM );
		return pFSM;
	}
	XEBaseFSM* FindFSM( ID idFSM );
	XEBaseFSM* ChangeFSM( ID idFSM ) {
		// old fsm
		m_pFSM->Uninit();
		m_pFSM->SetidPrevFSM( m_pFSM->GetidFSM() );		
		// new fsm
		m_pFSM = FindFSM( idFSM );
		XBREAK( m_pFSM == nullptr );
		m_pFSM->Init();
		return m_pFSM;
	}
	XEBaseFSM* GetFSM( ID idFSM ) {
		return FindFSM( idFSM );
	}
	void Release( void );
};