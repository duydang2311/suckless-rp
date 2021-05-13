#if defined app_message_detail_ui_included
	#endinput
#endif

#define app_message_detail_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:DetailUI {
	dtlMessageCount,
	PlayerText:dtlHeaderTd,
	LinkedList:dtlMessageTdList
}

static
	Map:m_detailUI, // { [Key]: {DetailUI} }
	Map:m_selectedMsgIter;

Dialog:MessageRemoveDialog(playerid, response, listitem, inputtext[]) {
	if(!map_valid(m_selectedMsgIter) || !map_has_key(m_selectedMsgIter, playerid)) {
		return 1;
	}

	new id = IPUI_GetPlayerPhone(playerid);
	if(id == -1) {
		return 1;
	}

	new Iter:it = Iter:map_get(m_selectedMsgIter, playerid);
	iter_release(it);
	if(response) {
		AppMsg_DeleteMsgByIter(it);
		new
			offset = AppMsg_GetDetailOffset(id),
			size = AppMsg_GetDetailSize(id);
		if(offset >= size) {
			offset = size - 1;
		}
		AppMsg_DestroyDetailUI(id, playerid);
		AppMsg_UpdateDetailUI(id, playerid);
	}
	map_remove(m_selectedMsgIter, playerid);
	if(!map_size(m_selectedMsgIter)) {
		map_delete(m_selectedMsgIter);
		m_selectedMsgIter = Map:-1;
	}
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

stock AppMsg_HasDetailUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_detailUI) && map_has_arr_key(m_detailUI, key, size));
}

stock AppMsg_UpdateDetailUI(id, playerid) {
	if(!AppMsg_HasDetailStats(id)) {
		return 0;
	}

	new
		offset = AppMsg_GetDetailOffset(id),
		Iter:it = AppMsg_GetDetailIter(id, offset);
	if(iter_valid(it)) { // cannot use iter_release straight here because there is asynchronous in this scope, release the iter below this scope is the solution.
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

		new Task:t = ISim_RetrieveNumber(from_sim);
		task_unblock(1);
		task_wait(t);
		task_get_result_str(t, from_sim_number);

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
		safe_max[1] -= 3.0 * 6.00; // make space for message_button_ui, BUTTON_SIZE == 6.0

		new
			Float:scr_width,
			Float:scr_height,
			Float:width,
			length,
			line,
			content[80] = "",
			Float:height = 0.00,
			msg_cnt = 0,
			idx;
		scr_width = safe_max[0] - safe_min[0] - 3.0;
		scr_height = safe_max[1] - safe_min[1];
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
						if(height >= scr_height) {
							content[idx - 3] = '.';
							content[idx - 2] = '.';
							content[idx - 1] = '.';
							msg_cnt--;
							break;
						} else {
							height += TEXT_SIZE;
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
					msgTd = CreatePlayerTextDraw(playerid, anc[0] + safe_min[0], anc[1] + safe_max[1] - height, content);
					PlayerTextDrawFont(playerid, msgTd, 1);
					PlayerTextDrawLetterSize(playerid, msgTd, 0.150000, 0.600000);
					PlayerTextDrawTextSize(playerid, msgTd, anc[0] + safe_min[0] + scr_width, TEXT_SIZE);
					PlayerTextDrawSetOutline(playerid, msgTd, 1);
					PlayerTextDrawSetShadow(playerid, msgTd, 0);
					PlayerTextDrawAlignment(playerid, msgTd, 1);
					PlayerTextDrawColor(playerid, msgTd, 0xA19999FF);
					PlayerTextDrawBackgroundColor(playerid, msgTd, 255);
					PlayerTextDrawBoxColor(playerid, msgTd, 50);
					PlayerTextDrawUseBox(playerid, msgTd, 0);
					PlayerTextDrawSetProportional(playerid, msgTd, 1);
					PlayerTextDrawSetSelectable(playerid, msgTd, 0);
				} else {
					msgTd = CreatePlayerTextDraw(playerid, anc[0] + safe_max[0], anc[1] + safe_max[1] - height, content);
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
		}

		ui[dtlMessageCount] = msg_cnt;
		map_arr_set_arr(m_detailUI, key, ui);
	}
	iter_release(it);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIHideAttempt(id, playerid) {
	AppMsg_DestroyDetailUI(id, playerid);
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
		new ui[DetailUI];
		map_arr_get_arr(m_detailUI, key, ui);
		if(linked_list_valid(ui[dtlMessageTdList])) {
			for(new cnt = 0, Iter:it = linked_list_iter(ui[dtlMessageTdList]); iter_inside(it); iter_move_next(it), cnt++) {
				if(playertextid == PlayerText:iter_get(it)) {
					new
						offset = AppMsg_GetDetailOffset(id),
						Iter:msg_it = AppMsg_GetDetailIter(id, offset + cnt);
					if(iter_valid(msg_it)) { // iter_release later on dialog response
						new msg[MessageStats];
						iter_get_arr(msg_it, msg);
						if(!map_valid(m_selectedMsgIter)) {
							m_selectedMsgIter = map_new();
						}
						map_set(m_selectedMsgIter, playerid, _:msg_it);
						Dialog_Show(playerid, MessageRemoveDialog, DIALOG_STYLE_MSGBOX, "Message Remove", "Ban co muon xoa tin nhan nay?", "Xoa", "<<");
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
			case PhoneButtonUp: {
				new
					offset = AppMsg_GetDetailOffset(id),
					ui[DetailUI];
				map_arr_get_arr(m_detailUI, key, ui);
				if(offset < (AppMsg_GetDetailSize(id) - ui[dtlMessageCount])) {
					AppMsg_SetDetailOffset(id, offset + 1);
					AppMsg_DestroyDetailUI(id, playerid);
					AppMsg_UpdateDetailUI(id, playerid);
				}
			}
			case PhoneButtonDown: {
				new offset = AppMsg_GetDetailOffset(id);
				if(offset) {
					AppMsg_SetDetailOffset(id, offset - 1);
					AppMsg_DestroyDetailUI(id, playerid);
					AppMsg_UpdateDetailUI(id, playerid);
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook AppMsg_UIOnMsgSent(msg_id, from_sim, to_sim, const content[]) {
	for(new Iter:it = map_iter(m_detailUI), Iter:phone_it, key[Key], cnt = 0; iter_inside(it) && cnt != 2; iter_move_next(it)) {
		iter_get_key_arr(it, key);
		if(!IGroup_Has(key[ItemId])) {
			continue;
		}

		phone_it = IGroup_GetIter(key[ItemId]);
		if(iter_valid(phone_it) && iter_release(phone_it)) {
			for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
				sim = iter_get(phone_it);
				if(sim == from_sim || sim == to_sim) {
					AppMsg_DetailPushFront(key[ItemId], msg_id, from_sim, content);
					AppMsg_DestroyDetailUI(key[ItemId], key[PlayerId]);
					AppMsg_UpdateDetailUI(key[ItemId], key[PlayerId]);
					cnt++;
					break;
				}
			}
		}
	}
	return 1;
}