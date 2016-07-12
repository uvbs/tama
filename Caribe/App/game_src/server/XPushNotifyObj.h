#pragma once

class XPushNotifyObj
{
private:
	ID		m_snNum;
	int		m_nNotifyType;

	DWORD	m_dwNotifySendTime;
	DWORD	m_dwNotifyLimitTime;
	XArchive m_arDataParam; // Count, Data //json 으로 변환할 데이터를 집어 넣는다.
	_tstring m_strMessage;
public:
	XPushNotifyObj(){}
	XPushNotifyObj(ID snNum){ m_snNum = snNum; }
	XPushNotifyObj(ID snNum, int nNotifyType, DWORD dwNotifySendTime, DWORD dwNotifyLimitTime, _tstring strMessage, XArchive& ar)
	{
		m_snNum = snNum; 
		m_nNotifyType = nNotifyType;
		m_dwNotifySendTime = dwNotifySendTime;
		m_dwNotifyLimitTime = dwNotifyLimitTime; 
		m_strMessage = strMessage;
		m_arDataParam = ar;
	}
	virtual ~XPushNotifyObj();

	GET_SET_ACCESSOR(ID, snNum);
	GET_SET_ACCESSOR(int, nNotifyType);
	GET_SET_ACCESSOR(DWORD, dwNotifySendTime);
	GET_SET_ACCESSOR(DWORD, dwNotifyLimitTime);
	GET_TSTRING_ACCESSOR(strMessage);
};