#if defined app_message_ui_included
	#endinput
#endif

#define app_message_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:DetailTdComponent {
	PlayerText:dtlHeader,
	PlayerText:dtlMessage
}

static
	Map:m_listTd,
	Map:m_detailsTd; // { [Key]: LinkedList<DetailTdComponent> }

stock AppMsg_DestroyUI(id, playerid) {
	return 1;
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
		Util_Clamp(component_cnt, (AppMsg_GetListSize(id) - offset), component_cnt);
		component_gap = (safe_max[1] - safe_min[1] - component_cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE)) / component_cnt;
		Util_FloatClamp(component_gap, MINIMUM_COMPONENT_GAP, MINIMUM_COMPONENT_GAP * 2.0);
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
		if(!map_valid(m_detailsTd)) {
			m_detailsTd = map_new();
		}
		if(!map_has_arr_key(m_detailsTd, key)) {
			ll = linked_list_new();
			map_arr_set(m_detailsTd, key, _:ll);
		} else {
			ll = LinkedList:map_arr_get(m_detailsTd, key);
		}
		for(new cnt = 0, component[DetailTdComponent], number[SIM_NUMBER_SIZE]; iter_inside(it) && cnt != component_cnt; iter_move_next(it), cnt++) {
			iter_get_arr(it, stats);
			ISim_GetNumber(stats[msgFromSim], number);

			component[dtlHeader] = CreatePlayerTextDraw(playerid, first_x, first_y + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), number);
			PlayerTextDrawFont(playerid, component[dtlHeader], 3);
			PlayerTextDrawLetterSize(playerid, component[dtlHeader], 0.150000, 0.600000);
			PlayerTextDrawTextSize(playerid, component[dtlHeader], 593.000000, HEADER_SIZE);
			PlayerTextDrawSetOutline(playerid, component[dtlHeader], 1);
			PlayerTextDrawSetShadow(playerid, component[dtlHeader], 0);
			PlayerTextDrawAlignment(playerid, component[dtlHeader], 1);
			PlayerTextDrawColor(playerid, component[dtlHeader], -1);
			PlayerTextDrawBackgroundColor(playerid, component[dtlHeader], 255);
			PlayerTextDrawBoxColor(playerid, component[dtlHeader], 50);
			PlayerTextDrawUseBox(playerid, component[dtlHeader], 0);
			PlayerTextDrawSetProportional(playerid, component[dtlHeader], 1);
			PlayerTextDrawSetSelectable(playerid, component[dtlHeader], 1);

			component[dtlMessage] = CreatePlayerTextDraw(playerid, first_x, first_y + HEADER_TO_TEXT_GAP + HEADER_SIZE + cnt * (HEADER_SIZE + HEADER_TO_TEXT_GAP + TEXT_SIZE + component_gap), stats[msgContent]);
			PlayerTextDrawFont(playerid, component[dtlMessage], 1);
			PlayerTextDrawLetterSize(playerid, component[dtlMessage], 0.119998, 0.479999);
			PlayerTextDrawTextSize(playerid, component[dtlMessage], 594.000000, 82.000000);
			PlayerTextDrawSetOutline(playerid, component[dtlMessage], 1);
			PlayerTextDrawSetShadow(playerid, component[dtlMessage], 0);
			PlayerTextDrawAlignment(playerid, component[dtlMessage], 1);
			PlayerTextDrawColor(playerid, component[dtlMessage], -1111638529);
			PlayerTextDrawBackgroundColor(playerid, component[dtlMessage], 255);
			PlayerTextDrawBoxColor(playerid, component[dtlMessage], 50);
			PlayerTextDrawUseBox(playerid, component[dtlMessage], 0);
			PlayerTextDrawSetProportional(playerid, component[dtlMessage], 1);
			PlayerTextDrawSetSelectable(playerid, component[dtlMessage], 0);

			PlayerTextDrawShow(playerid, component[dtlHeader]);
			PlayerTextDrawShow(playerid, component[dtlMessage]);
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
	AppMsg_DestroyUI(id, playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {

	return 1;
}