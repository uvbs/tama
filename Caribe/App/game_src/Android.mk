LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := gamesrc_static

LOCAL_MODULE_FILENAME := libgamesrc

LOCAL_SRC_FILES := \
XWndPrivateRaid.cpp \
XAsyncMng.cpp \
XSceneBattleSub.cpp \
XSpotPrivateRaid.cpp \
XScenePrivateRaid.cpp \
XSceneTest2.cpp \
XSceneTest.cpp \
XWndStorageItemElem.cpp \
XComp.cpp \
XMsgUnit.cpp \
XWndPopupDaily.cpp \
XSpotDaily.cpp \
XWndCampaignHero.cpp \
XCampObjHero2.cpp \
XStageObjHero2.cpp \
XTemp.cpp \
XGlobalConst.cpp \
XWndBattle.cpp \
XWndCampaign.cpp \
XWndHire.cpp \
XWndQuest.cpp \
XWndResCtrl.cpp \
XWndStorage.cpp \
XWndTech.cpp \
XWndUnitOrg.cpp \
XOption.cpp \
XWndDelegator.cpp \
XWndTemplate.cpp \
XWndWorld.cpp \
XSceneLoading.cpp \
XPropBgObj.cpp \
XFSMConnector.cpp \
XWndSpots.cpp \
XPropLegion.cpp \
XGameEtc.cpp \
XCampObjHero.cpp \
XStageObjHero.cpp \
XPropUser.cpp \
XStruct.cpp \
XSceneGuildShop.cpp \
XWndObj.cpp \
XSceneBase.cpp \
XSpotCampaign.cpp \
XSpotCastle.cpp \
XSpotEtc.cpp \
XSpotJewel.cpp \
XSpotMandrake.cpp \
XSpotNpc.cpp \
XSpotSulfur.cpp \
XCampObj.cpp \
XStageObj.cpp \
XSquadron.cpp \
XPropHelp.cpp \
XSeq.cpp \
XOrder.cpp \
XPropOrder.cpp \
XPropSeq.cpp \
XPropUpgrade.cpp \
XConnector.cpp \
XStatistic.cpp \
XSceneProcess.cpp \
XSkillMng.cpp \
XEBaseFSM.cpp \
XFSMUnit.cpp \
XExpTableHero.cpp \
XEObjMngWithType.cpp \
XLegionObj.cpp \
XBaseItem.cpp \
XPropItem.cpp \
XUnitArcher.cpp \
XUnitHero.cpp \
constGame.cpp \
XAccount.cpp \
XAppLayout.cpp \
XBaseUnit.cpp \
XBattleField.cpp \
XGame.cpp \
XGameCommon.cpp \
XGameLua.cpp \
XGameWnd.cpp \
JBWnd.cpp \
JWWnd.cpp \
XHero.cpp \
XLegion.cpp \
XLoginInfo.cpp \
XPropCloud.cpp \
XPropHero.cpp \
XPropUnit.cpp \
XPropWorld.cpp \
XPropSquad.cpp \
XPropTech.cpp \
XPropCamp.cpp \
XQuestProp.cpp \
XQuestMng.cpp \
XQuest.cpp \
XQuestCon.cpp \
XSceneBattle.cpp \
XScenePatchClient.cpp \
XSceneTitle.cpp \
XSceneUnitOrg.cpp \
XSceneStorage.cpp \
XSceneShop.cpp \
XSceneWorld.cpp \
XSceneHero.cpp \
XSceneLegion.cpp \
XSceneTrader.cpp \
XSceneArmory.cpp \
XSceneTech.cpp \
XSceneTechSel.cpp \
XSceneEquip.cpp \
XSceneGuild.cpp \
XSceneReady.cpp \
XSockGameSvr.cpp \
XSockGameSvrContent.cpp \
XSockLoginSvr.cpp \
XSockLoginSvrContent.cpp \
XSpot.cpp \
XWndBattleField.cpp \
XWndCloudLayer.cpp \
XWorld.cpp \
XObjEtc.cpp \
XUnitCyclops.cpp \
XUnitTreant.cpp \
XUnitCommon.cpp \
XSquadObj.cpp \
XExpTableUser.cpp \
XPostInfo.cpp \
XPatch.cpp \
XGuild.cpp \
client\XAppDelegate.cpp \
client\XAppMain.cpp \

#skill/XAdjParam.cpp \
#skill/XBuffObj.cpp \
#skill/XESkillMng.cpp \
#skill/XLuaSkill.cpp \
#skill/xSkill.cpp \
#skill/XSkillDat.cpp \
#skill/XSkillObj.cpp \
#skill/XSkillReceiver.cpp \
#skill/XSkillUser.cpp \

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
							$(LOCAL_PATH)/../ \
							$(LOCAL_PATH)/../../../XE/Common/_XLibrary/ \
							$(LOCAL_PATH)/../../../XE/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/android/ \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../Resource \
					$(LOCAL_PATH)/../../proj.android/jni/ \
					$(LOCAL_PATH)/../../../XE/ \
					$(LOCAL_PATH)/../../../XE/Common/_XLibrary/ \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/ \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/ftgles/freetype2_android/include/ \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/lua514/src \
					$(LOCAL_PATH)/../../../XE/Third_Party_lib/curl/android/prebuilt/libcurl/include/curl \
					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
#LOCAL_WHOLE_STATIC_LIBRARIES := xframework_static

include $(BUILD_STATIC_LIBRARY)

