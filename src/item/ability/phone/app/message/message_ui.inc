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

	new Iter:it = AppMsg_GetListIter(id, AppMsg_GetListOffset(id));
	if(iter_valid(it) && iter_release(it)) {
		// header to text: 7, text to next header: 8
		const
			Float:HEADER_TO_TEXT_GAP = 7.00,
			Float:COMPONENT_GAP = HEADER_TO_TEXT_GAP + 8.00; // 8.00 is the total of text size and gap to the header of the next component

		new
			stats[MessageStats],
			component_cnt = 0,
			Float:anc[2],
			Float:safe_min[2],
			Float:safe_max[2],
			Float:first_x,
			Float:first_y;
		IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
		IPUI_GetAnchor(id, playerid, anc[0], anc[1]);

		// calculate how many detail component textdraw will be displayed based on safe screen and gap sizes
		component_cnt = floatround((safe_max[1] - safe_min[1]) / (COMPONENT_GAP), floatround_floor);
		first_x = anc[0] + safe_min[0]; // Align left
		first_y = anc[1] + safe_min[1] + (safe_max[1] - safe_min[1]) / 2.0 - COMPONENT_GAP; // offset a value of COMPONENT_GAP from the center position of the screen because textdraw is displayed from top to bottom without vertical centering
		if(component_cnt % 2 == 0) {
			first_y -= COMPONENT_GAP / 2.0 + (floatround(float(component_cnt - 1) / 2.0, floatround_floor)) * COMPONENT_GAP;
		} else {
			first_y -= (floatround(float(component_cnt - 1) / 2.0, floatround_floor)) * COMPONENT_GAP;
		}

		for(new cnt = 0; iter_inside(it) && cnt != component_cnt; iter_move_next(it)) {
			iter_get_arr(it, stats);

			// TODO: creating textdraw based on stats
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppStore) {

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