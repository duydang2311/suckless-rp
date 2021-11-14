#if defined sungo_partner_entry_ui_included
	#endinput
#endif

#define sungo_partner_entry_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:EntryUI {
	PlayerText:uiLogo,
	PlayerText:uiText,
	PlayerText:uiBtnReg,
	PlayerText:uiBtnLogin
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppSunGoPartner) {
		//AppSGoPartner_UpdateEntryUI(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	//AppSGoPartner_DestroyEntryUI(id, playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {

	return 1;
}

hook IPUI_OnButtonClicked(id, playerid, button) {
	return 1;
}
