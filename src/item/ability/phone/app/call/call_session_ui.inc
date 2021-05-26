#if defined app_call_session_ui_included
	#endinput
#endif

#define app_call_session_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:PerformUI {
	LinkedList:uiSessionNumbers, // LinkedList<PlayerText:>
	PlayerText:uiSessionTimer,
	PlayerText:uiSpeakerBtn,
	PlayerText:uiSpeakerBtnCircle
}

static
	Map:m_sessionUI, // { {Key}: {PerformUI} }
	Map:m_sessionTimer; // { {Key}: int }

PhoneDialog:CallSessionJoinErrorDlg(id, playerid, response) {
	if(AppCall_GetPhoneSessionRef(id) != Ref<LinkedList>:-1) {
		AppCall_UpdateSessionUI(id, playerid);
		return 1;
	}

	AppCall_UpdateContactUI(id, playerid);
	AppCall_UpdateContactButtonUI(id, playerid);
	return 1;
}

PhoneDialog:CallSessionDeletedDlg(id, playerid, response) {
	AppCall_UpdateContactUI(id, playerid);
	AppCall_UpdateContactButtonUI(id, playerid);
	return 1;
}

forward AppCall_SessionTimerHandler(id, playerid);
public AppCall_SessionTimerHandler(id, playerid) {
	new
		key[Key],
		Ref<LinkedList>:ss_ref = AppCall_GetPhoneSessionRef(id);
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasSessionUIKey(key) || ss_ref == Ref<LinkedList>:-1) {
		if(map_valid(m_sessionTimer) && map_has_arr_key(m_sessionTimer, key)) {
			DeletePreciseTimer(map_arr_get(m_sessionTimer, key));
			map_arr_remove(m_sessionTimer, key);
			if(!map_size(m_sessionTimer)) {
				map_delete(m_sessionTimer);
				m_sessionTimer = Map:-1;
			}
		}
		return 0;
	}

	if(!map_valid(m_sessionTimer) || !map_has_arr_key(m_sessionTimer, key)) {
		return 0;
	}

	new
		ui[PerformUI],
		elapsed_sec = gettime() - AppCall_GetSessionCreatedTime(ss_ref),
		time_format[11];
	map_arr_get_arr(m_sessionUI, key, ui);
	format(time_format, sizeof(time_format), "%02d:%02d", floatround(float(elapsed_sec) / 60.0, floatround_floor), elapsed_sec % 60);
	PlayerTextDrawSetString(playerid, ui[uiSessionTimer], time_format);
	PlayerTextDrawShow(playerid, ui[uiSessionTimer]);
	return 1;
}

stock AppCall_HasSessionUIKey(const key[Key]) {
	return (map_valid(m_sessionUI) && map_has_arr_key(m_sessionUI, key));
}

stock AppCall_DestroySessionUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasSessionUIKey(key)) {
		return 0;
	}

	new ui[PerformUI];
	map_arr_get_arr(m_sessionUI, key, ui);
	if(linked_list_valid(ui[uiSessionNumbers])) {
		for(new Iter:it = linked_list_iter(ui[uiSessionNumbers]); iter_inside(it); iter_move_next(it)) {
			PlayerTextDrawDestroy(playerid, PlayerText:iter_get(it));
		}
		linked_list_delete(ui[uiSessionNumbers]);
	}
	PlayerTextDrawDestroy(playerid, ui[uiSessionTimer]);
	PlayerTextDrawDestroy(playerid, ui[uiSpeakerBtn]);
	PlayerTextDrawDestroy(playerid, ui[uiSpeakerBtnCircle]);
	map_arr_remove_deep(m_sessionUI, key);
	if(!map_size(m_sessionUI)) {
		map_delete(m_sessionUI);
		m_sessionUI = Map:-1;
	}
	return 1;
}

