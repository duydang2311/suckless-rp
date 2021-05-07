#if defined app_message_ui_included
	#endinput
#endif

#define app_message_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static
	Map:m_msgListPageTd,
	Map:m_msgDetailsPageTd;

stock AppMsg_UpdateMsgList(id, playerid) {
	if(!AppMsg_HasListStats(id)) { // TODO: print 'Sim not found' on the phone screen.
		return 0;
	}

	new Iter:it = AppMsg_GetListIter(id, AppMsg_GetListOffset(id));
	if(iter_valid(it) && iter_release(it)) {
		new stats[MessageListStats];
		for(; iter_inside(it); iter_move_next(it)) {
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