#if defined app_message_ui_included
	#endinput
#endif

#define app_message_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:ListTdComponent {
	PlayerText:lsTdHeader,
	PlayerText:lsTdMessage
}

static
	Map:m_listTd, // { [Key]: LinkedList<ListTdComponent> }
	Map:m_detailsTd;

stock AppMsg_DestroyListUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppMsg_HasListUIKey(key)) {
		return 0;
	}
	new LinkedList:ll = LinkedList:map_arr_get(m_listTd, key);
	if(linked_list_valid(ll)) {
		for(new Iter:it = linked_list_iter(ll), component[ListTdComponent]; iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, component);
			PlayerTextDrawDestroy(playerid, component[lsTdHeader]);
			PlayerTextDrawDestroy(playerid, component[lsTdMessage]);
		}
	}
	map_arr_remove_deep(m_listTd, key);
	return 1;
}

stock AppMsg_DestroyDetailsUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppMsg_HasDetailsUIKey(key)) {
		return 0;
	}
	map_arr_remove_deep(m_detailsTd, key);
	return 1;
}

stock AppMsg_HasListUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_listTd) && map_has_arr_key(m_listTd, key, size));
}

stock AppMsg_HasDetailsUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_detailsTd) && map_has_arr_key(m_detailsTd, key, size));
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
			stats[MessageStats],
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
			LinkedList:ll,
			key[Key];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(!map_valid(m_listTd)) {
			m_listTd = map_new();
		}
		if(!map_has_arr_key(m_listTd, key)) {
			ll = linked_list_new();
			map_arr_set(m_listTd, key, _:ll);
		} else {
			ll = LinkedList:map_arr_get(m_listTd, key);
		}
		for(new cnt = 0, component[ListTdComponent], number[SIM_NUMBER_SIZE]; iter_inside(it) && cnt != component_cnt; iter_move_next(it), cnt++) {
			iter_get_arr(it, stats);
			ISim_GetNumber(stats[msgFromSim], number);

			component[lsTdHeader] = CreatePlayerTextDraw(playerid, first_x, first_y + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), number);
			PlayerTextDrawFont(playerid, component[lsTdHeader], 3);
			PlayerTextDrawLetterSize(playerid, component[lsTdHeader], 0.150000, 0.600000);
			PlayerTextDrawTextSize(playerid, component[lsTdHeader], 593.000000, HEADER_SIZE);
			PlayerTextDrawSetOutline(playerid, component[lsTdHeader], 1);
			PlayerTextDrawSetShadow(playerid, component[lsTdHeader], 0);
			PlayerTextDrawAlignment(playerid, component[lsTdHeader], 1);
			PlayerTextDrawColor(playerid, component[lsTdHeader], -1);
			PlayerTextDrawBackgroundColor(playerid, component[lsTdHeader], 255);
			PlayerTextDrawBoxColor(playerid, component[lsTdHeader], 50);
			PlayerTextDrawUseBox(playerid, component[lsTdHeader], 0);
			PlayerTextDrawSetProportional(playerid, component[lsTdHeader], 1);
			PlayerTextDrawSetSelectable(playerid, component[lsTdHeader], 1);

			component[lsTdMessage] = CreatePlayerTextDraw(playerid, first_x, first_y + HEADER_TO_TEXT_GAP + HEADER_SIZE + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), stats[msgContent]);
			PlayerTextDrawFont(playerid, component[lsTdMessage], 1);
			PlayerTextDrawLetterSize(playerid, component[lsTdMessage], 0.119998, 0.479999);
			PlayerTextDrawTextSize(playerid, component[lsTdMessage], 594.000000, 82.000000);
			PlayerTextDrawSetOutline(playerid, component[lsTdMessage], 1);
			PlayerTextDrawSetShadow(playerid, component[lsTdMessage], 0);
			PlayerTextDrawAlignment(playerid, component[lsTdMessage], 1);
			PlayerTextDrawColor(playerid, component[lsTdMessage], -1111638529);
			PlayerTextDrawBackgroundColor(playerid, component[lsTdMessage], 255);
			PlayerTextDrawBoxColor(playerid, component[lsTdMessage], 50);
			PlayerTextDrawUseBox(playerid, component[lsTdMessage], 0);
			PlayerTextDrawSetProportional(playerid, component[lsTdMessage], 1);
			PlayerTextDrawSetSelectable(playerid, component[lsTdMessage], 0);

			PlayerTextDrawShow(playerid, component[lsTdHeader]);
			PlayerTextDrawShow(playerid, component[lsTdMessage]);
			linked_list_add_arr(ll, component);
		}
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
	AppMsg_DestroyDetailsUI(id, playerid);
	AppMsg_DestroyListUI(id, playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {

	return 1;
}

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppMsg_HasDetailsUIKey(key)) {

		return Y_HOOKS_BREAK_RETURN_1;
	} else if(AppMsg_HasListUIKey(key)) {
		switch(button) {
			case PhoneButtonDown: {
				new offset = AppMsg_GetListOffset(id);
				if(offset != (AppMsg_GetListSize(id) - 1)) {
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