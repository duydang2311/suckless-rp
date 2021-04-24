#if defined app_menu_ui_included
	#endinput
#endif

#define app_menu_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static Map:m_uiTd;

stock AppMenu_CreateAppTd(app, playerid, Float:x, Float:y, Float:size, &PlayerText:sprite_td, &PlayerText:name_td) {
	new sprite[APP_SPRITE_SIZE],
		name[APP_NAME_SIZE];
	IPApp_GetSprite(app, sprite);
	IPApp_GetName(app, name);

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
	PlayerTextDrawSetSelectable(playerid, sprite_td, 0);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid, &result) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	if(app == AppMenu) {
		new Iter:it = AppMenu_GetStatsIter(id);
		if(iter_valid(it) && iter_release(it)) {
			new
				Float:scr_min[2],
				Float:scr_max[2];
			if(IPModel_GetScreenOffset(id, scr_min[0], scr_min[1], scr_max[0], scr_max[1])) {
				new
					Float:anc_x,
					Float:anc_y,
					Float:margin_x,
					Float:margin_y,
					row = 3,
					col = 3,
					Float:size,
					key[Key];
				IPhone_GetUIAnchor(id, playerid, anc_x, anc_y);
				size = 15.0;
				margin_x = ((scr_max[0] - scr_min[0]) - size * col) / col;
				margin_y = ((scr_max[1] - scr_min[1]) - size * row) / row;
				key[ItemId] = id;
				key[PlayerId] = playerid;
				for(new cnt = 0, PlayerText:sprite_td, PlayerText:name_td, Float:x, Float:y, LinkedList:ll; iter_inside(it); iter_move_next(it)) {
					// overiding app variable because it's no longer used after comparison with AppMenu
					for(new j = 0; j != 4; j++) {
						app = iter_get(it);
						x = anc_x + scr_min[0] + (margin_x + size) * (cnt % col);
						y = (anc_y + scr_min[1]) + (margin_y + size) * (floatround(float(cnt) / float(col), floatround_floor));
						AppMenu_CreateAppTd(app, playerid, x, y, size, sprite_td, name_td);
						PlayerTextDrawShow(playerid, sprite_td);
						if(!map_valid(m_uiTd)) {
							m_uiTd = map_new();
						}
						if(!map_has_arr_key(m_uiTd, key)) {
							ll = linked_list_new();
							map_arr_set(m_uiTd, key, ll);
						} else {
							ll = LinkedList:map_arr_get(m_uiTd, key);
						}
						linked_list_add(ll, _:sprite_td);
						cnt++;
						printf("menu ui created textdraw for app %d", app);
					}
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}