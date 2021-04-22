#if defined app_menu_ui_included
	#endinput
#endif

#define app_menu_ui_included

#include <YSI_Coding\y_hooks>

hook IPA_OnUIShowAttempt(id, app, playerid) {

	return 1;
}