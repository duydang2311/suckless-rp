#if defined app_message_ui_included
	#endinput
#endif

#define app_message_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static
	Map:m_msgListPageTd,
	Map:m_msgDetailsPageTd;

stock AppMsg_UpdateMsgList(id, playerid) {
	if(!AppMsg_HasListStats(id)) {
		return 0;
	}

	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppStore) {

		AppMsg_UpdateMsgList(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	AppMsg_DestroyUI(id, playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {

	return 1;
}