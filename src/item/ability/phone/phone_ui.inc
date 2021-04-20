#if defined phone_ui_included
	#endinput
#endif

#define phone_ui_included

stock IPhone_ShowUIForPlayer(id, playerid, Float:x, Float:y) {
	new model = IPhone_GetModel(id);
	if(model == -1) {
		return 0;
	}

	CallLocalFunction("IPhoneUI_OnShowAttempt", "iiff", model, playerid, x, y);
	return 1;
}

stock IPhone_GetUIAnchor(model, playerid, &Float:x, &Float:y) {
	CallLocalFunction("IPhoneUI_OnAnchorGetAttempt", "iiff", model, playerid, ref(_:x), ref(_:y));
	return 1;
}