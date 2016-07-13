#pragma once
/*
 *  defineAct.h
 *  Game
 *
 *  Created by xuzhu on 10. 12. 22..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "sprite/sprDef.h"
//#define MAX_ID			100			// �ִ�� �ο��Ҽ� �ִ� �׼Ǿ��̵� ����0~99��
// ���� �׼� ���̵� // MAX_ID - 1 ���� ���ʷ� �������� �ο��Ѵ�	
#define ACT_IDLE1		99			// ���1
#define ACT_RUN			98			// �޸���
#define ACT_ATTACK1		97			// ����1
#define ACT_ATTACK2		96			// ����2
#define ACT_ATTACK3		95			// ����3
#define ACT_SKILL1		94
//#define ACT_SKILL2		93
//#define ACT_SKILL3		92
#define ACT_DIE			91				// ����
#define ACT_STUN		90			// ����
// ���� �׼Ǿ��̵�(1~)					// ������ �ڵ����� �Ű����� ���̵��ȣ�� 1���� �����Ѵ�

// �̺�Ʈ ���̵�
#define xSPREVT_HIT		1	///< ����Ÿ��
#define xSPREVT_SHOOT	2	///< ���Ÿ� �߻�
#define xSPREVT_SKILL	3	///< ��ų
