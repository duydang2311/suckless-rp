#if defined app_store_ui_included
	#endinput
#endif

#define app_store_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:InstallPageTextDraw {
	PlayerText:tdAppSprite,
	PlayerText:tdAppName,
	PlayerText:tdAppDesc,
	PlayerText:tdAppButton
}

static
	Map:m_mainPageTd,
	Map:m_installPageTd,
	Map:m_uiItemId;

stock AppStore_HasUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	return 1;
}

stock AppStore_HasUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_mainPageTd) && map_has_arr_key(m_mainPageTd, key, size));
}

stock AppStore_DestroyUI(id, playerid) {
	if(map_valid(m_uiItemId) && map_has_key(m_uiItemId, playerid)) {
		map_remove(m_uiItemId, playerid);
		if(!map_size(m_uiItemId)) {
			map_delete(m_uiItemId);
			m_uiItemId = Map:-1;
		}
	}

	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(map_valid(m_mainPageTd) && map_has_arr_key(m_mainPageTd, key)) {
		new LinkedList:ll = LinkedList:map_arr_get(m_mainPageTd, key);
		for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
			PlayerTextDrawDestroy(playerid, PlayerText:iter_get(it));
		}
		map_arr_remove_deep(m_mainPageTd, key);
		if(!map_size(m_mainPageTd)) {
			map_delete(m_mainPageTd);
			m_mainPageTd = Map:-1;
		}
	}
	if(map_valid(m_installPageTd) && map_has_arr_key(m_installPageTd, key)) {
		new insTd[InstallPageTextDraw];
		map_arr_get_arr(m_installPageTd, key, insTd);
		for(new i = 0; i != sizeof(insTd); i++) {
			PlayerTextDrawDestroy(playerid, PlayerText:insTd[i]);
		}
		map_arr_remove(m_installPageTd, key);
		if(!map_size(m_installPageTd)) {
			map_delete(m_installPageTd);
			m_installPageTd = Map:-1;
		}
	}
	return 1;
}

stock AppStore_CreateAppTd(playerid, app, Float:x, Float:y, Float:size, &PlayerText:sprite_td, &PlayerText:name_td, color = 0x000000FF) {
	new
		sprite[APP_SPRITE_SIZE],
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
	PlayerTextDrawColor(playerid, sprite_td, color);
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

stock AppStore_UpdateMainPage(id, playerid) {
	if(!AppStore_HasStats(id)) {
		return 0;
	}

	AppStore_DestroyUI(id, playerid);

	new index_offset = AppStore_GetIndexOffset(id);
	new
		Iter:it = AppStore_GetAppListIter(id, index_offset),
		LinkedList:ll = linked_list_new(),
		key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_mainPageTd)) {
		m_mainPageTd = map_new();
	}
	map_arr_set(m_mainPageTd, key, ll);
	const
		row = 3,
		col = 3,
		Float:size = 15.0;
	new
		Float:scr_min[2],
		Float:scr_max[2],
		bool:installed[App] = {false, ...};
	for(; iter_inside(it); iter_move_next(it)) {
		installed[iter_get(it)] = true;
	}
	if(IPModel_GetScreenOffset(id, scr_min[0], scr_min[1], scr_max[0], scr_max[1])) {
		scr_min[1] += 2.0;
		scr_max[1] -= 16.0; // decrease max y to leave some space for name td
		new
			Float:anc_x,
			Float:anc_y,
			Float:gap_x,
			Float:gap_y;
		IPhone_GetUIAnchor(id, playerid, anc_x, anc_y);
		gap_x = ((scr_max[0] - scr_min[0]) - size * col) / (col - 1); // if we have col columns, then we have (col - 1) gaps between columns
		gap_y = ((scr_max[1] - scr_min[1]) - (size + 2.0) * row) / (row - 1); // same as above for rows
		key[ItemId] = id;
		key[PlayerId] = playerid;
		for(new i = index_offset, cnt = 0, PlayerText:sprite_td, PlayerText:name_td, Float:x, Float:y; i != 6 && i != sizeof(installed); i++, cnt++) {
			x = anc_x + scr_min[0] + (gap_x + size) * (cnt % col);
			y = (anc_y + scr_min[1]) + (gap_y + size) * (floatround(float(cnt) / float(col), floatround_floor));
			if(installed[i]) {
				AppStore_CreateAppTd(playerid, i, x, y, size, sprite_td, name_td, .color = 0x000000FF);
			} else {
				AppStore_CreateAppTd(playerid, i, x, y, size, sprite_td, name_td, .color = 0x00000033);
			}
			PlayerTextDrawShow(playerid, sprite_td);
			PlayerTextDrawShow(playerid, name_td);
			linked_list_add(ll, _:sprite_td);
			linked_list_add(ll, _:name_td);
		}
		if(!map_valid(m_uiItemId)) {
			m_uiItemId = map_new();
		}
		map_set(m_uiItemId, playerid, id);
	}
	return 1;
}

