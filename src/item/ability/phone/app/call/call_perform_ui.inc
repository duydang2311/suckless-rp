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

// forward AppCall_DialingEffectHandler(id, playerid);
// public AppCall_DialingEffectHandler(id, playerid) {
// 	new key[Key];
// 	key[ItemId] = id;
// 	key[PlayerId] = playerid;
// 	if(!AppCall_HasPerformUIKey(key)) {
// 		if(map_valid(m_timerEffect) && map_has_arr_key(m_timerEffect, key)) {
// 			new eff[TimerEffect];
// 			map_arr_get_arr(m_timerEffect, key, eff);
// 			DeletePreciseTimer(eff[effTimer]);
// 			map_arr_remove(m_timerEffect, key);
// 			if(!map_size(m_timerEffect)) {
// 				map_delete(m_timerEffect);
// 				m_timerEffect = Map:-1;
// 			}
// 		}
// 		return 0;
// 	}

// 	if(!map_valid(m_timerEffect) || !map_has_arr_key(m_timerEffect, key)) {
// 		return 0;
// 	}

// 	new
// 		eff[TimerEffect],
// 		ui[PerformUI],
// 		extra_text[24] = "dialing";
// 	map_arr_get_arr(m_timerEffect, key, eff);
// 	map_arr_get_arr(m_performUI, key, ui);
// 	for(new i = 0; i != eff[effStage]; i++) {
// 		strins(extra_text, ".", 0);
// 		strcat(extra_text, ".");
// 	}
// 	PlayerTextDrawSetString(playerid, ui[uiCallTimer], extra_text);
// 	PlayerTextDrawShow(playerid, ui[uiCallTimer]);
// 	map_arr_set_cell(m_timerEffect, key, effStage, (eff[effStage] + 1) % (3 + 1));
// 	return 1;
// }

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
	if(!AppCall_HasPerformStats(id)) {
		return 0;
	}

	new
		key[Key],
		ui[PerformUI];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasPerformUIKey(key)) {
		AppCall_DestroyPerformUI(id, playerid);
	}

	const
		Float:SIM_NUMBER_TEXT_SIZE = 5.00,
		Float:SIM_NUMBER_TEXT_GAP = 3.00;

	new
		Float:anc[2],
		Float:safe_min[2],
		Float:safe_max[2],
		Float:first_x,
		remote_cnt,
		Float:remote_gap,
		size = AppCall_GetPerformRemoteSize(id),
		offset = AppCall_GetPerformOffset(id),
		Iter:it = AppCall_GetPerformRemoteIter(id, offset),
		Float:first_y;
	IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
	IPUI_GetAnchor(id, playerid, anc[0], anc[1]);
	safe_max[1] -= 25.0;
	first_x = anc[0] + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0;
	first_y = anc[1] + safe_min[1] + (safe_max[1] - safe_min[1]) / 2.0 + SIM_NUMBER_TEXT_SIZE;

	remote_cnt = floatround((safe_max[1] - safe_min[1]) / (SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP), floatround_floor);
	if((size - offset) < remote_cnt) {
		remote_cnt = (size - offset);
	}

	remote_gap = (safe_max[1] - safe_min[1] - remote_cnt * (SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP)) / remote_cnt;
	if(remote_gap > (2.0 * SIM_NUMBER_TEXT_GAP)) {
		remote_gap = (2.0 * SIM_NUMBER_TEXT_GAP);
	}

	first_y = anc[1] + safe_min[1] + (safe_max[1] - safe_min[1]) / 2.0 - (SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP) / 2.0; // center position of the screen because textdraw is displayed from top to bottom without vertical centering
	if(remote_cnt % 2 == 0) {
		first_y -= (SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP) / 2.0 + (floatround(float(remote_cnt - 1) / 2.0, floatround_floor)) * (SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP);
	} else {
		first_y -= (floatround(float(remote_cnt - 1) / 2.0, floatround_floor)) * (SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP);
	}

	ui[uiRemoteNumberList] = linked_list_new();
	if(iter_valid(it) && iter_release(it)) {
		for(new PlayerText:remoteNumber, cnt = 0, number[SIM_NUMBER_SIZE]; iter_inside(it) && cnt != remote_cnt; iter_move_next(it), cnt++) {
			iter_get_str(it, number);
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
			PlayerTextDrawShow(playerid, remoteNumber);
			first_y += SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;
			linked_list_add(ui[uiRemoteNumberList], _:remoteNumber);
		}
	}

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
	first_y += SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;

	ui[uiCallTimer] = CreatePlayerTextDraw(playerid, first_x - (15.0 / 2.0), first_y, "app:menu");
	PlayerTextDrawFont(playerid, ui[uiCallTimer], 2);
	PlayerTextDrawLetterSize(playerid, ui[uiCallTimer], 0.0000, 0.0000);
	PlayerTextDrawTextSize(playerid, ui[uiCallTimer], 15.0000, 15.0000);
	PlayerTextDrawSetOutline(playerid, ui[uiCallTimer], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiCallTimer], 0);
	PlayerTextDrawAlignment(playerid, ui[uiCallTimer], 2);
	PlayerTextDrawColor(playerid, ui[uiCallTimer], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiCallTimer], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiCallTimer], 50);
	PlayerTextDrawUseBox(playerid, ui[uiCallTimer], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiCallTimer], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiCallTimer], 1);

	PlayerTextDrawShow(playerid, ui[uiCallTimer]);
	PlayerTextDrawShow(playerid, ui[uiSpeakerBtn]);
	if(!map_valid(m_performUI)) {
		m_performUI = map_new();
	}
	map_arr_set_arr(m_performUI, key, ui);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnInCallAccept(id) {
	printf("perform_ui AppCall_OnInCallAccept(%d)", id);
	new Iter:it = AppCall_GetIncomingUIMapIter();
	if(iter_valid(it) && iter_release(it)) {
		print("AppCall_GetIncomingUIMapIter valid");
		for(new key[Key]; iter_inside(it); iter_move_next(it)) {
			iter_get_key_arr(it, key);
			printf("key{%d, %d} == %d", key[ItemId], key[PlayerId], id);
			if(key[ItemId] == id) {
				AppCall_UpdatePerformUI(key[ItemId], key[PlayerId]);
				break;
			}
		}
	}

	new
		number[SIM_NUMBER_SIZE],
		remote;
	AppCall_GetIncomingNumber(id, number);
	remote = ISim_GetPhoneWithNumber(number);
	printf("perform_ui: id %d, remote %d", id, remote);
	if(remote != -1) {
		it = AppCall_GetDialUIMapIter();
		if(iter_valid(it) && iter_release(it)) {
			print("AppCall_GetDialUIMapIter valid");
			for(new key[Key]; iter_inside(it); iter_move_next(it)) {
				iter_get_key_arr(it, key);
				if(key[ItemId] == remote) {
					AppCall_UpdatePerformUI(key[ItemId], key[PlayerId]);
					break;
				}
			}
		}
	}
	return 1;
}
