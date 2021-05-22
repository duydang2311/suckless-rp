#if defined app_call_contact_ui_included
	#endinput
#endif

#define app_call_contact_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:UIComponent {
	PlayerText:uiSavedNameTd,
	PlayerText:uiSavedNumberTd
}

static enum _:ContactUI {
	uiComponentCount,
	LinkedList:uiComponentList // LinkedList<{UIComponent}>
}

static
	Map:m_contactUI, // { {Key}: {ContactUI} }
	Map:m_selectedContactIdx; // { playerid: contact_index in call_stats linked list }

Dialog:AppCallContactAction(playerid, response, listitem, inputtext[]) {
	if(!map_valid(m_selectedContactIdx) || !map_has_key(m_selectedContactIdx, playerid)) {
		return 1;
	}

	if(response) {
		new phone = IPUI_GetPlayerPhone(playerid);
		if(phone == -1 || !IGroup_Has(phone)) {
			goto done;
		}

		new Iter:phone_it = IGroup_GetIter(phone);
		if(iter_valid(phone_it) && iter_release(phone_it)) {
			for(new phone_sim; iter_inside(phone_it); iter_move_next(phone_it)) {
				phone_sim = iter_get(phone_it);
				if(ISim_Has(phone_sim)) {
					new
						contact[ContactStats],
						Iter:it = AppCall_GetIter(phone, map_get(m_selectedContactIdx, playerid));
					if(iter_valid(it) && iter_release(it)) {
						iter_get_arr(it, contact);
						switch(listitem) {
							case 0: {
								// Call
								new number[SIM_NUMBER_SIZE];
								ISim_GetNumber(contact[ContactSim], number);
								if(isnull(number)) {
									task_unblock(1);
									await_arr(number) ISim_RetrieveNumber(contact[ContactSim]);
								}
								AppCall_Dial(phone, number);
							}
							case 1: {
								// Message
								CallLocalFunction("AppMsg_OnMsgSendRequest", "ii", playerid, contact[ContactSim]);
							}
							case 2: {
								task_unblock(1);
								iter_erase(it);
								await AppCall_DeleteContact(phone_sim, contact[ContactSim]);
								CallLocalFunction("AppCall_OnContDeleted", "i", phone);
							}
						}
					}
					break;
				}
			}
		}
	}
	done:
	map_remove(m_selectedContactIdx, playerid);
	if(!map_size(m_selectedContactIdx)) {
		map_delete(m_selectedContactIdx);
		m_selectedContactIdx = Map:-1;
	}
	return 1;
}

Dialog:AppCallSpecificNumber(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(!ISim_ValidateNumber(inputtext)) {
			Dialog_Show(playerid, AppCallSpecificNumber, DIALOG_STYLE_INPUT, "Call", "> Nhap so dien thoai ban muon goi.\nSo dien thoai vua nhap khong hop le.", ">>", "<<");
			return 1;
		}

		if(!AppCall_Dial(IPUI_GetPlayerPhone(playerid), inputtext)) {
			Dialog_Show(playerid, AppCallSpecificNumber, DIALOG_STYLE_INPUT, "Call", "> Nhap so dien thoai ban muon goi.\nCuoc goi that bai, co the do dien thoai cua ban khong co sim.", ">>", "<<");
			return 1;
		}
	}
	return 1;
}

stock AppCall_HasContactUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_contactUI) && map_has_arr_key(m_contactUI, key, size));
}

stock AppCall_DestroyContactUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasContactUIKey(key)) {
		return 0;
	}

	new ui[ContactUI];
	map_arr_get_arr(m_contactUI, key, ui);
	if(linked_list_valid(ui[uiComponentList])) {
		for(new Iter:it = linked_list_iter(ui[uiComponentList]), component[UIComponent]; iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, component);
			PlayerTextDrawDestroy(playerid, component[uiSavedNameTd]);
			PlayerTextDrawDestroy(playerid, component[uiSavedNumberTd]);
		}
	}
	map_arr_remove_deep(m_contactUI, key);
	if(!map_size(m_contactUI)) {
		map_delete(m_contactUI);
		m_contactUI = Map:-1;
	}
	return 1;
}

