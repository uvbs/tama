#pragma once

XE_NAMESPACE_START( xHelp )
class XPropOrder;
class XPropSeq;
class XPropDialog;
class XPropIndicate;
class XPropCamera;
class XPropTouch;
class XPropReadyTouch;
class XPropDelay;
class XPropMode;
enum xtEvent {
	xHE_NONE,
	xHE_END_CAMERA_MOVING,	// 카메라 이동이 끝남
	xHE_DRAGGED_SLOT,		// 전투전 편성씬에서 부대슬롯을 드래그
};
XE_NAMESPACE_END;