stock AppStore_UpdateInstallPage(id, playerid, app) {
	AppStore_DestroyUI(id, playerid);

	new
		insTd[InstallPageTextDraw],
		Float:scr_min[2],
		Float:scr_max[2];
	if(IPModel_GetScreenOffset(id, scr_min[0], scr_min[1], scr_max[0], scr_max[1])) {
		scr_min[1] += 20.0;
		scr_max[1] -= 20.0; // decrease max y to leave some space for name td
		new
			Float:anc_x,
			Float:anc_y,
			Float:x,
			Float:y,
			Float:range_x,
			Float:range_y,
			Float:size,
			key[Key];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		IPhone_GetUIAnchor(id, playerid, anc_x, anc_y);
		range_x = scr_max[0] - scr_min[0];
		range_y = scr_max[1] - scr_min[1];
		size = range_y / 3.0;
		x = anc_x + scr_min[0] + (scr_max[0] - scr_min[0]) / 2.0 - size / 2.0;
		y = anc_y + scr_min[1] - (size - 9.0) / 2.0;
		AppStore_CreateAppTd(playerid, app, x, y, size, insTd[tdAppSprite], insTd[tdAppName]);
		y += size + 9.0 + range_y / 3.0;
		insTd[tdAppDesc] = CreatePlayerTextDraw(playerid, x + (size / 2.0), y, "App description:...");
		PlayerTextDrawFont(playerid, insTd[tdAppDesc], 3);
		PlayerTextDrawLetterSize(playerid, insTd[tdAppDesc], 0.150000, 0.600000);
		PlayerTextDrawTextSize(playerid, insTd[tdAppDesc], 400.000000, 9.000000);
		PlayerTextDrawSetOutline(playerid, insTd[tdAppDesc], 1);
		PlayerTextDrawSetShadow(playerid, insTd[tdAppDesc], 0);
		PlayerTextDrawAlignment(playerid, insTd[tdAppDesc], 2);
		PlayerTextDrawColor(playerid, insTd[tdAppDesc], -1);
		PlayerTextDrawBackgroundColor(playerid, insTd[tdAppDesc], 255);
		PlayerTextDrawBoxColor(playerid, insTd[tdAppDesc], 50);
		PlayerTextDrawUseBox(playerid, insTd[tdAppDesc], 0);
		PlayerTextDrawSetProportional(playerid, insTd[tdAppDesc], 1);
		PlayerTextDrawSetSelectable(playerid, insTd[tdAppDesc], 0);
		y += range_y / 3.0;
		insTd[tdAppButton] = CreatePlayerTextDraw(playerid, x + (size / 2.0), y, "Install");
		PlayerTextDrawFont(playerid, insTd[tdAppButton], 3);
		PlayerTextDrawLetterSize(playerid, insTd[tdAppButton], 0.150000, 0.600000);
		PlayerTextDrawTextSize(playerid, insTd[tdAppButton], 400.000000, 9.000000);
		PlayerTextDrawSetOutline(playerid, insTd[tdAppButton], 1);
		PlayerTextDrawSetShadow(playerid, insTd[tdAppButton], 0);
		PlayerTextDrawAlignment(playerid, insTd[tdAppButton], 2);
		PlayerTextDrawColor(playerid, insTd[tdAppButton], -1);
		PlayerTextDrawBackgroundColor(playerid, insTd[tdAppButton], 255);
		PlayerTextDrawBoxColor(playerid, insTd[tdAppButton], 0xFFFFFFFF);
		PlayerTextDrawUseBox(playerid, insTd[tdAppButton], 1);
		PlayerTextDrawSetProportional(playerid, insTd[tdAppButton], 1);
		PlayerTextDrawSetSelectable(playerid, insTd[tdAppButton], 1);

		for(new i = 0; i != sizeof(insTd); i++) {
			PlayerTextDrawShow(playerid, PlayerText:insTd[i]);
		}
		if(!map_valid(m_installPageTd)) {
			m_installPageTd = map_new();
		}
		map_arr_set_arr(m_installPageTd, key, insTd);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppStore) {
		AppStore_UpdateMainPage(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	AppStore_DestroyUI(id, playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {
	if(!map_valid(m_uiItemId) || !map_has_key(m_uiItemId, playerid)) {
		return 1;
	}

	new key[Key];
	key[ItemId] = map_get(m_uiItemId, playerid);
	key[PlayerId] = playerid;
	if(AppStore_HasUIKey(key)) {
		new LinkedList:ll = LinkedList:map_arr_get(m_mainPageTd, key);
		for(new Iter:it = linked_list_iter(ll), app = 0; iter_inside(it); iter_move_next(it, 2), app++) {
			if(playertextid == PlayerText:iter_get(it)) {
				AppStore_UpdateInstallPage(key[ItemId], playerid, app);
				return Y_HOOKS_BREAK_RETURN_1;
			}
		}
	}
	return 1;
}