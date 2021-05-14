#if defined app_message_list_ui_included
	#endinput
#endif

#define app_message_list_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:ListTdComponent {
	PlayerText:lsHeaderTd,
	PlayerText:lsMessageTd
}

static enum _:ListUI {
	lsComponentCount,
	LinkedList:lsComponentList
}

static Map:m_listUI; // { [Key]: {ListUI} }

stock AppMsg_DestroyListUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppMsg_HasListUIKey(key)) {
		return 0;
	}
	new ui[ListUI];
	map_arr_get_arr(m_listUI, key, ui);
	if(linked_list_valid(ui[lsComponentList])) {
		for(new Iter:it = linked_list_iter(ui[lsComponentList]), component[ListTdComponent]; iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, component);
			PlayerTextDrawDestroy(playerid, component[lsHeaderTd]);
			PlayerTextDrawDestroy(playerid, component[lsMessageTd]);
		}
	}
	map_arr_remove_deep(m_listUI, key);
	return 1;
}

stock AppMsg_HasListUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_listUI) && map_has_arr_key(m_listUI, key, size));
}

stock AppMsg_UpdateListUI(id, playerid) {
	if(!AppMsg_HasListStats(id)) { // TODO: print 'Sim not found' on the phone screen.
		return 0;
	}

	new
		offset = AppMsg_GetListOffset(id),
		Iter:it = AppMsg_GetListIter(id, offset);
	if(iter_valid(it)) { // cannot iter_release here, use iter_release below this scope because there are asynchronous processes in this scope.
		const
			Float:HEADER_SIZE = 6.00,
			Float:HEADER_TO_TEXT_GAP = 2.00,
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
		if((AppMsg_GetListSize(id) - offset) < component_cnt) {
			component_cnt = (AppMsg_GetListSize(id) - offset);
		}

		component_gap = (safe_max[1] - safe_min[1] - component_cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE)) / component_cnt;
		if(component_gap > (2.0 * MINIMUM_COMPONENT_GAP)) {
			component_gap = (2.0 * MINIMUM_COMPONENT_GAP);
		}
		first_x = anc[0] + safe_min[0]; // Align left
		first_y = anc[1] + safe_min[1] + (safe_max[1] - safe_min[1]) / 2.0 - (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE) / 2.0; // center position of the screen because textdraw is displayed from top to bottom without vertical centering
		if(component_cnt % 2 == 0) {
			first_y -= (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap) / 2.0 + (floatround(float(component_cnt - 1) / 2.0, floatround_floor)) * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap);
		} else {
			first_y -= (floatround(float(component_cnt - 1) / 2.0, floatround_floor)) * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap);
		}

		new
			ui[ListUI],
			key[Key];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(!map_valid(m_listUI)) {
			m_listUI = map_new();
		} else if(map_has_arr_key(m_listUI, key)) {
			AppMsg_DestroyListUI(id, playerid);
		}
		ui[lsComponentList] = linked_list_new();
		ui[lsComponentCount] = component_cnt;
		map_arr_set_arr(m_listUI, key, ui);
		for(new cnt = 0, component[ListTdComponent], number[SIM_NUMBER_SIZE], stats[MessageStats], Task:t, length, Float:scr_width = safe_max[0] - safe_min[0] - 3.0; iter_inside(it) && cnt != component_cnt; iter_move_next(it), cnt++) {
			iter_get_arr(it, stats);
			task_unblock(1);
			t = ISim_RetrieveNumber(stats[msgFromSim]);
			task_wait(t);
			task_get_result_str(t, number);

			length = strlen(number);
			for(new i = 0, Float:width = 0.0; i != length; i++) {
				width += GetTextDrawCharacterWidth(number[i], 3) * 0.15;
				if(width > scr_width) {
					strdel(number, i - 3, length);
					strcat(number, "...");
					break;
				}
			}
			component[lsHeaderTd] = CreatePlayerTextDraw(playerid, first_x, first_y + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), number);
			PlayerTextDrawFont(playerid, component[lsHeaderTd], 3);
			PlayerTextDrawLetterSize(playerid, component[lsHeaderTd], 0.150000, 0.600000);
			PlayerTextDrawTextSize(playerid, component[lsHeaderTd], 593.000000, HEADER_SIZE);
			PlayerTextDrawSetOutline(playerid, component[lsHeaderTd], 1);
			PlayerTextDrawSetShadow(playerid, component[lsHeaderTd], 0);
			PlayerTextDrawAlignment(playerid, component[lsHeaderTd], 1);
			PlayerTextDrawColor(playerid, component[lsHeaderTd], -1);
			PlayerTextDrawBackgroundColor(playerid, component[lsHeaderTd], 255);
			PlayerTextDrawBoxColor(playerid, component[lsHeaderTd], 50);
			PlayerTextDrawUseBox(playerid, component[lsHeaderTd], 0);
			PlayerTextDrawSetProportional(playerid, component[lsHeaderTd], 1);
			PlayerTextDrawSetSelectable(playerid, component[lsHeaderTd], 1);

			length = strlen(stats[msgContent]);
			for(new i = 0, Float:width = 0.0; i != length; i++) {
				width += GetTextDrawCharacterWidth(stats[msgContent][i], 1) * 0.12;
				if(width > scr_width) {
					strdel(stats[msgContent], i - 3, length);
					strcat(stats[msgContent], "...");
					break;
				}
			}
			component[lsMessageTd] = CreatePlayerTextDraw(playerid, first_x, first_y + HEADER_TO_TEXT_GAP + HEADER_SIZE + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), stats[msgContent]);
			PlayerTextDrawFont(playerid, component[lsMessageTd], 1);
			PlayerTextDrawLetterSize(playerid, component[lsMessageTd], 0.12000, 0.48000);
			PlayerTextDrawTextSize(playerid, component[lsMessageTd], 594.000000, 82.000000);
			PlayerTextDrawSetOutline(playerid, component[lsMessageTd], 1);
			PlayerTextDrawSetShadow(playerid, component[lsMessageTd], 0);
			PlayerTextDrawAlignment(playerid, component[lsMessageTd], 1);
			PlayerTextDrawColor(playerid, component[lsMessageTd], -1111638529);
			PlayerTextDrawBackgroundColor(playerid, component[lsMessageTd], 255);
			PlayerTextDrawBoxColor(playerid, component[lsMessageTd], 50);
			PlayerTextDrawUseBox(playerid, component[lsMessageTd], 0);
			PlayerTextDrawSetProportional(playerid, component[lsMessageTd], 1);
			PlayerTextDrawSetSelectable(playerid, component[lsMessageTd], 0);

			PlayerTextDrawShow(playerid, component[lsHeaderTd]);
			PlayerTextDrawShow(playerid, component[lsMessageTd]);
			linked_list_add_arr(ui[lsComponentList], component);
		}
	}
	iter_release(it);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppMessage) {
		AppMsg_UpdateListUI(id, playerid);
		AppMsg_UpdateButtonUI(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	AppMsg_DestroyListUI(id, playerid);
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
	if(AppMsg_HasListUIKey(key)) {
		new ui[ListUI];
		map_arr_get_arr(m_listUI, key, ui);
		if(linked_list_valid(ui[lsComponentList])) {
			for(new Iter:it = linked_list_iter(ui[lsComponentList]), component[ListTdComponent], cnt = 0; iter_inside(it); iter_move_next(it), cnt++) {
				iter_get_arr(it, component);
				if(component[lsHeaderTd] == playertextid) {
					new
						offset = AppMsg_GetListOffset(id),
						Iter:msg_it = AppMsg_GetListIter(id, offset + cnt);
					if(iter_valid(msg_it) && iter_release(msg_it)) {
						new stats[MessageStats];
						iter_get_arr(msg_it, stats);
						AppMsg_DestroyListUI(id, playerid);

						task_unblock(Y_HOOKS_BREAK_RETURN_1);
						await AppMsg_RetrieveDetailStats(id, stats[msgFromSim]);
						AppMsg_UpdateDetailUI(id, playerid);
						return Y_HOOKS_BREAK_RETURN_1;
					}
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
	if(AppMsg_HasListUIKey(key)) {
		switch(button) {
			case PhoneButtonDown: {
				new
					offset = AppMsg_GetListOffset(id),
					ui[ListUI];
				map_arr_get_arr(m_listUI, key, ui);
				if(offset < (AppMsg_GetListSize(id) - ui[lsComponentCount])) {
					AppMsg_SetListOffset(id, offset + 1);
					AppMsg_DestroyListUI(id, playerid);
					AppMsg_UpdateListUI(id, playerid);
				}
			}
			case PhoneButtonUp: {
				new offset = AppMsg_GetListOffset(id);
				if(offset) {
					AppMsg_SetListOffset(id, offset - 1);
					AppMsg_DestroyListUI(id, playerid);
					AppMsg_UpdateListUI(id, playerid);
				}
			}
			case PhoneButtonHangup: {
				IPApp_TerminateStats(id, AppMessage);
				AppMsg_DestroyListUI(id, playerid);
				AppMenu_UpdateAppList(id, playerid);
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook AppMsg_UIOnMsgSent(msg_id, from_sim, to_sim) {
	if(!map_valid(m_listUI)) {
		return 1;
	}

	new keyToUpdate[2][Key] = {{-1, -1}, ...};
	for(new Iter:it = map_iter(m_listUI), Iter:phone_it, key[Key], cnt = 0; iter_inside(it) && cnt != 2; iter_move_next(it)) {
		iter_get_key_arr(it, key);
		if(!IGroup_Has(key[ItemId])) {
			continue;
		}

		phone_it = IGroup_GetIter(key[ItemId]);
		if(iter_valid(phone_it) && iter_release(phone_it)) {
			for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
				sim = iter_get(phone_it);
				if(sim == from_sim || sim == to_sim) {
					if(AppMsg_HasListUIKey(key)) {
						keyToUpdate[cnt] = key;
						cnt++;
					}
					break;
				}
			}
		}
	}
	for(new i = 0; i != sizeof(keyToUpdate); i++) {
		if(keyToUpdate[i][ItemId] != -1) {
			AppMsg_DestroyListUI(keyToUpdate[i][ItemId], keyToUpdate[i][PlayerId]);
			AppMsg_UpdateListUI(keyToUpdate[i][ItemId], keyToUpdate[i][PlayerId]);
		}
	}
	return 1;
}