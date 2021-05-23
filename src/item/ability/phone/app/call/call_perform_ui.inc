#if defined app_call_perform_ui_included
	#endinput
#endif

#define app_call_perform_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:PerformUI {
	LinkedList:uiRemoteNumberList, // LinkedList<PlayerText:>
	PlayerText:uiCallTimer,
	PlayerText:uiSpeakerBtn
}

static enum _:TimerEffect {
	tmStartTime,
	tmTimer
}

static
	Map:m_performUI, // { {Key}: {PerformUI} }
	Map:m_timerEffect; // { {Key}: {TimerEffect} }

forward AppCall_DialingEffectHandler(id, playerid);
public AppCall_DialingEffectHandler(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasPerformUIKey(key)) {
		if(map_valid(m_timerEffect) && map_has_arr_key(m_timerEffect, key)) {
			new eff[TimerEffect];
			map_arr_get_arr(m_timerEffect, key, eff);
			DeletePreciseTimer(eff[effTimer]);
			map_arr_remove(m_timerEffect, key);
			if(!map_size(m_timerEffect)) {
				map_delete(m_timerEffect);
				m_timerEffect = Map:-1;
			}
		}
		return 0;
	}

	if(!map_valid(m_timerEffect) || !map_has_arr_key(m_timerEffect, key)) {
		return 0;
	}

	new
		eff[TimerEffect],
		ui[PerformUI],
		extra_text[24] = "dialing";
	map_arr_get_arr(m_timerEffect, key, eff);
	map_arr_get_arr(m_performUI, key, ui);
	for(new i = 0; i != eff[effStage]; i++) {
		strins(extra_text, ".", 0);
		strcat(extra_text, ".");
	}
	PlayerTextDrawSetString(playerid, ui[uiCallTimer], extra_text);
	PlayerTextDrawShow(playerid, ui[uiCallTimer]);
	map_arr_set_cell(m_timerEffect, key, effStage, (eff[effStage] + 1) % (3 + 1));
	return 1;
}

stock AppCall_HasPerformUIKey(const key[Key]) {
	return (map_valid(m_performUI) && map_has_arr_key(m_performUI, key));
}

stock AppCall_DestroyPerformUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasPerformUIKey(key)) {
		return 0;
	}

	new ui[PerformUI];
	map_arr_get_arr(m_performUI, key, ui);
	if(linked_list_valid(ui[uiRemoteNumberList])) {
		for(new Iter:it = linked_list_iter(ui[uiRemoteNumberList]); iter_inside(it); iter_release(it)) {
			PlayerTextDrawDestroy(playerid, PlayerText:iter_get(it));
		}
	}
	PlayerTextDrawDestroy(playerid, ui[uiCallTimer]);
	PlayerTextDrawDestroy(playerid, ui[uiSpeakerBtn]);
	map_arr_remove_deep(m_performUI, key);
	if(!map_size(m_performUI)) {
		map_delete(m_performUI);
		m_performUI = Map:-1;
	}
	return 1;
}

stock AppCall_UpdatePerformUI(id, playerid) {
	if(!AppCall_HasDialStats(id)) {
		return 0;
	}

	new
		key[Key],
		number[SIM_NUMBER_SIZE],
		ui[PerformUI];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasPerformUIKey(key)) {
		AppCall_DestroyPerformUI(id, playerid);
	}

	AppCall_GetDialSimNumber(id, number);
	const
		Float:SIM_NUMBER_TEXT_SIZE = 5.00,
		Float:SIM_NUMBER_TEXT_GAP = 3.00;

	new
		Float:anc[2],
		Float:safe_min[2],
		Float:safe_max[2],
		Float:first_x,
		Float:first_y;
	IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
	IPUI_GetAnchor(id, playerid, anc[0], anc[1]);
	first_x = anc[0] + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0;
	first_y = anc[1] + safe_min[1] + SIM_NUMBER_TEXT_SIZE;

	remoteNumber = CreatePlayerTextDraw(playerid, first_x, first_y, number);
	PlayerTextDrawFont(playerid, remoteNumber, 3);
	PlayerTextDrawLetterSize(playerid, remoteNumber, 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, remoteNumber, SIM_NUMBER_TEXT_SIZE, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, remoteNumber, 1);
	PlayerTextDrawSetShadow(playerid, remoteNumber, 0);
	PlayerTextDrawAlignment(playerid, remoteNumber, 2);
	PlayerTextDrawColor(playerid, remoteNumber, -1);
	PlayerTextDrawBackgroundColor(playerid, remoteNumber, 255);
	PlayerTextDrawBoxColor(playerid, remoteNumber, 50);
	PlayerTextDrawUseBox(playerid, remoteNumber, 0);
	PlayerTextDrawSetProportional(playerid, remoteNumber, 1);
	PlayerTextDrawSetSelectable(playerid, remoteNumber, 0);
	first_y += SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;

	ui[uiCallTimer] = CreatePlayerTextDraw(playerid, first_x, first_y, "00:00");
	PlayerTextDrawFont(playerid, ui[uiCallTimer], 2);
	PlayerTextDrawLetterSize(playerid, ui[uiCallTimer], 0.120000, 0.4800000);
	PlayerTextDrawTextSize(playerid, ui[uiCallTimer], SIM_NUMBER_TEXT_SIZE - 2.0, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiCallTimer], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiCallTimer], 0);
	PlayerTextDrawAlignment(playerid, ui[uiCallTimer], 2);
	PlayerTextDrawColor(playerid, ui[uiCallTimer], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiCallTimer], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiCallTimer], 50);
	PlayerTextDrawUseBox(playerid, ui[uiCallTimer], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiCallTimer], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiCallTimer], 0);

	ui[uiCallTimer] = CreatePlayerTextDraw(playerid, first_x, first_y, "appcall:speaker");
	PlayerTextDrawFont(playerid, ui[uiCallTimer], 2);
	PlayerTextDrawLetterSize(playerid, ui[uiCallTimer], 0.120000, 0.4800000);
	PlayerTextDrawTextSize(playerid, ui[uiCallTimer], SIM_NUMBER_TEXT_SIZE - 2.0, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiCallTimer], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiCallTimer], 0);
	PlayerTextDrawAlignment(playerid, ui[uiCallTimer], 2);
	PlayerTextDrawColor(playerid, ui[uiCallTimer], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiCallTimer], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiCallTimer], 50);
	PlayerTextDrawUseBox(playerid, ui[uiCallTimer], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiCallTimer], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiCallTimer], 0);

	PlayerTextDrawShow(playerid, remoteNumber);
	PlayerTextDrawShow(playerid, ui[uiCallTimer]);
	PlayerTextDrawShow(playerid, ui[uiSpeakerBtn]);
	if(!map_valid(m_performUI)) {
		m_performUI = map_new();
	}
	map_arr_set_arr(m_performUI, key, ui);
	return 1;
}

#include <YSI_Coding\y_hooks>