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
	PlayerText:uiSpeakerBtn,
	PlayerText:uiSpeakerBtnCircle
}

static enum _:PerformTimer {
	tmStartTime,
	tmTimer
}

static
	Map:m_performUI, // { {Key}: {PerformUI} }
	Map:m_performTimer; // { {Key}: {PerformTimer} }

forward AppCall_PerformTimerHandler(id, playerid);
public AppCall_PerformTimerHandler(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasPerformUIKey(key)) {
		if(map_valid(m_performTimer) && map_has_arr_key(m_performTimer, key)) {
			new tm[PerformTimer];
			map_arr_get_arr(m_performTimer, key, tm);
			DeletePreciseTimer(tm[tmTimer]);
			map_arr_remove(m_performTimer, key);
			if(!map_size(m_performTimer)) {
				map_delete(m_performTimer);
				m_performTimer = Map:-1;
			}
		}
		return 0;
	}

	if(!map_valid(m_performTimer) || !map_has_arr_key(m_performTimer, key)) {
		return 0;
	}

	new
		ui[PerformUI],
		tm[PerformTimer],
		elapsed_sec,
		time_format[11];
	map_arr_get_arr(m_performTimer, key, tm);
	map_arr_get_arr(m_performUI, key, ui);
	elapsed_sec = gettime() - tm[tmStartTime];
	format(time_format, sizeof(time_format), "%02d:%02d", floatround(float(elapsed_sec) / 60.0, floatround_floor), elapsed_sec % 60);
	PlayerTextDrawSetString(playerid, ui[uiCallTimer], time_format);
	PlayerTextDrawShow(playerid, ui[uiCallTimer]);
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
	PlayerTextDrawDestroy(playerid, ui[uiSpeakerBtnCircle]);
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
		Float:SIM_NUMBER_TEXT_GAP = 3.00,
		Float:SPEAKER_BUTTON_SIZE = 20.00;

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
	safe_max[1] -= SPEAKER_BUTTON_SIZE + SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;
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
		for(new PlayerText:remoteNumber, cnt = 0, phone, number[SIM_NUMBER_SIZE], Iter:phone_it; iter_inside(it) && cnt != remote_cnt; iter_move_next(it), cnt++) {
			phone = iter_get(it);
			if(!IPhone_Has(phone) || !IGroup_Has(phone)) {
				continue;
			}
			phone_it = IGroup_GetIter(phone);
			if(!iter_valid(phone_it)) {
				continue;
			}
			iter_release(phone_it);
			for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
				sim = iter_get(phone_it);
				if(ISim_Has(sim)) {
					ISim_GetNumber(sim, number);
					break;
				}
			}
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

	ui[uiSpeakerBtnCircle] = CreatePlayerTextDraw(playerid, first_x - (SPEAKER_BUTTON_SIZE / 2.0), first_y, "shape:circle");
	PlayerTextDrawFont(playerid, ui[uiSpeakerBtnCircle], 4);
	PlayerTextDrawLetterSize(playerid, ui[uiSpeakerBtnCircle], 0.0000, 0.0000);
	PlayerTextDrawTextSize(playerid, ui[uiSpeakerBtnCircle], SPEAKER_BUTTON_SIZE, SPEAKER_BUTTON_SIZE);
	PlayerTextDrawSetOutline(playerid, ui[uiSpeakerBtnCircle], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiSpeakerBtnCircle], 0);
	PlayerTextDrawAlignment(playerid, ui[uiSpeakerBtnCircle], 2);
	PlayerTextDrawColor(playerid, ui[uiSpeakerBtnCircle], 0x131313FF);
	PlayerTextDrawBackgroundColor(playerid, ui[uiSpeakerBtnCircle], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiSpeakerBtnCircle], 50);
	PlayerTextDrawUseBox(playerid, ui[uiSpeakerBtnCircle], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiSpeakerBtnCircle], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiSpeakerBtnCircle], 0);

	ui[uiSpeakerBtn] = CreatePlayerTextDraw(playerid, first_x - ((SPEAKER_BUTTON_SIZE - (SPEAKER_BUTTON_SIZE / 2.0)) / 2.0), first_y + (SPEAKER_BUTTON_SIZE / 2.0) - ((SPEAKER_BUTTON_SIZE - (SPEAKER_BUTTON_SIZE / 2.0)) / 2.0), "appcall:speaker");
	PlayerTextDrawFont(playerid, ui[uiSpeakerBtn], 4);
	PlayerTextDrawLetterSize(playerid, ui[uiSpeakerBtn], 0.0000, 0.0000);
	PlayerTextDrawTextSize(playerid, ui[uiSpeakerBtn], SPEAKER_BUTTON_SIZE - (SPEAKER_BUTTON_SIZE / 2.0), SPEAKER_BUTTON_SIZE - (SPEAKER_BUTTON_SIZE / 2.0));
	PlayerTextDrawSetOutline(playerid, ui[uiSpeakerBtn], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiSpeakerBtn], 0);
	PlayerTextDrawAlignment(playerid, ui[uiSpeakerBtn], 2);
	PlayerTextDrawColor(playerid, ui[uiSpeakerBtn], 0xFFFFFF77);
	PlayerTextDrawBackgroundColor(playerid, ui[uiSpeakerBtn], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiSpeakerBtn], 50);
	PlayerTextDrawUseBox(playerid, ui[uiSpeakerBtn], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiSpeakerBtn], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiSpeakerBtn], 1);

	PlayerTextDrawShow(playerid, ui[uiCallTimer]);
	PlayerTextDrawShow(playerid, ui[uiSpeakerBtn]);
	PlayerTextDrawShow(playerid, ui[uiSpeakerBtnCircle]);
	if(!map_valid(m_performUI)) {
		m_performUI = map_new();
	}
	map_arr_set_arr(m_performUI, key, ui);
	return 1;
}