stock Task:AppCall_UpdateSessionUI(id, playerid) {
	new
		Ref<LinkedList>:ss_ref = AppCall_GetPhoneSessionRef(id),
		Task:t = task_new();
	if(ss_ref == Ref<LinkedList>:-1) {
		task_set_result_ms(t, 0, 1);
		return t;
	}

	new LinkedList:participantList = pawn_unref<LinkedList>(ss_ref);
	if(!linked_list_valid(participantList)) {
		task_set_result_ms(t, 0, 1);
		return t;
	}
	task_yield(t);

	new
		key[Key],
		ui[PerformUI],
		elapsed_sec = gettime() - AppCall_GetSessionCreatedTime(ss_ref),
		time_format[11];
	format(time_format, sizeof(time_format), "%02d:%02d", floatround(float(elapsed_sec) / 60.0, floatround_floor), elapsed_sec % 60);
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasSessionUIKey(key)) {
		AppCall_DestroySessionUI(id, playerid);
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
		size = linked_list_size(participantList),
		offset = AppCall_GetSessionOffset(ss_ref),
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

	ui[uiSessionNumbers] = linked_list_new();
	new Iter:it = linked_list_iter(participantList, offset);
	iter_acquire(it);
	for(new PlayerText:numberTd, cnt = 0, phone, number[SIM_NUMBER_SIZE], contact_name[CONTACT_NAME_SIZE], Iter:phone_it; iter_inside(it) && cnt != remote_cnt; iter_move_next(it), cnt++) {
		number[0] = EOS;
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
		task_unblock(t);
		await_arr(contact_name) Contact_NameByNumberAsync(id, number);
		numberTd = CreatePlayerTextDraw(playerid, first_x, first_y, contact_name);
		PlayerTextDrawFont(playerid, numberTd, 3);
		PlayerTextDrawLetterSize(playerid, numberTd, 0.150000, 0.600000);
		PlayerTextDrawTextSize(playerid, numberTd, SIM_NUMBER_TEXT_SIZE, (safe_max[0] - safe_min[0]));
		PlayerTextDrawSetOutline(playerid, numberTd, 1);
		PlayerTextDrawSetShadow(playerid, numberTd, 0);
		PlayerTextDrawAlignment(playerid, numberTd, 2);
		PlayerTextDrawColor(playerid, numberTd, -1);
		PlayerTextDrawBackgroundColor(playerid, numberTd, 255);
		PlayerTextDrawBoxColor(playerid, numberTd, 50);
		PlayerTextDrawUseBox(playerid, numberTd, 0);
		PlayerTextDrawSetProportional(playerid, numberTd, 1);
		PlayerTextDrawSetSelectable(playerid, numberTd, 0);
		PlayerTextDrawShow(playerid, numberTd);
		first_y += SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;
		linked_list_add(ui[uiSessionNumbers], _:numberTd);
	}
	iter_release(it);

	ui[uiSessionTimer] = CreatePlayerTextDraw(playerid, first_x, first_y, time_format);
	PlayerTextDrawFont(playerid, ui[uiSessionTimer], 2);
	PlayerTextDrawLetterSize(playerid, ui[uiSessionTimer], 0.120000, 0.4800000);
	PlayerTextDrawTextSize(playerid, ui[uiSessionTimer], SIM_NUMBER_TEXT_SIZE - 2.0, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiSessionTimer], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiSessionTimer], 0);
	PlayerTextDrawAlignment(playerid, ui[uiSessionTimer], 2);
	PlayerTextDrawColor(playerid, ui[uiSessionTimer], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiSessionTimer], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiSessionTimer], 50);
	PlayerTextDrawUseBox(playerid, ui[uiSessionTimer], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiSessionTimer], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiSessionTimer], 0);
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

	PlayerTextDrawShow(playerid, ui[uiSessionTimer]);
	PlayerTextDrawShow(playerid, ui[uiSpeakerBtn]);
	PlayerTextDrawShow(playerid, ui[uiSpeakerBtnCircle]);
	if(!map_valid(m_sessionUI)) {
		m_sessionUI = map_new();
	}
	map_arr_set_arr(m_sessionUI, key, ui);
	task_set_result(t, 1);
	return t;
}

stock AppCall_RunSessionTimer(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasSessionUIKey(key)) {
		return 0;
	}

	if(!map_valid(m_sessionTimer)) {
		m_sessionTimer = map_new();
	} else if(map_has_arr_key(m_sessionTimer, key)) {
		DeletePreciseTimer(map_arr_get(m_sessionTimer, key));
	}

	map_arr_set(m_sessionTimer, key, SetPreciseTimer("AppCall_SessionTimerHandler", 1000, true, "ii", id, playerid));
	return 1;
}

