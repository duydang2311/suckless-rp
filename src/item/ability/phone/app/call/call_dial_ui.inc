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

stock AppCall_UpdateDialUI(id, playerid) {
	if(!AppCall_HasDialStats(id)) {
		return 0;
	}

	new
		key[Key],
		number[SIM_NUMBER_SIZE],
		ui[DialUI];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasDialUIKey(key)) {
		AppCall_DestroyDialUI(id, playerid);
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

	ui[uiDialNumberText] = CreatePlayerTextDraw(playerid, first_x, first_y, number);
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
	return 1;
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


#include <YSI_Coding\y_hooks>

HOOK__ AppCall_OnPreDialed(from_sim, to_number[]) {
	new key[Key];
	AppCall_GetContactUIKeyBySim(from_sim, key);
	if(key[ItemId] != -1 && key[PlayerId] != -1) {
		task_unblock(1);
		await task_ticks(1);
		AppCall_UpdateDialUI(key[ItemId], key[PlayerId]);
		AppCall_RunDialingEffect(key[ItemId], key[PlayerId]);
	}
	return 1;
}

hook AppCall_OnDialed(from_sim, to_number[]) {
	new phone = ISim_GetPhoneWithNumber(to_number);
	printf("number %s, phone %d", to_number, phone);
	if(phone != -1) {
		task_unblock(1);
		new id = await Inventory_RetrievePlayerByItem(phone);
		printf("phone %d, id %d", phone, id);
		if(id != -1) {
			new playerid = Player_GetById(id);
			printf("phone %d, id %d, playerid %d", phone, id, playerid);
			if(playerid != -1) {
				SubtitleText_Show(playerid, "~g~Chuong dien thoai dang rung len...");
			}
		}
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
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}