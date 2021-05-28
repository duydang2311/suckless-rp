#if defined app_call_incoming_ui_included
	#endinput
#endif

#define app_call_incoming_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:IncomingUI {
	PlayerText:uiIncNumberText,
	PlayerText:uiIncExtraText
}

static Map:m_incomingUI; // { {Key}: {IncomingUI} }

stock AppCall_HasIncomingUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_incomingUI) && map_has_arr_key(m_incomingUI, key, size));
}

stock AppCall_DestroyIncomingUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasIncomingUIKey(key)) {
		return 0;
	}

	new ui[IncomingUI];
	map_arr_get_arr(m_incomingUI, key, ui);
	PlayerTextDrawDestroy(playerid, ui[uiIncNumberText]);
	PlayerTextDrawDestroy(playerid, ui[uiIncExtraText]);
	map_arr_remove(m_incomingUI, key);
	if(!map_size(m_incomingUI)) {
		map_delete(m_incomingUI);
		m_incomingUI = Map:-1;
	}
	return 1;
}

stock AppCall_UpdateIncomingUI(id, playerid) {
	printf("AppCall_UpdateIncomingUI(%d, %d), has %d", id, playerid, AppCall_HasIncoming(id));
	if(!AppCall_HasIncoming(id)) {
		return 0;
	}

	new
		key[Key],
		number[SIM_NUMBER_SIZE],
		contact_name[CONTACT_NAME_SIZE],
		ui[IncomingUI];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasIncomingUIKey(key)) {
		AppCall_DestroyIncomingUI(id, playerid);
	}

	AppCall_GetIncomingNumber(id, number);
	task_unblock(1);
	await_arr(contact_name) Contact_NameByNumberAsync(id, number);
	printf("AppCall_UpdateIncomingUI(%d, %d), has %d, contact name %s", id, playerid, AppCall_HasIncoming(id), contact_name);
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

	ui[uiIncNumberText] = CreatePlayerTextDraw(playerid, first_x, first_y, contact_name);
	PlayerTextDrawFont(playerid, ui[uiIncNumberText], 3);
	PlayerTextDrawLetterSize(playerid, ui[uiIncNumberText], 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, ui[uiIncNumberText], SIM_NUMBER_TEXT_SIZE, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiIncNumberText], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiIncNumberText], 0);
	PlayerTextDrawAlignment(playerid, ui[uiIncNumberText], 2);
	PlayerTextDrawColor(playerid, ui[uiIncNumberText], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiIncNumberText], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiIncNumberText], 50);
	PlayerTextDrawUseBox(playerid, ui[uiIncNumberText], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiIncNumberText], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiIncNumberText], 0);
	first_y += SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;

	ui[uiIncExtraText] = CreatePlayerTextDraw(playerid, first_x, first_y, "dang goi den ban...");
	PlayerTextDrawFont(playerid, ui[uiIncExtraText], 2);
	PlayerTextDrawLetterSize(playerid, ui[uiIncExtraText], 0.120000, 0.4800000);
	PlayerTextDrawTextSize(playerid, ui[uiIncExtraText], SIM_NUMBER_TEXT_SIZE - 2.0, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiIncExtraText], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiIncExtraText], 0);
	PlayerTextDrawAlignment(playerid, ui[uiIncExtraText], 2);
	PlayerTextDrawColor(playerid, ui[uiIncExtraText], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiIncExtraText], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiIncExtraText], 50);
	PlayerTextDrawUseBox(playerid, ui[uiIncExtraText], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiIncExtraText], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiIncExtraText], 0);

	PlayerTextDrawShow(playerid, ui[uiIncNumberText]);
	PlayerTextDrawShow(playerid, ui[uiIncExtraText]);
	if(!map_valid(m_incomingUI)) {
		m_incomingUI = map_new();
	}
	map_arr_set_arr(m_incomingUI, key, ui);
	return 1;
}

