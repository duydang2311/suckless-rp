#if defined app_menu_ui_included
	#endinput
#endif

#define app_menu_ui_included

static const
	ROW = 3,
	COLUMN = 3,
	Float:MARGIN_X = 10.0000,
	Float:MARGIN_Y = 10.0000;

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
				for(new name[APP_NAME_SIZE], sprite[APP_SPRITE_SIZE]; iter_inside(it); iter_move_next(it)) {
					// overiding app variable because it's no longer used after comparison with AppMenu
					app = iter_get(it);
					IPApp_GetName(app, name);
					IPApp_GetSprite(app, sprite);
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}