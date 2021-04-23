#if defined app_menu_ui_included
	#endinput
#endif

#define app_menu_ui_included

static const
	Float:MARGIN_X = 10.0000,
	Float:MARGIN_Y = 10.0000;

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
					row = 3,
					col = 3,
					Float:size;
				IPhone_GetUIAnchor(id, playerid, anc_x, anc_y);
				size = ((scr_max[0] - scr_min[0]) - MARGIN_X * col) / col;
				for(new cnt = 0, PlayerText:sprite_td, PlayerText:name_td, Float:x, Float:y; iter_inside(it); iter_move_next(it)) {
					// overiding app variable because it's no longer used after comparison with AppMenu
					app = iter_get(it);
					x = anc_x + scr_min[0] + MARGIN_X * (cnt % col);
					y = (anc_y + scr_min[1]) + MARGIN_Y * (floatround(float(cnt) / float(col), floatround_floor));
					AppMenu_CreateAppTd(app, playerid, x, y, size, sprite_td, name_td);
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}