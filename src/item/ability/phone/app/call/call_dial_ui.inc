#if defined app_call_dial_ui_included
	#endinput
#endif

#define app_call_dial_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:DialUI {
	PlayerText:uiDialNumberText,
	PlayerText:uiDialExtraText
}

static Map:m_dialUI; // { {Key}: {DialUI} }

stock AppCall_UpdateDialUI(id, playerid) {

	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnPreDialed(from_sim, to_sim) {

	return 1;
}

hook AppCall_OnDialed(from_sim, to_sim) {

	return 1;
}
