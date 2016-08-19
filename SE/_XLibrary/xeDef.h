#pragma once

// 이 파일은 항상 최상위가 되어야 하므로 다른 헤더 include 시키지 말것.
// 오직 이 파일엔 define, enum류만 넣을것.
//#define FONT_SYSTEM		_T("NanumGothicBold.ttf")
#define	TICKS_PER_SEC		1000.0f
//#define	XSEC(S)		((DWORD)(TICKS_PER_SEC * (float)(S)))
#define XTO_SEC(TICK)		((TICK) / TICKS_PER_SEC)
#define XTO_TICK(SEC)		XSEC(SEC)
#define xSEC_TO_MIN(SEC)	((SEC) / 60)
#define xMIN_TO_SEC(MIN)	((MIN) * 60)
#define xSEC_TO_HOUR(SEC)	((SEC) / 60 / 60)
#define xHOUR_TO_SEC(HOUR)	((HOUR) * 60 * 60)
#define xMIN(M)			xMIN_TO_SEC(M)		// 분을 초로 바꿈
#define xHOUR(H)		xHOUR_TO_SEC(H)		// 시간을 초로 바꿈

class XArchive;

namespace XE {
	enum xtAlert { xMB_OK=1, 
				xMB_OKCANCEL=2,
				xMB_YESNO=3 };
	// Alert창의 리턴값
	enum xtAlertRet {
		xAR_NONE,
		xAR_OK=1,	// 윈도우의 IDOK시리즈랑 번호를 맞춤
		xAR_CANCEL=2,
		xAR_ABORT=3,
		xAR_RETRY=4,
		xAR_IGNORE=5,
		xAR_YES=6,
		xAR_NO=7,
	};
	enum xtError {
		xOK,
		XFAIL,
		xERR_FAIL_CREATE_ASIC_FONT,	// 기본 시스템 폰트 생성 실패
	};
	// XBaseRes용 ㅠㅠ;
	enum xtERROR { 
		ERR_SUCCESS = 0, 
		ERR_FAILED, 
		ERR_READONLY, 
		ERR_PERMISSION_DENIED, 
		ERR_FILE_NOT_FOUND 
	};
	enum xtDestroy { xDES_LIVE = 0,				// 살아있음
					xDES_READY,		// 최초 삭제 명령받은상태(죽을준비하는중)
					xDES_COMPLETE,	// 죽을준비끝. 이상태면 돌아오는 메인스레드턴에서 삭제가 된다.
	};
	enum xAlign 
	{
		xALIGN_NONE = 0,
		xALIGN_LEFT = 0x01,
		xALIGN_RIGHT = 0x02,
		xALIGN_HCENTER = 0x04,
		xALIGN_VCENTER = 0x08,		// 현재 지원되지 않음.
		xALIGN_CENTER = xALIGN_HCENTER | xALIGN_VCENTER,
		xALIGN_BOTTOM = 0x10,
// 		xALIGN_HCENTER_PARENT = 0x20,		// 부모의 크기에 수평정렬
// 		xALIGN_VCENTER_PARENT = 0x40,		// 
// 		xALIGN_CENTER_PARENT = xALIGN_HCENTER_PARENT | xALIGN_VCENTER_PARENT,
	};
	enum xtWnd {
		WT_NONE,
		WT_POPUP,
	};
	enum xtENCODING {
		TXT_EUCKR=0,
		TXT_UTF8,
		TXT_UTF16,
	};
	// 이리 정의되어있는 파티클 타입
	enum xtParticle {
		xPTC_NONE=0,
		xPTC_RAIN=0x8000,
		xPTC_SNOW=0x8001,
	};
	enum xtLoadType {
		xLT_NONE,
        xLT_PACKAGE_ONLY,    // 패키지에서만 읽음
        xLT_WORK_FOLDER_ONLY,   // 워킹폴더에서만 읽음
        xLT_WORK_TO_PACKAGE,     // 워킹폴더에서 읽어보고 없으면 패키지에서 찾음.
		xLT_WORK_TO_PACKAGE_COPY, // 워킹에서 읽어보고 없으면 패키지에서 찾아서 워크로 카피하여 워크에서 읽음.
		xLT_MAX,
    };
	enum xtDir {
		xDIR_NONE,
		xDIR_RIGHT = 1,
		xDIR_E = 1,
		xDIR_ES = 2,
		xDIR_DOWN = 3,
		xDIR_S = 3,
		xDIR_WS = 4,
		xDIR_LEFT = 5,
		xDIR_W = 5,
		xDIR_WN = 6,
		xDIR_UP = 7,
		xDIR_N = 7,
		xDIR_EN = 8,
		xDIR_CENTER = 9,
	};
	enum xtHorizDir { HDIR_NONE = 0, 
					HDIR_LEFT = -1, 
					HDIR_RIGHT = 1 };	// 좌우방향
	enum xtCrossDir { xCDIR_NONE=0,
					  xVERT = 0x01,
					  xHORIZ = 0x02,
					  xVERT_HORIZ_ALL = 0x03,
	};
	// 요일(Day Of the Week)
	enum xtDOW {
		xDOW_SUNDAY,
		xDOW_MONDAY,
		xDOW_TUESDAY,
		xDOW_WEDNESDAY,
		xDOW_THURSDAY,
		xDOW_FRIDAY,
		xDOW_SATURDAY,
	};
	struct xYMD {
		WORD year;
		BYTE month;
		BYTE day;
		xYMD() {
			year = 0;
			month = 0;
			day = 0;
		}
		BOOL IsActive() const {
			return year != 0;
		}
		int GetYear() const {
			return (int)year;
		}
		int GetMonth() const {
			return (int)month;
		}
		int GetDay() const {
			return (int)day;
		}
		void Serialize( XArchive& ar );
		void DeSerialize( XArchive& ar );
	};
};