stock AppCall_RunPerformTimer(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasPerformUIKey(key)) {
		return 0;
	}

	new tm[PerformTimer];
	if(!map_valid(m_performTimer)) {
		m_performTimer = map_new();
	} else if(map_has_arr_key(m_performTimer, key)) {
		map_arr_get_arr(m_performTimer, key, tm);
		DeletePreciseTimer(tm[tmTimer]);
	}

	tm[tmStartTime] = gettime();
	tm[tmTimer] = SetPreciseTimer("AppCall_PerformTimerHandler", 1000, true, "ii", id, playerid);
	map_arr_set_arr(m_performTimer, key, tm);
	return 1;
}

stock AppCall_KillPerformTimer(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_performTimer) || !map_has_arr_key(m_performTimer, key)) {
		return 0;
	}

	new tm[PerformTimer];
	map_arr_get_arr(m_performTimer, key, tm);
	DeletePreciseTimer(tm[tmTimer]);
	map_arr_remove(m_performTimer, key);
	if(!map_size(m_performTimer)) {
		map_delete(m_performTimer);
		m_performTimer = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnInCallAccept(id) {
	new Iter:it = AppCall_GetIncomingUIMapIter();
	if(iter_valid(it) && iter_release(it)) {
		for(new key[Key]; iter_inside(it); iter_move_next(it)) {
			iter_get_key_arr(it, key);
			if(key[ItemId] == id) {
				AppCall_UpdatePerformUI(key[ItemId], key[PlayerId]);
				AppCall_RunPerformTimer(key[ItemId], key[PlayerId]);
				break;
			}
		}
	}

	new
		number[SIM_NUMBER_SIZE],
		remote;
	AppCall_GetIncomingNumber(id, number);
	remote = ISim_GetPhoneWithNumber(number);
	if(remote != -1) {
		it = AppCall_GetDialUIMapIter();
		if(iter_valid(it) && iter_release(it)) {
			for(new key[Key]; iter_inside(it); iter_move_next(it)) {
				iter_get_key_arr(it, key);
				if(key[ItemId] == remote) {
					AppCall_UpdatePerformUI(key[ItemId], key[PlayerId]);
					AppCall_RunPerformTimer(key[ItemId], key[PlayerId]);
					break;
				}
			}
		}
	}
	return 1;
}

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasIncomingUIKey(key)) {
		switch(button) {
			case PhoneButtonHangup: {
				AppCall_QuitPerform(id);
				AppCall_DestroyPerformUI(id, playerid);
				AppCall_UpdateContactUI(id, playerid);
				AppCall_UpdateContactButtonUI(id, playerid);
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasIncomingUIKey(key)) {
		AppCall_CancelIncoming(id);
	}
	AppCall_DestroyIncomingUI(id, playerid);
	return 1;
}
