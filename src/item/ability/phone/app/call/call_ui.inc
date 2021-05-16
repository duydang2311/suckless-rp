#if defined app_call_ui_included
	#endinput
#endif

#define app_call_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:UIComponent {
	PlayerText:uiSavedNameTd,
	PlayerText:uiSavedNumberTd
}

static Map:m_callUI; // { {Key}: LinkedList<{UIComponent}> }

stock AppCall_HasUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_callUI) && map_has_arr_key(m_callUI, key, size));
}

stock AppCall_DestroyUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasUIKey(key)) {
		return 0;
	}

	new LinkedList:ll = LinkedList:map_arr_get(m_callUI, key);
	if(linked_list_valid(ll)) {
		for(new Iter:it = linked_list_iter(ll), component[UIComponent]; iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, component);
			PlayerTextDrawDestroy(playerid, component[uiSavedNameTd]);
			PlayerTextDrawDestroy(playerid, component[uiSavedNumberTd]);
		}
	}
	map_arr_remove_deep(m_callUI, key);
	if(!map_size(m_callUI)) {
		map_delete(m_callUI);
		m_callUI = Map:-1;
	}
	return 1;
}

stock AppCall_UpdateUI(id, playerid) {
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
			LinkedList:ui,
			key[Key];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(!map_valid(m_callUI)) {
			m_callUI = map_new();
		} else if(map_has_arr_key(m_callUI, key)) {
			AppCall_DestroyUI(id, playerid);
		}
		ui = linked_list_new();
		map_arr_set(m_callUI, key, ui);
		for(new cnt = 0, component[UIComponent], number[SIM_NUMBER_SIZE], stats[ContactStats], Task:t, length, Float:scr_width = safe_max[0] - safe_min[0] - 3.0, Float:width = 0.0; iter_inside(it) && cnt != component_cnt; iter_move_next(it), cnt++) {
			iter_get_arr(it, stats);
			task_unblock(1);
			t = ISim_RetrieveNumber(stats[contactSim]);
			task_wait(t);
			task_get_result_str(t, number);

			length = strlen(stats[contactName]);
			width = 0.0;
			for(new i = 0; i != length; i++) {
				width += GetTextDrawCharacterWidth(stats[contactName][i], 1) * 0.15;
				if(width > scr_width) {
					strdel(stats[contactName], i - 3, length);
					strcat(stats[contactName], "...", CONTACT_NAME_SIZE);
					break;
				}
			}
			component[uiSavedNameTd] = CreatePlayerTextDraw(playerid, first_x, first_y + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), stats[contactName]);
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
			linked_list_add_arr(ui, component);
		}
	}
	iter_release(it);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppCall) {
		AppCall_UpdateUI(id, playerid);
		// AppCall_UpdateButtonUI(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	AppCall_DestroyUI(id, playerid);
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
	if(AppCall_HasUIKey(key)) {

	}
	return 1;
}

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasUIKey(key)) {
		switch(button) {
			// case PhoneButtonDown: {
			// 	new
			// 		offset = AppCall_GetListOffset(id),
			// 		ui[ListUI];
			// 	map_arr_get_arr(m_callUI, key, ui);
			// 	if(offset < (AppCall_GetListSize(id) - ui[lsComponentCount])) {
			// 		AppCall_SetListOffset(id, offset + 1);
			// 		AppCall_DestroyListUI(id, playerid);
			// 		AppCall_UpdateListUI(id, playerid);
			// 	}
			// }
			// case PhoneButtonUp: {
			// 	new offset = AppCall_GetListOffset(id);
			// 	if(offset) {
			// 		AppCall_SetListOffset(id, offset - 1);
			// 		AppCall_DestroyListUI(id, playerid);
			// 		AppCall_UpdateListUI(id, playerid);
			// 	}
			// }
			case PhoneButtonHangup: {
				IPApp_TerminateStats(id, AppCall);
				AppCall_DestroyUI(id, playerid);
				if(AppCall_HasButtonUIKey(key)) {
					AppCall_DestroyButtonUI(id, playerid);
				}
				AppMenu_UpdateAppList(id, playerid);
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook AppCall_OnContactAdded(id, saved_sim, const saved_name[]) {
	if(!map_valid(m_callUI)) {
		return 1;
	}

	for(new Iter:it = map_iter(m_callUI), key[Key]; iter_inside(it); iter_move_next(it)) {
		iter_get_key_arr(it, key);
		if(key[ItemId] == id) {
			AppCall_DestroyUI(key[ItemId], key[PlayerId]);
			AppCall_UpdateUI(key[ItemId], key[PlayerId]);
			break;
		}
	}
	return 1;
}
