LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := gamesrc_static

LOCAL_MODULE_FILENAME := libgamesrc

LOCAL_SRC_FILES := \
Civ.cpp \
Airplane.cpp \
Bullet.cpp \
Fog.cpp \
hdc_Lily.cpp \
hdc_Nighting.cpp \
Main.cpp \
Obj.cpp \
ObjEtc.cpp \
ObjHud.cpp \
ObjLua.cpp \
ObjMng.cpp \
Obj_Interface.cpp \
Property.cpp \
Sam.cpp \
Soldier.cpp \
SyncDebug.cpp \
Tank.cpp \
Tower.cpp \
Unit.cpp \
XAchievement.cpp \
XBaseProp.cpp \
XBaseScene.cpp \
XCivFactory.cpp \
XCivFontMng.cpp \
XCivLua.cpp \
XCivSprObj.cpp \
XCivToolTipSkill.cpp \
XCivWnd.cpp \
XMapCiv.cpp \
XNetworkRecv.cpp \
XProfile.cpp \
XPropGameFlow.cpp \
XPropGenerals.cpp \
XPropUpgrade.cpp \
XRollUpgradeUnit.cpp \
XSceneContinue.cpp \
XSceneEnding.cpp \
XSceneEvent.cpp \
XSceneGame.cpp \
XSceneMainMenu.cpp \
XSceneMultiplayGame.cpp \
XSceneNewGame.cpp \
XSceneOption.cpp \
XSceneProfile.cpp \
XSceneReady.cpp \
XSceneUpgrade.cpp \
XSceneUpgradeReady.cpp \
XToolTipGeneral.cpp \
XToolTipSkill.cpp \
XToolTipUnit.cpp \
XUser.cpp \
XUserMng.cpp \
civ_skill/XCIVSkillDat.cpp \
civ_skill/XCivSkillMng.cpp \
civ_skill/XCObjSkillRecv.cpp \
civ_skill/XCObjSkillUse.cpp \
civ_skill/XGeneralBuff.cpp \
civ_skill/XObjGeneral.cpp \
civ_skill/XObjStage.cpp \
client/XAppDelegate.cpp \
client/XAppMain.cpp \
map/XMap.cpp \
map/XMapLayer.cpp \
map/XMapLayerImage.cpp \
map/XMapLayerTile.cpp \
map/XTilePack.cpp \
map/XTilePackMng.cpp \
skill/XLuaSkill.cpp \
skill/XObjSkillRecv.cpp \
skill/XObjSkillUse.cpp \
skill/xSkill.cpp \
skill/XSkillDat.cpp \
skill/XSkillMng.cpp \
skill/XSkillRecvObj.cpp \
skill/XSkillUseObj.cpp \
../../../XE/XFramework/XGRoll.cpp \

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
							$(LOCAL_PATH)/../ \
							$(LOCAL_PATH)/../../../XE/Common/_XLibrary/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../../XFramework/ \
					$(LOCAL_PATH)/../Resource \
					$(LOCAL_PATH)/../../proj.android/jni/ \
					$(LOCAL_PATH)/../../../XE/XFramework/ \
					$(LOCAL_PATH)/../../../XE/Common/_XLibrary/ \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/ \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/ftgles/freetype2_android/include/ \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/lua514/src \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/curl/android/prebuilt/libcurl/include/curl \
					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
#LOCAL_WHOLE_STATIC_LIBRARIES := xframework_static

include $(BUILD_STATIC_LIBRARY)

