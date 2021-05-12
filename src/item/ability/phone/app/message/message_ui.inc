#if defined app_message_ui_included
	#endinput
#endif

#define app_message_ui_included

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
static enum _:DetailUI {
	dtlMessageCount,
	PlayerText:dtlHeaderTd,
	LinkedList:dtlMessageTdList
}

static
	Map:m_listUI, // { [Key]: {ListUI} }
	Map:m_detailUI;

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

stock AppMsg_DestroyDetailUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppMsg_HasDetailUIKey(key)) {
		return 0;
	}
	new ui[DetailUI];
	map_arr_get_arr(m_detailUI, key, ui);
	PlayerTextDrawDestroy(playerid, ui[dtlHeaderTd]);
	if(linked_list_valid(ui[dtlMessageTdList])) {
		for(new Iter:it = linked_list_iter(ui[dtlMessageTdList]); iter_inside(it); iter_move_next(it)) {
			PlayerTextDrawDestroy(playerid, PlayerText:iter_get(it));
		}
	}
	map_arr_remove_deep(m_detailUI, key);
	return 1;
}

stock AppMsg_HasListUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_listUI) && map_has_arr_key(m_listUI, key, size));
}

stock AppMsg_HasDetailUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_detailUI) && map_has_arr_key(m_detailUI, key, size));
}