stock AppCall_UpdateContactUI(id, playerid) {
	if(!AppCall_HasStats(id)) {
		return 0;
	}

	new
		offset = AppCall_GetOffset(id),
		Iter:it = AppCall_GetIter(id, offset);
	if(iter_valid(it)) { // cannot use iter_release straight here because there is asynchronous in this scope, release the iter below this scope is the solution.
		const
			Float:HEADER_SIZE = 5.00,
			Float:HEADER_TO_TEXT_GAP = 3.00,
			Float:TEXT_SIZE = 4.00,
			Float:MINIMUM_COMPONENT_GAP = 4.00;

		new
			component_cnt = 0,
			Float:anc[2],
			Float:safe_min[2],
			Float:safe_max[2],
			Float:first_x,
			Float:first_y,
			Float:component_gap;
		IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
		IPUI_GetAnchor(id, playerid, anc[0], anc[1]);

		// calculate how many detail component textdraw will be displayed based on safe screen and gap sizes
		component_cnt = floatround((safe_max[1] - safe_min[1]) / (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + MINIMUM_COMPONENT_GAP), floatround_floor);
		if((AppCall_GetListSize(id) - offset) < component_cnt) {
			component_cnt = (AppCall_GetListSize(id) - offset);
		}

		component_gap = (safe_max[1] - safe_min[1] - component_cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE)) / component_cnt;
		if(component_gap > (2.0 * MINIMUM_COMPONENT_GAP)) {
			component_gap = (2.0 * MINIMUM_COMPONENT_GAP);
		}
		first_x = anc[0] + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0; // Align middle
		first_y = anc[1] + safe_min[1] + (safe_max[1] - safe_min[1]) / 2.0 - (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE) / 2.0; // center position of the screen because textdraw is displayed from top to bottom without vertical centering
		if(component_cnt % 2 == 0) {
			first_y -= (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap) / 2.0 + (floatround(float(component_cnt - 1) / 2.0, floatround_floor)) * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap);
		} else {
			first_y -= (floatround(float(component_cnt - 1) / 2.0, floatround_floor)) * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap);
		}

		new
			ui[ContactUI],
			key[Key];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(map_valid(m_contactUI) && map_has_arr_key(m_contactUI, key)) {
			AppCall_DestroyContactUI(id, playerid);
		}
		if(!map_valid(m_contactUI)) {
			m_contactUI = map_new();
		}
		ui[uiComponentList] = linked_list_new();
		ui[uiComponentCount] = component_cnt;
		map_arr_set_arr(m_contactUI, key, ui);
		for(new cnt = 0, component[UIComponent], number[SIM_NUMBER_SIZE], stats[ContactStats], Task:t, length, Float:scr_width = safe_max[0] - safe_min[0] - 3.0, Float:width = 0.0; iter_inside(it) && cnt != component_cnt; iter_move_next(it), cnt++) {
			iter_get_arr(it, stats);
			task_unblock(1);
			t = ISim_RetrieveNumber(stats[ContactSim]);
			task_wait(t);
			task_get_result_str(t, number);

			length = strlen(stats[ContactName]);
			width = 0.0;
			for(new i = 0; i != length; i++) {
				width += GetTextDrawCharacterWidth(stats[ContactName][i], 1) * 0.15;
				if(width > scr_width) {
					strdel(stats[ContactName], i - 3, length);
					strcat(stats[ContactName], "...", CONTACT_NAME_SIZE);
					break;
				}
			}
			component[uiSavedNameTd] = CreatePlayerTextDraw(playerid, first_x, first_y + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), stats[ContactName]);
			PlayerTextDrawFont(playerid, component[uiSavedNameTd], 3);
			PlayerTextDrawLetterSize(playerid, component[uiSavedNameTd], 0.150000, 0.600000);
			PlayerTextDrawTextSize(playerid, component[uiSavedNameTd], HEADER_SIZE, width + 5.0);
			PlayerTextDrawSetOutline(playerid, component[uiSavedNameTd], 1);
			PlayerTextDrawSetShadow(playerid, component[uiSavedNameTd], 0);
			PlayerTextDrawAlignment(playerid, component[uiSavedNameTd], 2);
			PlayerTextDrawColor(playerid, component[uiSavedNameTd], -1);
			PlayerTextDrawBackgroundColor(playerid, component[uiSavedNameTd], 255);
			PlayerTextDrawBoxColor(playerid, component[uiSavedNameTd], 50);
			PlayerTextDrawUseBox(playerid, component[uiSavedNameTd], 0);
			PlayerTextDrawSetProportional(playerid, component[uiSavedNameTd], 1);
			PlayerTextDrawSetSelectable(playerid, component[uiSavedNameTd], 1);

			length = strlen(number);
			width = 0.0;
			for(new i = 0; i != length; i++) {
				width += GetTextDrawCharacterWidth(number[i], 3) * 0.12;
				if(width > scr_width) {
					strdel(number, i - 3, length);
					strcat(number, "...");
					break;
				}
			}

			component[uiSavedNumberTd] = CreatePlayerTextDraw(playerid, first_x, first_y + HEADER_TO_TEXT_GAP + HEADER_SIZE + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), number);
			PlayerTextDrawFont(playerid, component[uiSavedNumberTd], 1);
			PlayerTextDrawLetterSize(playerid, component[uiSavedNumberTd], 0.12000, 0.48000);
			PlayerTextDrawTextSize(playerid, component[uiSavedNumberTd], 594.000000, 6.000000);
			PlayerTextDrawSetOutline(playerid, component[uiSavedNumberTd], 1);
			PlayerTextDrawSetShadow(playerid, component[uiSavedNumberTd], 0);
			PlayerTextDrawAlignment(playerid, component[uiSavedNumberTd], 2);
			PlayerTextDrawColor(playerid, component[uiSavedNumberTd], -1111638529);
			PlayerTextDrawBackgroundColor(playerid, component[uiSavedNumberTd], 255);
			PlayerTextDrawBoxColor(playerid, component[uiSavedNumberTd], 50);
			PlayerTextDrawUseBox(playerid, component[uiSavedNumberTd], 0);
			PlayerTextDrawSetProportional(playerid, component[uiSavedNumberTd], 1);
			PlayerTextDrawSetSelectable(playerid, component[uiSavedNumberTd], 0);

			PlayerTextDrawShow(playerid, component[uiSavedNameTd]);
			PlayerTextDrawShow(playerid, component[uiSavedNumberTd]);
			linked_list_add_arr(ui[uiComponentList], component);
		}
	}
	iter_release(it);
	return 1;
}

