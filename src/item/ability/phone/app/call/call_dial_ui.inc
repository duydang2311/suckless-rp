#if defined app_call_dial_ui_included
	#endinput
#endif

#define app_call_dial_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:DialUI {
	PlayerText:uiDialNumberText,
	PlayerText:uiDialExtraText
}

static Map:m_dialUI; // { {Key}: {DialUI} }

stock AppCall_HasDialUIKey(const key[Key]) {
	return (map_valid(m_dialUI) && map_has_arr_key(m_dialUI, key));
}

stock AppCall_UpdateDialUI(id, playerid) {

	return 1;
}

stock AppCall_DestroyDialUI(id, playerid) {

	return 1;
}

#include <YSI_Coding\y_hooks>

HOOK__ AppCall_OnPreDialed(from_sim, to_sim) {
	new saved_key[Key] = {-1, -1};
	if(map_valid(m_dialUI)) {
		for(new Iter:it = map_iter(m_dialUI), key[Key], bool:stop = false; iter_inside(it) && !stop; iter_move_next(it)) {
			iter_get_key_arr(it, key);
			if(IPhone_Has(key[ItemId]) && IGroup_Has(key[ItemId])) {
				new Iter:phone_it = IGroup_GetIter(key[ItemId]);
				if(iter_valid(phone_it) && iter_release(phone_it)) {
					for(; iter_inside(phone_it); iter_move_next(phone_it)) {
						if(iter_get(phone_it) == from_sim) {
							saved_key = key;
							if(AppCall_HasDialUIKey(key)) {
								AppCall_DestroyDialUI(key[ItemId], key[PlayerId]);
								stop = true;
								break;
							}
						}
					}
				}
			}
		}
	}

	if(saved_key[ItemId] == -1 || saved_key[PlayerId] == -1) {
		AppCall_GetContactUIKeyBySim(from_sim, saved_key);
	}
	if(saved_key[ItemId] != -1 && saved_key[PlayerId] != -1) {
		task_unblock(1);
		await task_ticks(1);
		AppCall_UpdateDialUI(saved_key[ItemId], saved_key[PlayerId]);
	}
	return 1;
}

hook AppCall_OnDialed(from_sim, to_sim) {

	return 1;
}
