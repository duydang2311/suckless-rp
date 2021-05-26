#if defined item_phone_included
	#endinput
#endif

#define item_phone_included

#include ".\ability\phone\model\model.inc"
#include ".\ability\phone\system\system.inc"
#include ".\ability\phone\system\system_ui.inc"
#include ".\ability\phone\app\app.inc"
#include ".\ability\phone\phone_stats.inc"
#include ".\ability\phone\phone_ui.inc"

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	return 1;
}