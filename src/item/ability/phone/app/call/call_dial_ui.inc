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

stock AppCall_HasDialUIKey(const key[Key]) {
	return (map_valid(m_dialUI) && map_has_arr_key(m_dialUI, key));
}

stock AppCall_UpdateDialUI(id, playerid) {
	if(!AppCall_HasDialStats(id)) {
		return 0;
	}

	new
		key[Key],
		dial_sim,
		number[SIM_NUMBER_SIZE],
		ui[DialUI];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasDialUIKey(key)) {
		AppCall_DestroyDialUI(id, playerid);
	}

	dial_sim = AppCall_GetDialSim(id);
	if()
	return 1;
}

stock AppCall_DestroyDialUI(id, playerid) {

	return 1;
}

#include <YSI_Coding\y_hooks>

HOOK__ AppCall_OnPreDialed(from_sim, to_sim) {
	new key[Key];
	AppCall_GetContactUIKeyBySim(from_sim, key);
	if(key[ItemId] != -1 && key[PlayerId] != -1) {
		task_unblock(1);
		await task_ticks(1);
		AppCall_UpdateDialUI(key[ItemId], key[PlayerId]);
	}
	return 1;
}

hook AppCall_OnDialed(from_sim, to_sim) {

	return 1;
}