stock AppMsg_UpdateMsgList(id, playerid) {
	if(!AppMsg_HasListStats(id)) { // TODO: print 'Sim not found' on the phone screen.
		return 0;
	}

	new
		offset = AppMsg_GetListOffset(id),
		Iter:it = AppMsg_GetListIter(id, offset);
	if(iter_valid(it) && iter_release(it)) {
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
			stats[MessageStats],
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
		for(new cnt = 0, component[ListTdComponent], number[SIM_NUMBER_SIZE]; iter_inside(it) && cnt != component_cnt; iter_move_next(it), cnt++) {
			iter_get_arr(it, stats);
			ISim_GetNumber(stats[msgFromSim], number);

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

			component[lsMessageTd] = CreatePlayerTextDraw(playerid, first_x, first_y + HEADER_TO_TEXT_GAP + HEADER_SIZE + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), stats[msgContent]);
			PlayerTextDrawFont(playerid, component[lsMessageTd], 1);
			PlayerTextDrawLetterSize(playerid, component[lsMessageTd], 0.119998, 0.479999);
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
	return 1;
}

stock AppMsg_UpdateMsgDetail(id, playerid) {
	if(!AppMsg_HasDetailStats(id)) {
		return 0;
	}

	new
		offset = AppMsg_GetDetailOffset(id),
		Iter:it = AppMsg_GetDetailIter(id, offset);
	if(iter_valid(it) && iter_release(it)) {
		const
			Float:HEADER_SIZE = 10.00,
			Float:HEADER_GAP = 5.00,
			Float:TEXT_SIZE = 5.00,
			Float:MINIMUM_MESSAGE_GAP = 10.00;

		new
			ui[DetailUI],
			stats[MessageStats],
			key[Key],
			Float:anc[2],
			Float:safe_min[2],
			Float:safe_max[2],
			from_sim = AppMsg_GetSimFromDetail(id),
			from_sim_number[SIM_NUMBER_SIZE];
		IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
		IPUI_GetAnchor(id, playerid, anc[0], anc[1]);
		ISim_GetNumber(from_sim, from_sim_number);

		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(!map_valid(m_detailUI)) {
			m_detailUI = map_new();
		} else if(map_has_arr_key(m_detailUI, key)) {
			AppMsg_DestroyDetailUI(id, playerid);
		}
		ui[dtlMessageTdList] = linked_list_new();

		ui[dtlHeaderTd] = CreatePlayerTextDraw(playerid, anc[0] + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0, anc[1] + safe_min[1], from_sim_number);
		PlayerTextDrawFont(playerid, ui[dtlHeaderTd], 0);
		PlayerTextDrawLetterSize(playerid, ui[dtlHeaderTd], 0.200000, 0.800000);
		PlayerTextDrawTextSize(playerid, ui[dtlHeaderTd], 400.000000, 82.000000);
		PlayerTextDrawSetOutline(playerid, ui[dtlHeaderTd], 1);
		PlayerTextDrawSetShadow(playerid, ui[dtlHeaderTd], 0);
		PlayerTextDrawAlignment(playerid, ui[dtlHeaderTd], 2);
		PlayerTextDrawColor(playerid, ui[dtlHeaderTd], -1);
		PlayerTextDrawBackgroundColor(playerid, ui[dtlHeaderTd], 255);
		PlayerTextDrawBoxColor(playerid, ui[dtlHeaderTd], 50);
		PlayerTextDrawUseBox(playerid, ui[dtlHeaderTd], 0);
		PlayerTextDrawSetProportional(playerid, ui[dtlHeaderTd], 1);
		PlayerTextDrawSetSelectable(playerid, ui[dtlHeaderTd], 0);
		PlayerTextDrawShow(playerid, ui[dtlHeaderTd]);

		safe_min[1] += HEADER_SIZE + HEADER_GAP;

		new
			Float:scr_width,
			Float:scr_height,
			Float:width,
			length,
			line,
			content[80] = "",
			Float:height = 0.00,
			Float:last_height = 0.00,
			msg_cnt = 0,
			idx;
		scr_width = safe_max[0] - safe_min[0] - 5.0;
		scr_height = safe_max[1] - safe_min[1] - 10.0;
		for(new PlayerText:msgTd; iter_inside(it); iter_move_next(it)) {
			if(height < scr_height) {
				iter_get_arr(it, stats);
				width = 0;
				line = 1;
				msg_cnt++;
				content[0] = EOS;
				idx = 0;
				length = strlen(stats[msgContent]);
				for(new i = 0; i != length; i++) {
					width += GetTextDrawCharacterWidth(stats[msgContent][i], 1, true) * 0.15;
					if(width >= scr_width) {
						height += TEXT_SIZE;
						if(height >= scr_height) {
							content[idx - 3] = '.';
							content[idx - 2] = '.';
							content[idx - 1] = '.';
							msg_cnt--;
							break;
						} else {
							width = GetTextDrawCharacterWidth(stats[msgContent][i], 1, true) * 0.15;
							content[idx++] = '~';
							content[idx++] = 'n';
							content[idx++] = '~';
							line++;
						}
					}
					// strcat(content, stats[msgContent][i]);
					content[idx++] = (stats[msgContent][i] == ' ') ? '_' : stats[msgContent][i];
				}
				content[idx] = EOS;
				if(stats[msgFromSim] == from_sim) {
					msgTd = CreatePlayerTextDraw(playerid, anc[0] + safe_min[0], anc[1] + safe_min[1] + last_height, content);
					PlayerTextDrawFont(playerid, msgTd, 1);
					PlayerTextDrawLetterSize(playerid, msgTd, 0.150000, 0.600000);
					PlayerTextDrawTextSize(playerid, msgTd, anc[0] + safe_min[0] + scr_width, TEXT_SIZE);
					PlayerTextDrawSetOutline(playerid, msgTd, 1);
					PlayerTextDrawSetShadow(playerid, msgTd, 0);
					PlayerTextDrawAlignment(playerid, msgTd, 1);
					PlayerTextDrawColor(playerid, msgTd, 0x7A7A7AFF);
					PlayerTextDrawBackgroundColor(playerid, msgTd, 255);
					PlayerTextDrawBoxColor(playerid, msgTd, 50);
					PlayerTextDrawUseBox(playerid, msgTd, 0);
					PlayerTextDrawSetProportional(playerid, msgTd, 1);
					PlayerTextDrawSetSelectable(playerid, msgTd, 0);
				} else {
					msgTd = CreatePlayerTextDraw(playerid, anc[0] + safe_max[0], anc[1] + safe_min[1] + last_height, content);
					PlayerTextDrawFont(playerid, msgTd, 1);
					PlayerTextDrawLetterSize(playerid, msgTd, 0.150000, 0.600000);
					PlayerTextDrawTextSize(playerid, msgTd, anc[0] + safe_max[0] + scr_width, TEXT_SIZE);
					PlayerTextDrawSetOutline(playerid, msgTd, 1);
					PlayerTextDrawSetShadow(playerid, msgTd, 0);
					PlayerTextDrawAlignment(playerid, msgTd, 3);
					PlayerTextDrawColor(playerid, msgTd, 0xBDBDBDFF);
					PlayerTextDrawBackgroundColor(playerid, msgTd, 255);
					PlayerTextDrawBoxColor(playerid, msgTd, 50);
					PlayerTextDrawUseBox(playerid, msgTd, 0);
					PlayerTextDrawSetProportional(playerid, msgTd, 1);
					PlayerTextDrawSetSelectable(playerid, msgTd, 1);
				}
				PlayerTextDrawShow(playerid, msgTd);
				linked_list_add(ui[dtlMessageTdList], _:msgTd);
			}
			height += MINIMUM_MESSAGE_GAP;
			last_height = height;
		}

		ui[dtlMessageCount] = msg_cnt;
		map_arr_set_arr(m_detailUI, key, ui);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppMessage) {
		AppMsg_UpdateMsgList(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	AppMsg_DestroyDetailUI(id, playerid);
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
	if(AppMsg_HasDetailUIKey(key)) {

	} else if(AppMsg_HasListUIKey(key)) {
		new ui[ListUI];
		map_arr_get_arr(m_listUI, key, ui);
		if(linked_list_valid(ui[lsComponentList])) {
			for(new Iter:it = linked_list_iter(ui[lsComponentList]), component[ListTdComponent]; iter_inside(it); iter_move_next(it)) {
				iter_get_arr(it, component);
				if(component[lsHeaderTd] == playertextid) {
					new
						offset = AppMsg_GetListOffset(id),
						Iter:msg_it = AppMsg_GetListIter(id, offset);
					if(iter_valid(msg_it) && iter_release(msg_it)) {
						new stats[MessageStats];
						iter_get_arr(msg_it, stats);
						AppMsg_DestroyListUI(id, playerid);

						task_unblock(Y_HOOKS_BREAK_RETURN_1);
						await AppMsg_RetrieveDetailStats(id, stats[msgFromSim]);
						AppMsg_UpdateMsgDetail(id, playerid);
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
	if(AppMsg_HasDetailUIKey(key)) {
		switch(button) {
			case PhoneButtonDown: {
				new
					offset = AppMsg_GetDetailOffset(id),
					ui[DetailUI];
				map_arr_get_arr(m_detailUI, key, ui);
				if(offset < (AppMsg_GetDetailSize(id) - ui[dtlMessageCount])) {
					AppMsg_SetDetailOffset(id, offset + 1);
					AppMsg_DestroyDetailUI(id, playerid);
					AppMsg_UpdateMsgDetail(id, playerid);
				}
			}
			case PhoneButtonUp: {
				new offset = AppMsg_GetDetailOffset(id);
				if(offset) {
					AppMsg_SetDetailOffset(id, offset - 1);
					AppMsg_DestroyDetailUI(id, playerid);
					AppMsg_UpdateMsgDetail(id, playerid);
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	} else if(AppMsg_HasListUIKey(key)) {
		switch(button) {
			case PhoneButtonDown: {
				new
					offset = AppMsg_GetListOffset(id),
					ui[ListUI];
				map_arr_get_arr(m_listUI, key, ui);
				if(offset < (AppMsg_GetListSize(id) - ui[lsComponentCount])) {
					AppMsg_SetListOffset(id, offset + 1);
					AppMsg_DestroyListUI(id, playerid);
					AppMsg_UpdateMsgList(id, playerid);
				}
			}
			case PhoneButtonUp: {
				new offset = AppMsg_GetListOffset(id);
				if(offset) {
					AppMsg_SetListOffset(id, offset - 1);
					AppMsg_DestroyListUI(id, playerid);
					AppMsg_UpdateMsgList(id, playerid);
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}