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

static enum _:SelectedMessageStats {
	sltedOffset,
	Iter:sltedIter
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

	new selected[SelectedMessageStats];
	map_get_arr(m_selectedMsgIter, playerid, selected);
	if(response) {
		new
			to_sim = AppMsg_GetSimFromDetail(id),
			offset,
			size;

		// update UI for playerid
		AppMsg_DeleteMsgByIter(selected[sltedIter]);
		offset = AppMsg_GetDetailOffset(id);
		size = AppMsg_GetDetailSize(id);
		if(offset >= size) {
			offset = size - 1;
			AppMsg_SetDetailOffset(id, offset);
		}
		AppMsg_DestroyDetailUI(id, playerid);
		AppMsg_UpdateDetailUI(id, playerid);

		// update UI for another player
		if(map_valid(m_detailUI)) {
			for(new Iter:jt = map_iter(m_detailUI), Iter:phone_it, key[Key], bool:stop = false; iter_inside(jt) && !stop; iter_move_next(jt)) {
				iter_get_key_arr(jt, key);
				if((key[PlayerId] == playerid && key[ItemId] == id) || !IGroup_Has(key[ItemId])) {
					continue;
				}
				phone_it = IGroup_GetIter(key[ItemId]);
				if(iter_valid(phone_it) && iter_release(phone_it)) {
					for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
						sim = iter_get(phone_it);
						if(sim == to_sim) {
							new Iter:kt = AppMsg_GetDetailIter(key[ItemId], selected[sltedOffset]);
							if(iter_valid(kt) && iter_release(kt)) {
								iter_erase(kt);
							}
							offset = AppMsg_GetDetailOffset(key[ItemId]);
							size = AppMsg_GetDetailSize(key[ItemId]);
							if(offset >= size) {
								offset = size - 1;
								AppMsg_SetDetailOffset(key[ItemId], offset);
							}
							AppMsg_DestroyDetailUI(key[ItemId], key[PlayerId]);
							AppMsg_UpdateDetailUI(key[ItemId], key[PlayerId]);
							stop = true;
							break;
						}
					}
				}
			}
		}
	}
	iter_release(selected[sltedIter]);
	map_remove_deep(m_selectedMsgIter, playerid);
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
		linked_list_delete(ui[dtlMessageTdList]);
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
			from_sim_number[SIM_NUMBER_SIZE],
			contact[CONTACT_NAME_SIZE];
		IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
		IPUI_GetAnchor(id, playerid, anc[0], anc[1]);

		task_unblock(1);
		await_arr(from_sim_number) ISim_RetrieveNumber(from_sim);
		task_unblock(1);
		await_arr(contact) Contact_NameByNumberAsync(id, from_sim_number);
		// new Task:t = ISim_RetrieveNumber(from_sim);
		// task_unblock(1);
		// task_wait(t);
		// task_get_result_str(t, from_sim_number);

		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(map_valid(m_detailUI) && map_has_arr_key(m_detailUI, key)) {
			AppMsg_DestroyDetailUI(id, playerid);
		}
		if(!map_valid(m_detailUI)) {
			m_detailUI = map_new();
		}
		ui[dtlMessageTdList] = linked_list_new();

		ui[dtlHeaderTd] = CreatePlayerTextDraw(playerid, anc[0] + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0, anc[1] + safe_min[1], contact);
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
				width = 0.0;
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
						selected[SelectedMessageStats];
					selected[sltedOffset] = offset + cnt;
					selected[sltedIter] = AppMsg_GetDetailIter(id, selected[sltedOffset]);
					if(iter_valid(selected[sltedIter])) { // iter_release later on dialog response
						if(!map_valid(m_selectedMsgIter)) {
							m_selectedMsgIter = map_new();
						}
						map_set_arr(m_selectedMsgIter, playerid, selected);
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
				} else if(offset == (AppMsg_GetDetailSize(id) - ui[dtlMessageCount])) {
					task_unblock(Y_HOOKS_BREAK_RETURN_1);
					await AppMsg_RetrieveDetailStats(id, AppMsg_GetSimFromDetail(id), ui[dtlMessageCount]);
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
			case PhoneButtonHangup: {
				AppMsg_DestroyDetailUI(id, playerid);
				AppMsg_UpdateListUI(id, playerid);
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook AppMsg_UIOnMsgSent(msg_id, from_sim, to_sim) {
	if(!map_valid(m_detailUI)) {
		return 1;
	}

	new keyToUpdate[2][Key] = {{-1, -1}, ...};
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
					keyToUpdate[cnt] = key;
					cnt++;
					break;
				}
			}
		}
	}
	for(new i = 0; i != sizeof(keyToUpdate); i++) {
		if(keyToUpdate[i][ItemId] != -1) {
			AppMsg_DestroyDetailUI(keyToUpdate[i][ItemId], keyToUpdate[i][PlayerId]);
			AppMsg_UpdateDetailUI(keyToUpdate[i][ItemId], keyToUpdate[i][PlayerId]);
		}
	}
	return 1;
}