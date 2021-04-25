#if defined app_store_ui_included
	#endinput
#endif

#define app_store_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

stock AppMenu_HasUI(id, playerid) {
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppStore) {
		AppStore_ShowMainPage(id, playerid, )
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {

	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {

	return 1;
}