stock AppCall_KillSessionTimer(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_sessionTimer) || !map_has_arr_key(m_sessionTimer, key)) {
		return 0;
	}

	DeletePreciseTimer(map_arr_get(m_sessionTimer, key));
	if(!map_size(m_sessionTimer)) {
		map_delete(m_sessionTimer);
		m_sessionTimer = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnSsJoinError(id, error) {
	new
		Iter:it = AppCall_GetIncomingUIMapIter(),
		saved_key[Key] = {-1, -1};
	if(iter_valid(it) && iter_release(it)) {
		for(new key[Key]; iter_inside(it); iter_move_next(it)) {
			iter_get_key_arr(it, key);
			if(key[ItemId] == id) {
				saved_key = key;
				return 1;
			}
		}
	}

	if(saved_key[ItemId] == -1) {
		it = AppCall_GetDialUIMapIter();
		if(iter_valid(it) && iter_release(it)) {
			for(new key[Key]; iter_inside(it); iter_move_next(it)) {
				iter_get_key_arr(it, key);
				if(key[ItemId] == id) {
					saved_key = key;
					break;
				}
			}
		}
	}

	if(saved_key[ItemId] != -1) {
		switch(error) {
			case ssErrorAlreadyInASession: {
				IPDialog_New(saved_key[ItemId], CallSessionJoinErrorDlg, "Ban hoac doi phuong dang tham gia hai cuoc goi khac nhau, mot trong hai can phai roi cuoc goi de co the goi.");
				IPDialog_UpdateUI(saved_key[ItemId], saved_key[PlayerId]);
			}
		}
	}
	return 1;
}

hook AppCall_OnSessionJoined(id) {
	printf("AppCall_OnSessionJoined(%d)", id);
	new
		number[SIM_NUMBER_SIZE],
		Ref<LinkedList>:ssref = AppCall_GetPhoneSessionRef(id),
		size = 0,
		remote;
	AppCall_GetIncomingNumber(id, number);
	remote = ISim_GetPhoneWithNumber(number);

	if(map_valid(m_sessionUI)) {
		{
			new LinkedList:ll = pawn_unref<LinkedList>(ssref);
			if(linked_list_valid(ll)) {
				size = linked_list_size(ll) - 2;
			}
		}
		for(new Iter:it = map_iter(m_sessionUI), key[Key], cnt = 0; iter_inside(it) && cnt != size; iter_move_next(it)) {
			iter_get_key_arr(it, key);
			if(key[ItemId] == remote || key[ItemId] == id) {
				continue;
			}

			if(AppCall_GetPhoneSessionRef(key[ItemId]) == ssref) {
				AppCall_UpdateSessionUI(key[ItemId], key[PlayerId]);
				printf("AppCall_OnSessionJoined(%d), update for %d", id, key[ItemId]);
				cnt++;
			}
		}
	}

	new
		Iter:it = AppCall_GetIncomingUIMapIter(),
		saved_key[2][Key] = {{-1, -1}, ...};
	if(iter_valid(it) && iter_release(it)) {
		for(new key[Key]; iter_inside(it); iter_move_next(it)) {
			iter_get_key_arr(it, key);
			if(key[ItemId] == id) {
				printf("AppCall_OnSessionJoined(%d), update for %d", id, key[ItemId]);
				saved_key[0] = key;
				break;
			}
		}
	}

	if(remote != -1) {
		it = AppCall_GetDialUIMapIter();
		if(iter_valid(it) && iter_release(it)) {
			for(new key[Key]; iter_inside(it); iter_move_next(it)) {
				iter_get_key_arr(it, key);
				if(key[ItemId] == remote) {
					printf("AppCall_OnSessionJoined(%d), update for %d", id, key[ItemId]);
					saved_key[1] = key;
					break;
				}
			}
		}
	}

	for(new i = 0; i != sizeof(saved_key); i++) {
		if(saved_key[i][ItemId] != -1 && saved_key[i][PlayerId] != -1) {
			task_unblock(1);
			await AppCall_UpdateSessionUI(saved_key[i][ItemId], saved_key[i][PlayerId]);
			AppCall_RunSessionTimer(saved_key[i][ItemId], saved_key[i][PlayerId]);
		}
	}
	return 1;
}

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasSessionUIKey(key)) {
		switch(button) {
			case PhoneButtonHangup: {
				AppCall_QuitSession(id);
				AppCall_DestroySessionUI(id, playerid);
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
	if(AppCall_HasSessionUIKey(key)) {
		AppCall_CancelIncoming(id);
	}
	AppCall_DestroyIncomingUI(id, playerid);
	return 1;
}

hook AppCall_OnSessionQuit(Ref<LinkedList>:ssref, id) {
	new LinkedList:ll = pawn_unref<LinkedList>(ssref);
	if(linked_list_valid(ll)) {
		new size = linked_list_size(ll);
		if(size <= 1) {
			AppCall_DeleteSession(ssref);
		} else if(map_valid(m_sessionUI)) {
			for(new Iter:it = map_iter(m_sessionUI), key[Key], cnt = 0; iter_inside(it) && cnt != size; iter_move_next(it)) {
				iter_get_key_arr(it, key);
				if(AppCall_GetPhoneSessionRef(key[ItemId]) == ssref) {
					AppCall_UpdateSessionUI(key[ItemId], key[PlayerId]);
					cnt++;
				}
			}
		}
	}
	return 1;
}

hook AppCall_OnSessDeleted(Ref<LinkedList>:ssref) {
	if(!map_valid(m_sessionUI)) {
		return 1;
	}

	new LinkedList:ll = pawn_unref<LinkedList>(ssref);
	if(linked_list_valid(ll)) {
		new size = linked_list_size(ll);
		for(new Iter:it = linked_list_iter(ll), Iter:map_it, key[Key], cnt = 0; iter_inside(it) && cnt != size; iter_move_next(it)) {
			for(map_it = map_iter(m_sessionUI); iter_inside(map_it); iter_move_next(map_it)) {
				iter_get_key_arr(map_it, key);
				if(key[ItemId] == iter_get(it)) {
					AppCall_DestroySessionUI(key[ItemId], key[PlayerId]);
					IPDialog_New(key[ItemId], CallSessionDeletedDlg, "Cuoc goi da ket thuc...");
					IPDialog_UpdateUI(key[ItemId], key[PlayerId]);
					cnt++;
					break;
				}
			}
		}
	}
	return 1;
}
