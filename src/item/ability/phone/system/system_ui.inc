#if defined phone_system_ui_included
	#endinput
#endif

#define phone_system_ui_included

stock IPSystem_HideUIForPlayer(id, playerid) {
	CallLocalFunction("IPSystem_OnUIHide", "ii", id, playerid);
	return 1;
}
