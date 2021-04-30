#if defined app_menu_ui_included
	#endinput
#endif

#define app_menu_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static Map:m_uiStats; // { {Key}: [PlayerText:value, ...] }

stock AppMenu_HasUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	return (map_valid(m_uiStats) && map_has_arr_key(m_uiStats, key));
}

stock AppMenu_DestroyUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_uiStats) || !map_has_arr_key(m_uiStats, key)) {
		return 0;
	}

	new LinkedList:ll = LinkedList:map_arr_get(m_uiStats, key);
	for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
		PlayerTextDrawDestroy(playerid, PlayerText:iter_get(it));
	}
	map_arr_remove_deep(m_uiStats, key);
	if(!map_size(m_uiStats)) {
		map_delete(m_uiStats);
		m_uiStats = Map:-1;
	}
	return 1;
}


stock AppMenu_CreateAppTd(playerid, app, Float:x, Float:y, Float:size, &PlayerText:sprite_td, &PlayerText:name_td) {
	new sprite[APP_SPRITE_SIZE],
		name[APP_NAME_SIZE];
	IPApp_GetSprite(app, sprite);
	IPApp_GetName(app, name);
	strreplace(name, " ", "~n~");

	sprite_td = CreatePlayerTextDraw(playerid, x, y, sprite);
	PlayerTextDrawFont(playerid, sprite_td, 4);
	PlayerTextDrawLetterSize(playerid, sprite_td, 0.00000, 0.000000);
	PlayerTextDrawTextSize(playerid, sprite_td, size, size);
	PlayerTextDrawSetOutline(playerid, sprite_td, 0);
	PlayerTextDrawSetShadow(playerid, sprite_td, 0);
	PlayerTextDrawAlignment(playerid, sprite_td, 2);
	PlayerTextDrawColor(playerid, sprite_td, -65436);
	PlayerTextDrawBackgroundColor(playerid, sprite_td, 255);
	PlayerTextDrawBoxColor(playerid, sprite_td, 255);
	PlayerTextDrawUseBox(playerid, sprite_td, 1);
	PlayerTextDrawSetProportional(playerid, sprite_td, 1);
	PlayerTextDrawSetSelectable(playerid, sprite_td, 1);

	name_td = CreatePlayerTextDraw(playerid, x + (size / 2.0), y + size, name);
	PlayerTextDrawFont(playerid, name_td, 3);
	PlayerTextDrawLetterSize(playerid, name_td, 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, name_td, 400.000000, 9.000000);
	PlayerTextDrawSetOutline(playerid, name_td, 1);
	PlayerTextDrawSetShadow(playerid, name_td, 0);
	PlayerTextDrawAlignment(playerid, name_td, 2);
	PlayerTextDrawColor(playerid, name_td, -1);
	PlayerTextDrawBackgroundColor(playerid, name_td, 255);
	PlayerTextDrawBoxColor(playerid, name_td, 50);
	PlayerTextDrawUseBox(playerid, name_td, 0);
	PlayerTextDrawSetProportional(playerid, name_td, 1);
	PlayerTextDrawSetSelectable(playerid, name_td, 0);
	return 1;
}

stock AppMenu_UpdateAppList(id, playerid) {
	if(!AppMenu_HasStats(id)) {
		return 1;
	}

	AppMenu_DestroyUI(id, playerid);
	new
		Iter:it = AppMenu_GetAppListIter(id, AppStore_GetIndexOffset(id)),
		LinkedList:ll = linked_list_new(),
		key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_uiStats)) {
		m_uiStats = map_new();
	}
	map_arr_set(m_uiStats, key, ll);
	if(iter_valid(it) && iter_release(it)) {
		new
			Float:safe_min[2],
			Float:safe_max[2];
		if(IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1])) {
			new
				Float:size = 12.0,
				row,
				col,
				appSize = AppMenu_GetAppListSize(id),
				Float:anc_x,
				Float:anc_y,
				Float:gap_x,
				Float:gap_y,
				Float:first_x,
				Float:first_y;
			IPhone_GetUIAnchor(id, playerid, anc_x, anc_y);
			row = floatround(floatsqroot(float(appSize)), floatround_floor);
			col = floatround(float(appSize) / float(row), floatround_floor);
			if(row > 3) {
				row = 3;
			}
			if(col > 4) {
				col = 4;
			}
			safe_min[1] += size; safe_min[0] += size / 2.0;
			safe_max[1] -= size; safe_max[0] -= size / 2.0;
			first_x = anc_x + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0;
			first_y = anc_y + safe_min[1] + (safe_max[1] - safe_min[1]) / 2.0;
			gap_x = (safe_max[0] - safe_min[0]) / (col - 1); // if we have col columns, then we have (col - 1) column gaps
			gap_y = ((safe_max[1] - safe_min[1])) / (row - 1) + 2.0; // same as above for rows
			if(col == 1) {
				gap_x = (safe_max[0] - safe_min[0]);
			}
			if(row == 1) {
				gap_y = ((safe_max[1] - safe_min[1])) + 2.0;
			}
			if(gap_x > size * 2.5) {
				gap_x = size * 2.5;
			}
			if(gap_y > (size * 2.0 + 2.0)) {
				gap_y = (size * 2.0 + 2.0);
			}
			if(col % 2 == 0) {
				first_x -= ((floatround(float(col) / 2.0, floatround_floor)) - 0.5) * gap_x;
			} else {
				first_x -= ((floatround(float(col) / 2.0, floatround_floor))) * gap_x;
			}
			if(row % 2 == 0) {
				first_y -= ((floatround(float(row) / 2.0, floatround_floor)) - 0.5) * gap_y + 2.0;
			} else {
				first_y -= ((floatround(float(row) / 2.0, floatround_floor))) * gap_y + 2.0;
			}
			key[ItemId] = id;
			key[PlayerId] = playerid;
			for(new cnt = 0, app, PlayerText:sprite_td, PlayerText:name_td, Float:x, Float:y; iter_inside(it) && cnt != (row * col); iter_move_next(it), cnt++) {
				// overiding app variable because it's no longer used after comparison with AppMenu
				app = iter_get(it);
				x = first_x + (cnt % col) * gap_x - size / 2.0;
				y = first_y + (floatround(float(cnt) / float(col), floatround_floor)) * gap_y - size / 2.0;
				AppMenu_CreateAppTd(playerid, app, x, y, size, sprite_td, name_td);
				PlayerTextDrawShow(playerid, sprite_td);
				PlayerTextDrawShow(playerid, name_td);
				linked_list_add(ll, _:sprite_td);
				linked_list_add(ll, _:name_td);
			}
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppMenu) {
		print("app menu update app list");
		AppMenu_UpdateAppList(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	AppMenu_DestroyUI(id, playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {
	new id = IPUI_GetPlayerPhone(playerid);
	if(id != -1 && AppMenu_HasUI(id, playerid)) {
		new key[Key];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		new LinkedList:ll = LinkedList:map_arr_get(m_uiStats, key);
		for(new Iter:it = linked_list_iter(ll), index = 0; iter_inside(it); iter_move_next(it, 2), index++) {
			if(PlayerText:iter_get(it) == playertextid) {
				new Iter:app_it = AppMenu_GetAppListIter(id, index);
				if(iter_valid(app_it) && iter_release(app_it)) {
					new app = iter_get(app_it);
					IPApp_InitStats(id, app);
					IPApp_HideUIForPlayer(id, playerid);
					IPApp_ShowUIForPlayer(id, app, playerid);
				}
				return Y_HOOKS_BREAK_RETURN_1;
			}
		}
	}
	return 1;
}