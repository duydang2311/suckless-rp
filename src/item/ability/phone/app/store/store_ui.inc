#if defined app_store_ui_included
	#endinput
#endif

#define app_store_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:InstallPageStats {
	insApp,
	PlayerText:insAppSprite,
	PlayerText:insAppName,
	PlayerText:insAppDesc,
	PlayerText:insAppButton
}

static
	Map:m_mainPageTd,
	Map:m_installPageStats;

stock AppStore_HasMainPageKey(const key[], size = sizeof(key)) {
	return (map_valid(m_mainPageTd) && map_has_arr_key(m_mainPageTd, key, size));
}

stock AppStore_HasInstallPageKey(const key[], size = sizeof(key)) {
	return (map_valid(m_installPageStats) && map_has_arr_key(m_installPageStats, key, size));
}

stock AppStore_HasMainPage(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	return AppStore_HasMainPageKey(key);
}

stock AppStore_HasInstallPage(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	return AppStore_HasInstallPageKey(key);
}

stock AppStore_DestroyUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppStore_HasMainPageKey(key)) {
		new LinkedList:ll = LinkedList:map_arr_get(m_mainPageTd, key);
		for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
			PlayerTextDrawDestroy(playerid, PlayerText:iter_get(it));
		}
		linked_list_delete(ll);
		map_arr_remove_deep(m_mainPageTd, key);
		if(!map_size(m_mainPageTd)) {
			map_delete(m_mainPageTd);
			m_mainPageTd = Map:-1;
		}
	}
	if(AppStore_HasInstallPageKey(key)) {
		new stats[InstallPageStats];
		map_arr_get_arr(m_installPageStats, key, stats);
		PlayerTextDrawDestroy(playerid, PlayerText:stats[insAppSprite]);
		PlayerTextDrawDestroy(playerid, PlayerText:stats[insAppName]);
		PlayerTextDrawDestroy(playerid, PlayerText:stats[insAppDesc]);
		PlayerTextDrawDestroy(playerid, PlayerText:stats[insAppButton]);
		map_arr_remove(m_installPageStats, key);
		if(!map_size(m_installPageStats)) {
			map_delete(m_installPageStats);
			m_installPageStats = Map:-1;
		}
	}
	return 1;
}

stock AppStore_CreateAppTd(playerid, app, Float:x, Float:y, Float:size, &PlayerText:sprite_td, &PlayerText:name_td) {
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
	PlayerTextDrawColor(playerid, sprite_td, -1);
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

	new index_offset = AppStore_GetIndexOffset(id);
	new Iter:it = AppStore_GetLLAppStatsIter(index_offset);
	if(!iter_valid(it)) {
		return 0;
	}
	iter_release(it);
	new
		LinkedList:ll = linked_list_new(),
		key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_mainPageTd)) {
		m_mainPageTd = map_new();
	}
	map_arr_set(m_mainPageTd, key, ll);
	new
		Float:safe_min[2],
		Float:safe_max[2];
	if(IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1])) {
		new
			Float:size = 12.0,
			row,
			col,
			appSize = AppStore_GetLLAppStatsSize(),
			Float:anc_x,
			Float:anc_y,
			Float:gap_x,
			Float:gap_y,
			Float:first_x,
			Float:first_y;
		IPUI_GetAnchor(id, playerid, anc_x, anc_y);
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
		for(new cnt = 0, PlayerText:sprite_td, PlayerText:name_td, Float:x, Float:y, stats[AppStoreRegisteredStats]; iter_inside(it) && cnt != 6; iter_move_next(it), cnt++) {
			iter_get_arr(it, stats);
			x = first_x + (cnt % col) * gap_x - size / 2.0;
			y = first_y + (floatround(float(cnt) / float(col), floatround_floor)) * gap_y - size / 2.0;
			if(!IPApp_IsExternal(stats[appId]) || AppMenu_IsAppInstalled(id, stats[appId])) {
				AppStore_CreateAppTd(playerid, stats[appId], x, y, size, sprite_td, name_td);
			} else {
				AppStore_CreateAppTd(playerid, stats[appId], x, y, size, sprite_td, name_td);
			}
			PlayerTextDrawShow(playerid, sprite_td);
			PlayerTextDrawShow(playerid, name_td);
			linked_list_add(ll, _:sprite_td);
			linked_list_add(ll, _:name_td);
		}
	}
	return 1;
}