stock AppCall_GetContactUIKeyBySim(sim, result_key[Key]) {
	result_key[ItemId] = -1;
	result_key[PlayerId] = -1;
	if(!map_valid(m_contactUI)) {
		return 0;
	}

	for(new Iter:it = map_iter(m_contactUI), key[Key]; iter_inside(it); iter_move_next(it)) {
		iter_get_key_arr(it, key);
		if(IPhone_Has(key[ItemId]) && IGroup_Has(key[ItemId])) {
			new Iter:phone_it = IGroup_GetIter(key[ItemId]);
			if(iter_valid(phone_it) && iter_release(phone_it)) {
				for(; iter_inside(phone_it); iter_move_next(phone_it)) {
					if(iter_get(phone_it) == sim) {
						result_key[ItemId] = key[ItemId];
						result_key[PlayerId] = key[PlayerId];
						return 1;
					}
				}
			}
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppCall) {
		new bool:busy = false;
		CallLocalFunction("AppCall_OnUIShowAttempt", "iii", id, playerid, ref(busy));
		if(!busy) {
			AppCall_UpdateContactUI(id, playerid);
			AppCall_UpdateContactButtonUI(id, playerid);
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	AppCall_DestroyContactUI(id, playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {
	new id = IPUI_GetPlayerPhone(playerid);
	if(id == -1) {
		return 1;
	}

	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasContactUIKey(key)) {
		new ui[ContactUI];
		map_arr_get_arr(m_contactUI, key, ui);
		if(linked_list_valid(ui[uiComponentList])) {
			for(new Iter:it = linked_list_iter(ui[uiComponentList]), cnt = 0, component[UIComponent]; iter_inside(it); iter_move_next(it), cnt++) {
				iter_get_arr(it, component);
				if(playertextid == component[uiSavedNameTd]) {
					new offset = AppCall_GetOffset(id);
					if(!map_valid(m_selectedContactIdx)) {
						m_selectedContactIdx = map_new();
					}
					map_set(m_selectedContactIdx, playerid, offset + cnt);
					Dialog_Show(playerid, AppCallContactAction, DIALOG_STYLE_LIST, "Contact Actions", "Call\nMessage\nDelete", ">>", "<<");
					return Y_HOOKS_BREAK_RETURN_1;
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
	if(AppCall_HasContactUIKey(key)) {
		switch(button) {
			case PhoneButtonDown: {
				new
					offset = AppCall_GetListOffset(id),
					ui[ContactUI];
				map_arr_get_arr(m_contactUI, key, ui);
				if(offset < (AppCall_GetListSize(id) - ui[uiComponentCount])) {
					AppCall_SetListOffset(id, offset + 1);
					AppCall_DestroyContactUI(id, playerid);
					AppCall_UpdateContactUI(id, playerid);
				}
			}
			case PhoneButtonUp: {
				new offset = AppCall_GetListOffset(id);
				if(offset) {
					AppCall_SetListOffset(id, offset - 1);
					AppCall_DestroyContactUI(id, playerid);
					AppCall_UpdateContactUI(id, playerid);
				}
			}
			case PhoneButtonHangup: {
				IPApp_TerminateStats(id, AppCall);
				AppCall_DestroyContactUI(id, playerid);
				if(AppCall_HasButtonUIKey(key)) {
					AppCall_DestroyContactButtonUI(id, playerid);
				}
				AppMenu_UpdateAppList(id, playerid);
			}
			case PhoneButtonCall: {
				Dialog_Show(playerid, AppCallSpecificNumber, DIALOG_STYLE_INPUT, "Call", "> Nhap so dien thoai ban muon goi.", ">>", "<<");
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook AppCall_OnContAdded(id, saved_sim, const saved_name[]) {
	if(!map_valid(m_contactUI)) {
		return 1;
	}

	for(new Iter:it = map_iter(m_contactUI), key[Key]; iter_inside(it); iter_move_next(it)) {
		iter_get_key_arr(it, key);
		if(key[ItemId] == id) {
			AppCall_DestroyContactUI(key[ItemId], key[PlayerId]);
			AppCall_UpdateContactUI(key[ItemId], key[PlayerId]);
			break;
		}
	}
	return 1;
}

hook AppCall_OnContDeleted(id) {
	if(!map_valid(m_contactUI)) {
		return 1;
	}

	for(new Iter:it = map_iter(m_contactUI), key[Key]; iter_inside(it); iter_move_next(it)) {
		iter_get_key_arr(it, key);
		if(key[ItemId] == id) {
			AppCall_DestroyContactUI(key[ItemId], key[PlayerId]);
			AppCall_UpdateContactUI(key[ItemId], key[PlayerId]);
			break;
		}
	}
	return 1;
}

hook AppCall_OnPreDialed(from_sim, to_number[]) {
	if(!map_valid(m_contactUI)) {
		return 1;
	}

	for(new Iter:it = map_iter(m_contactUI), key[Key]; iter_inside(it); iter_move_next(it)) {
		iter_get_key_arr(it, key);
		if(IPhone_Has(key[ItemId]) && IGroup_Has(key[ItemId])) {
			new Iter:phone_it = IGroup_GetIter(key[ItemId]);
			if(iter_valid(phone_it) && iter_release(phone_it)) {
				for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
					sim = iter_get(phone_it);
					if(sim == from_sim) {
						AppCall_DestroyContactUI(key[ItemId], key[PlayerId]);
						AppCall_DestroyContactButtonUI(key[ItemId], key[PlayerId]);
						return 1;
					}
				}
			}
		}
	}
	return 1;
}