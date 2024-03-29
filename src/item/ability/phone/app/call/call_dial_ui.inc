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

static enum _:DialingEffect {
	effStage,
	effTimer
}

static
	Map:m_dialUI, // { {Key}: {DialUI} }
	Map:m_dialingEffect; // { {Key}: {DialingEffect} }

forward AppCall_DialingEffectHandler(id, playerid);
public AppCall_DialingEffectHandler(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasDialUIKey(key)) {
		if(map_valid(m_dialingEffect) && map_has_arr_key(m_dialingEffect, key)) {
			new eff[DialingEffect];
			map_arr_get_arr(m_dialingEffect, key, eff);
			DeletePreciseTimer(eff[effTimer]);
			map_arr_remove(m_dialingEffect, key);
			if(!map_size(m_dialingEffect)) {
				map_delete(m_dialingEffect);
				m_dialingEffect = Map:-1;
			}
		}
		return 0;
	}

	if(!map_valid(m_dialingEffect) || !map_has_arr_key(m_dialingEffect, key)) {
		return 0;
	}

	new
		eff[DialingEffect],
		ui[DialUI],
		extra_text[24] = "dialing";
	map_arr_get_arr(m_dialingEffect, key, eff);
	map_arr_get_arr(m_dialUI, key, ui);
	for(new i = 0; i != eff[effStage]; i++) {
		strins(extra_text, ".", 0);
		strcat(extra_text, ".");
	}
	PlayerTextDrawSetString(playerid, ui[uiDialExtraText], extra_text);
	PlayerTextDrawShow(playerid, ui[uiDialExtraText]);
	map_arr_set_cell(m_dialingEffect, key, effStage, (eff[effStage] + 1) % (3 + 1));
	return 1;
}

stock AppCall_HasDialUIKey(const key[Key]) {
	return (map_valid(m_dialUI) && map_has_arr_key(m_dialUI, key));
}

stock AppCall_DestroyDialUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasDialUIKey(key)) {
		return 0;
	}

	new ui[DialUI];
	map_arr_get_arr(m_dialUI, key, ui);
	PlayerTextDrawDestroy(playerid, ui[uiDialNumberText]);
	PlayerTextDrawDestroy(playerid, ui[uiDialExtraText]);
	map_arr_remove(m_dialUI, key);
	if(!map_size(m_dialUI)) {
		map_delete(m_dialUI);
		m_dialUI = Map:-1;
	}
	return 1;
}

stock Task:AppCall_UpdateDialUI(id, playerid) {
	new Task:t = task_new();
	if(!AppCall_HasDialStats(id)) {
		task_set_result_ms(t, 0, 1);
		return t;
	}

	new
		key[Key],
		number[SIM_NUMBER_SIZE],
		contact_name[CONTACT_NAME_SIZE],
		ui[DialUI];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasDialUIKey(key)) {
		AppCall_DestroyDialUI(id, playerid);
	}

	AppCall_GetDialSimNumber(id, number);
	task_unblock(t);
	await_arr(contact_name) Contact_NameByNumberAsync(id, number);
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

	ui[uiDialNumberText] = CreatePlayerTextDraw(playerid, first_x, first_y, contact_name);
	PlayerTextDrawFont(playerid, ui[uiDialNumberText], 3);
	PlayerTextDrawLetterSize(playerid, ui[uiDialNumberText], 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, ui[uiDialNumberText], SIM_NUMBER_TEXT_SIZE, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiDialNumberText], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiDialNumberText], 0);
	PlayerTextDrawAlignment(playerid, ui[uiDialNumberText], 2);
	PlayerTextDrawColor(playerid, ui[uiDialNumberText], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiDialNumberText], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiDialNumberText], 50);
	PlayerTextDrawUseBox(playerid, ui[uiDialNumberText], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiDialNumberText], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiDialNumberText], 0);
	first_y += SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;

	ui[uiDialExtraText] = CreatePlayerTextDraw(playerid, first_x, first_y, "...dialing...");
	PlayerTextDrawFont(playerid, ui[uiDialExtraText], 2);
	PlayerTextDrawLetterSize(playerid, ui[uiDialExtraText], 0.120000, 0.4800000);
	PlayerTextDrawTextSize(playerid, ui[uiDialExtraText], SIM_NUMBER_TEXT_SIZE - 2.0, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiDialExtraText], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiDialExtraText], 0);
	PlayerTextDrawAlignment(playerid, ui[uiDialExtraText], 2);
	PlayerTextDrawColor(playerid, ui[uiDialExtraText], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiDialExtraText], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiDialExtraText], 50);
	PlayerTextDrawUseBox(playerid, ui[uiDialExtraText], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiDialExtraText], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiDialExtraText], 0);

	PlayerTextDrawShow(playerid, ui[uiDialNumberText]);
	PlayerTextDrawShow(playerid, ui[uiDialExtraText]);
	if(!map_valid(m_dialUI)) {
		m_dialUI = map_new();
	}
	map_arr_set_arr(m_dialUI, key, ui);
	task_set_result(t, 1);
	return Task:0;
}

stock AppCall_RunDialingEffect(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasDialUIKey(key)) {
		return 0;
	}

	new eff[DialingEffect];
	if(!map_valid(m_dialingEffect)) {
		m_dialingEffect = map_new();
	} else if(map_has_arr_key(m_dialingEffect, key)) {
		map_arr_get_arr(m_dialingEffect, key, eff);
		DeletePreciseTimer(eff[effTimer]);
	}
	eff[effStage] = 0;
	eff[effTimer] = SetPreciseTimer("AppCall_DialingEffectHandler", 500, true, "ii", id, playerid);
	map_arr_set_arr(m_dialingEffect, key, eff);
	return 1;
}

