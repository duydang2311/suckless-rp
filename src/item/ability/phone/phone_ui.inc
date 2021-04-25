#if defined phone_ui_included
	#endinput
#endif

#define phone_ui_included

stock IPhone_ShowUIForPlayer(id, playerid, Float:x, Float:y) {
	if(!IPhone_Has(id)) {
		return 0;
	}

	CallLocalFunction("IPUI_OnShowAttempt", "iiff", id, playerid, x, y);
	task_detach();
	task_yield(1);
	await IPApp_InitStats(id, AppMenu);
	IPApp_ShowUIForPlayer(id, AppMenu, playerid);
	return 1;
}

stock IPhone_GetUIAnchor(id, playerid, &Float:x, &Float:y) {
	CallLocalFunction("IPUI_OnAnchorGetAttempt", "iiff", id, playerid, ref(_:x), ref(_:y));
	return 1;
}

stock IPhone_HideUIForPlayer(id, playerid) {
	CallLocalFunction("IPUI_OnHideAttempt", "ii", id, playerid);
	return 1;
}