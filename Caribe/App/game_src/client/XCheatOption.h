#pragma once
#pragma once
namespace XGAME {
	// m_dwFilter
	enum {
		xBIT_PLAYER_HERO = 0x01,
		xBIT_PLAYER_UNIT= 0x02,
		xBIT_ENEMY_HERO = 0x04,
		xBIT_ENEMY_UNIT = 0x08,
		xBIT_SIDE_FILTER = 0x0f,
		// 
	};
	// m_dwOption
	enum {
		xBO_SHOW_DPCALL				= 0x0001,
		xBO_HERO_INFO_CONSOLE	= 0x0002,
		xBO_FLUSH_IMG					= 0x0004,
		xBO_FLUSH_SPR					= 0x0008,
		//
		xBO_NO_CREATE_DMG_NUM		= 0x00010000,
		xBO_NO_CREATE_HIT_SFX		= 0x00020000,
		xBO_NO_CREATE_SKILL_SFX	= 0x00040000,
		xBO_NO_CREATE_BUFF_ICON	= 0x00080000,
		xBO_STOP_AI							= 0x01000000,
	};
	// m_dwNoDraw
	enum {
		xBD_NO_DRAW_DMG_NUM			= 0x0001,
		xBD_NO_DRAW_HIT_SFX			= 0x0002,
		xBD_NO_DRAW_SKILL_SFX		= 0x0004,
		xBD_NO_DRAW_BUFF_ICON		= 0x0008,
		xBD_NO_DRAW_FACES				= 0x0010,
		xBD_NO_DRAW_FTGL				= 0x0020,
		xBD_HIDE_HPBAR					= 0x0040,
		xBD_HIDE_NAME						= 0x0080,
		xBD_NO_DRAW_PRIM_UP			= 0x0100,
	};
}