stock AppCall_KillDialingEffect(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(map_valid(m_dialingEffect) && map_has_arr_key(m_dialingEffect, key)) {
		new eff[DialingEffect];
		map_arr_get_arr(m_dialingEffect, key, eff);
		DeletePreciseTimer(eff[effTimer]);
		map_arr_remove(m_dialingEffect, key);
		if(!map_size(m_dialingEffect)) {
			map_delete(m_dialingEffect);
			m_dialingEffect = Map:-1;
		}
		return 1;
	}
	return 0;
}

stock AppCall_UpdateDialExtraText(id, playerid, const text[]) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasDialUIKey(key)) {
		return 0;
	}

	new ui[DialUI];
	map_arr_get_arr(m_dialUI, key, ui);
	PlayerTextDrawSetString(playerid, ui[uiDialExtraText], text);
	PlayerTextDrawShow(playerid, ui[uiDialExtraText]);
	return 1;
}

stock Iter:AppCall_GetDialUIMapIter(offset = 0) {
	new Iter:it = Iter:-1;
	if(map_valid(m_dialUI)) {
		it = map_iter(m_dialUI, offset);
		iter_acquire(it);
	}
	return it;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnPreDialed(from_sim, const to_number[]) {
	new key[Key];
	AppCall_GetContactUIKeyBySim(from_sim, key);
	if(key[ItemId] != -1 && key[PlayerId] != -1) {
		task_unblock(1);
		await AppCall_UpdateDialUI(key[ItemId], key[PlayerId]);
		AppCall_RunDialingEffect(key[ItemId], key[PlayerId]);
	}
	return 1;
}

hook AppCall_OnDialError(from_phone, error) {
	if(!map_valid(m_dialUI)) {
		return 1;
	}

	for(new Iter:it = map_iter(m_dialUI), key[Key]; iter_inside(it); iter_move_next(it)) {
		iter_get_key_arr(it, key);
		if(key[ItemId] != from_phone) {
			continue;
		}

		AppCall_KillDialingEffect(key[ItemId], key[PlayerId]);
		switch(error) {
			case errRemoteBusy: {
				AppCall_UpdateDialExtraText(key[ItemId], key[PlayerId], "May ban...");
			}
			case errRemoteInvalid: {
				AppCall_UpdateDialExtraText(key[ItemId], key[PlayerId], "Khong tim thay so dien thoai...");
			}
			case errRemoteNotReachable: {
				AppCall_UpdateDialExtraText(key[ItemId], key[PlayerId], "So dien thoai ngoai vung phu song...");
			}
		}

		task_unblock(1);
		await task_ms(floatround(float(APP_CALL_DIAL_DELAY_MS) / 2.0, floatround_ceil));
		AppCall_DestroyDialUI(key[ItemId], key[PlayerId]);
		AppCall_UpdateContactUI(key[ItemId], key[PlayerId]);
		AppCall_UpdateContactButtonUI(key[ItemId], key[PlayerId]);
		break;
	}
	return 1;
}

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasDialUIKey(key)) {
		switch(button) {
			case PhoneButtonHangup: {
				AppCall_DestroyDialUI(id, playerid);
				AppCall_KillDialingEffect(id, playerid);
				AppCall_UpdateContactUI(id, playerid);
				AppCall_UpdateContactButtonUI(id, playerid);
				AppCall_CancelDial(id);
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	AppCall_DestroyDialUI(id, playerid);
	AppCall_KillDialingEffect(id, playerid);
	AppCall_CancelDial(id);
	return 1;
}

hook AppCall_OnInCallCancel(id) {
	if(!map_valid(m_dialUI)) {
		return 1;
	}

	new number[SIM_NUMBER_SIZE];
	AppCall_GetIncomingNumber(id, number);
	new phone = ISim_GetPhoneWithNumber(number);
	if(phone != -1) {
		for(new Iter:it = map_iter(m_dialUI), key[Key]; iter_inside(it); iter_move_next(it)) {
			iter_get_key_arr(it, key);
			if(key[ItemId] == phone) {
				AppCall_KillDialingEffect(key[ItemId], key[PlayerId]);
				AppCall_UpdateDialExtraText(key[ItemId], key[PlayerId], "da tu choi cuoc goi...");

				task_unblock(1);
				await task_ms(floatround(float(APP_CALL_DIAL_DELAY_MS) / 2.0, floatround_ceil));
				AppCall_DestroyDialUI(key[ItemId], key[PlayerId]);
				if(!AppCall_HasContactUIKey(key)) {
					AppCall_UpdateContactUI(key[ItemId], key[PlayerId]);
				}
				if(!AppCall_HasButtonUIKey(key)) {
					AppCall_UpdateContactButtonUI(key[ItemId], key[PlayerId]);
				}
				break;
			}
		}
	}
	return 1;
}

hook AppCall_OnInCallAccept@(id) {
	if(!map_valid(m_dialUI)) {
		return 0;
	}

	new
		number[SIM_NUMBER_SIZE],
		phone;
	AppCall_GetIncomingNumber(id, number);
	phone = ISim_GetPhoneWithNumber(number);
	if(phone != -1) {
		for(new Iter:it = map_iter(m_dialUI), key[Key]; iter_inside(it); iter_move_next(it)) {
			iter_get_key_arr(it, key);
			if(key[ItemId] == phone) {
				AppCall_DestroyDialUI(key[ItemId], key[PlayerId]);
				break;
			}
		}
	}
	return 1;
}