stock Iter:AppCall_GetIncomingUIMapIter(offset = 0) {
	new Iter:it = Iter:-1;
	if(map_valid(m_incomingUI)) {
		it = map_iter(m_incomingUI, offset);
		iter_acquire(it);
	}
	return it;
}

stock AppCall_UpdateIncExtraText(id, playerid, const text[]) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasIncomingUIKey(key)) {
		return 0;
	}

	new ui[IncomingUI];
	map_arr_get_arr(m_incomingUI, key, ui);
	PlayerTextDrawSetString(playerid, ui[uiIncExtraText], text);
	PlayerTextDrawShow(playerid, ui[uiIncExtraText]);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasIncomingUIKey(key)) {
		switch(button) {
			case PhoneButtonCall: {
				AppCall_AcceptIncoming(id);
				AppCall_DestroyIncomingUI(id, playerid);
			}
			case PhoneButtonHangup: {
				AppCall_CancelIncoming(id);
				AppCall_DestroyIncomingUI(id, playerid);
				if(AppCall_GetPhoneSessionRef(id) != Ref<LinkedList>:-1) {
					AppCall_UpdateSessionUI(id, playerid);
				} else {
					AppCall_UpdateContactUI(id, playerid);
					AppCall_UpdateContactButtonUI(id, playerid);
				}
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

hook IPUI_OnShown(id, playerid) {
	if(AppCall_HasIncoming(id)) {
		await IPApp_InitStatsAsync(id, AppCall);
		AppCall_UpdateIncomingUI(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook AppCall_OnDialCancel(id) {
	if(!map_valid(m_incomingUI)) {
		return 1;
	}
	new
		dialPhone,
		number[SIM_NUMBER_SIZE];
	AppCall_GetDialSimNumber(id, number);
	dialPhone = ISim_GetPhoneWithNumber(number);
	if(AppCall_HasIncoming(dialPhone)) {
		AppCall_GetIncomingNumber(dialPhone, number);
		if(ISim_GetPhoneWithNumber(number) == id) {
			for(new Iter:it = map_iter(m_incomingUI), key[Key]; iter_inside(it); iter_move_next(it)) {
				iter_get_key_arr(it, key);
				if(key[ItemId] == dialPhone) {
					AppCall_UpdateIncExtraText(key[ItemId], key[PlayerId], "da huy cuoc goi...");

					task_unblock(1);
					await task_ms(floatround(float(APP_CALL_DIAL_DELAY_MS) / 2.0, floatround_ceil));
					AppCall_DestroyIncomingUI(key[ItemId], key[PlayerId]);
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
	}
	return 1;
}

hook AppCall_OnDialed@0(from_phone, const to_number[]) {
	new phone = ISim_GetPhoneWithNumber(to_number);
	printf("AppCall_OnDialed@0(%d, %s) -> phone %d", from_phone, to_number, phone);
	if(phone != -1) {
		task_unblock(1);
		new id = await Player_RetrieveIdByHandItem(phone);
		printf("AppCall_OnDialed@0(%d, %s) -> phone %d, player id %d", from_phone, to_number, phone, id);
		if(id == -1) {
			task_unblock(1);
			id = await Inventory_RetrievePlayerByItem(phone);
		}
		if(id != -1) {
			new playerid = Player_GetById(id);
			printf("AppCall_OnDialed@0(%d, %s) -> phone %d, player id %d, player ingame id %d", from_phone, to_number, phone, id, playerid);
			if(playerid != -1) {
				printf("AppCall_OnDialed@0(%d, %s) -> phone %d, player id %d, player ingame id %d, ipui phone %d", from_phone, to_number, phone, id, playerid, IPUI_GetPlayerPhone(playerid));
				if(IPUI_GetPlayerPhone(playerid) == phone) {
					IPApp_HideUIForPlayer(phone, playerid);
					IPApp_InitStats(phone, AppCall);
					AppCall_UpdateIncomingUI(phone, playerid);
				} else {
					SubtitleText_Show(playerid, "~g~Chuong dien thoai dang rung len...");
				}
			}
		}
	}
	return 1;
}
