#pragma once
// cpp�ҽ������� ���ǰ� ���ҽ��ε� ���Ǵ� ������Դϴ�.
// ���⼭ ������ ������� ������Ƽ�� ��ũ��Ʈ ��� ����� ���ֽ��ϴ�.
#ifdef __cplusplus
// �Ʒ��� ���ǵ� ������� ��� ��ƿ��� ���� �ִ�.
#define DEFINE_SAMPLE		11


namespace XGAME
{
	// ��ȭ
	enum xCurrency {
		xCUR_NONE,
		xCUR_WON_KOREA,
		xCUR_US_DOLLAR,
	};
	// 8����
	enum xtDir { DIR_E, 
				DIR_ES, 
				DIR_S, 
				DIR_WS, 
				DIR_W, 
				DIR_WN, 
				DIR_N, 
				DIR_EN };	
	
	enum xtEnum { xSIZE_NONE,
					xSIZE_SMALL,
					xSIZE_MIDDLE,
					xSIZE_BIG,
					};
};

#endif
