#if defined phone_ui_included
	#endinput
#endif

#define phone_ui_included

stock IPhone_ShowUIForPlayer(id, playerid, Float:x, Float:y) {
	if(!IPhone_Has(id)) {
		return 0;
	}

	CallLocalFunction("IPhoneUI_OnShowAttempt", "iiff", id, playerid, x, y);
	return 1;
}

stock IPhone_GetUIAnchor(id, playerid, &Float:x, &Float:y) {
	CallLocalFunction("IPhoneUI_OnAnchorGetAttempt", "iiff", id, playerid, ref(_:x), ref(_:y));
	return 1;
}

stock IPhone_HideUIForPlayer(id, playerid) {
	CallLocalFunction("IPhoneUI_OnHideAttempt", "ii", id, playerid);
	return 1;
}