stock AppStore_UpdateInstallPage(id, playerid, app) {
	new
		stats[InstallPageStats],
		Float:scr_min[2],
		Float:scr_max[2];
	stats[insApp] = app;
	if(IPModel_GetSafeScreenOffset(id, scr_min[0], scr_min[1], scr_max[0], scr_max[1])) {
		scr_min[1] += 12.0;
		scr_max[1] -= 20.0;
		new
			Float:anc_x,
			Float:anc_y,
			Float:x,
			Float:y,
			Float:range_y,
			Float:size,
			key[Key],
			sprite[APP_SPRITE_SIZE],
			name[APP_NAME_SIZE];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		IPApp_GetSprite(app, sprite);
		IPApp_GetName(app, name);
		IPUI_GetAnchor(id, playerid, anc_x, anc_y);
		range_y = scr_max[1] - scr_min[1];
		size = range_y * 2.0 / 5.0;
		x = anc_x + scr_min[0] + (scr_max[0] - scr_min[0]) / 2.0 - size / 2.0;
		y = anc_y + scr_min[1] - (size - 9.0) / 2.0;

		stats[insAppSprite] = CreatePlayerTextDraw(playerid, x, y, sprite);
		PlayerTextDrawFont(playerid, stats[insAppSprite], 4);
		PlayerTextDrawLetterSize(playerid, stats[insAppSprite], 0.00000, 0.000000);
		PlayerTextDrawTextSize(playerid, stats[insAppSprite], size, size);
		PlayerTextDrawSetOutline(playerid, stats[insAppSprite], 0);
		PlayerTextDrawSetShadow(playerid, stats[insAppSprite], 0);
		PlayerTextDrawAlignment(playerid, stats[insAppSprite], 2);
		PlayerTextDrawColor(playerid, stats[insAppSprite], -1);
		PlayerTextDrawBackgroundColor(playerid, stats[insAppSprite], 255);
		PlayerTextDrawBoxColor(playerid, stats[insAppSprite], 255);
		PlayerTextDrawUseBox(playerid, stats[insAppSprite], 1);
		PlayerTextDrawSetProportional(playerid, stats[insAppSprite], 1);
		PlayerTextDrawSetSelectable(playerid, stats[insAppSprite], 1);

		stats[insAppName] = CreatePlayerTextDraw(playerid, x + (size / 2.0), y + size, name);
		PlayerTextDrawFont(playerid, stats[insAppName], 3);
		PlayerTextDrawLetterSize(playerid, stats[insAppName], 0.150000, 0.600000);
		PlayerTextDrawTextSize(playerid, stats[insAppName], 400.000000, scr_max[0] - scr_min[0] - 1.0);
		PlayerTextDrawSetOutline(playerid, stats[insAppName], 1);
		PlayerTextDrawSetShadow(playerid, stats[insAppName], 0);
		PlayerTextDrawAlignment(playerid, stats[insAppName], 2);
		PlayerTextDrawColor(playerid, stats[insAppName], -1);
		PlayerTextDrawBackgroundColor(playerid, stats[insAppName], 255);
		PlayerTextDrawBoxColor(playerid, stats[insAppName], 50);
		PlayerTextDrawUseBox(playerid, stats[insAppName], 0);
		PlayerTextDrawSetProportional(playerid, stats[insAppName], 1);
		PlayerTextDrawSetSelectable(playerid, stats[insAppName], 0);

		y += size / 2.0 + 9.0 + range_y / 5.0; // insAppName and insAppSprite occupy a total of 1/5 range_y
		stats[insAppDesc] = CreatePlayerTextDraw(playerid, x + (size / 2.0), y, "App description:...");
		PlayerTextDrawFont(playerid, stats[insAppDesc], 3);
		PlayerTextDrawLetterSize(playerid, stats[insAppDesc], 0.150000, 0.600000);
		PlayerTextDrawTextSize(playerid, stats[insAppDesc], 9.000000, scr_max[0] - scr_min[0] - 1.0);
		PlayerTextDrawSetOutline(playerid, stats[insAppDesc], 1);
		PlayerTextDrawSetShadow(playerid, stats[insAppDesc], 0);
		PlayerTextDrawAlignment(playerid, stats[insAppDesc], 2);
		PlayerTextDrawColor(playerid, stats[insAppDesc], -1);
		PlayerTextDrawBackgroundColor(playerid, stats[insAppDesc], 255);
		PlayerTextDrawBoxColor(playerid, stats[insAppDesc], 50);
		PlayerTextDrawUseBox(playerid, stats[insAppDesc], 1);
		PlayerTextDrawSetProportional(playerid, stats[insAppDesc], 1);
		PlayerTextDrawSetSelectable(playerid, stats[insAppDesc], 0);

		y += range_y * 2.0 / 5.0; // insAppDesc occupies 2/5 of range_y, insAppButton occupies the last 1/5 of range_y
		stats[insAppButton] = CreatePlayerTextDraw(playerid, x + (size / 2.0), y, "Install");
		PlayerTextDrawFont(playerid, stats[insAppButton], 3);
		PlayerTextDrawLetterSize(playerid, stats[insAppButton], 0.150000, 0.600000);
		PlayerTextDrawTextSize(playerid, stats[insAppButton], 4.000000, scr_max[0] - scr_min[0] - 1.0);
		PlayerTextDrawSetOutline(playerid, stats[insAppButton], 1);
		PlayerTextDrawSetShadow(playerid, stats[insAppButton], 0);
		PlayerTextDrawAlignment(playerid, stats[insAppButton], 2);
		PlayerTextDrawColor(playerid, stats[insAppButton], -1);
		PlayerTextDrawBackgroundColor(playerid, stats[insAppButton], 255);
		PlayerTextDrawBoxColor(playerid, stats[insAppButton], 0x00B029FF);
		PlayerTextDrawUseBox(playerid, stats[insAppButton], 1);
		PlayerTextDrawSetProportional(playerid, stats[insAppButton], 1);
		PlayerTextDrawSetSelectable(playerid, stats[insAppButton], 1);

		if(!IPApp_IsExternal(app)) {
			PlayerTextDrawSetSelectable(playerid, stats[insAppButton], 0);
			PlayerTextDrawBoxColor(playerid, stats[insAppButton], 0x6E6E6EFF);
		} else if(AppMenu_IsAppInstalled(id, app)) {
			PlayerTextDrawBoxColor(playerid, stats[insAppButton], 0xBA0404FF);
			PlayerTextDrawSetString(playerid, stats[insAppButton], "Uninstall");
		}

		PlayerTextDrawShow(playerid, PlayerText:stats[insAppSprite]);
		PlayerTextDrawShow(playerid, PlayerText:stats[insAppName]);
		PlayerTextDrawShow(playerid, PlayerText:stats[insAppDesc]);
		PlayerTextDrawShow(playerid, PlayerText:stats[insAppButton]);
		if(!map_valid(m_installPageStats)) {
			m_installPageStats = map_new();
		}
		map_arr_set_arr(m_installPageStats, key, stats);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {
	if(!IPhone_Has(id)) {
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
	new id = IPUI_GetPlayerPhone(playerid);
	if(id == -1) {
		return 1;
	}

	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppStore_HasMainPageKey(key)) {
		new LinkedList:ll = LinkedList:map_arr_get(m_mainPageTd, key);
		for(new Iter:it = linked_list_iter(ll), app = 0; iter_inside(it); iter_move_next(it, 2), app++) {
			if(playertextid == PlayerText:iter_get(it)) {
				new Iter:app_it = AppStore_GetLLAppStatsIter(app);
				if(iter_valid(app_it) && iter_release(app_it)) {
					AppStore_DestroyUI(id, playerid);
					AppStore_UpdateInstallPage(key[ItemId], playerid, iter_get(app_it));
				}
				return Y_HOOKS_BREAK_RETURN_1;
			}
		}
	}
	if(AppStore_HasInstallPageKey(key)) {
		new stats[InstallPageStats];
		map_arr_get_arr(m_installPageStats, key, stats);
		if(playertextid == stats[insAppButton]) {
			new Task:t = Task:-1;
			CallLocalFunction("AppMenu_RequestInstall", "iii", key[ItemId], stats[insApp], ref(_:t));
			if(task_valid(t)) {
				task_unblock(Y_HOOKS_BREAK_RETURN_1);
				new result = await t;
				if(result) {
					IPApp_HideUIForPlayer(id, playerid);
					IPApp_ShowUIForPlayer(id, AppMenu, playerid);
				}
			}
			return Y_HOOKS_BREAK_RETURN_1;
		}
	}
	return 1;
}

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppStore_HasMainPageKey(key) && button == PhoneButtonHangup) {
		IPApp_TerminateStats(id, AppStore);
		AppStore_DestroyUI(id, playerid);
		AppMenu_UpdateAppList(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	} else if(AppStore_HasInstallPageKey(key) && button == PhoneButtonHangup) {
		AppStore_DestroyUI(id, playerid);
		AppStore_UpdateMainPage(id, playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}
