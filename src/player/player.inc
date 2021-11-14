#if defined player_included
	#endinput
#endif

#define player_included

#include <a_samp>
#include <a_mysql>
#include <streamer>
#include <YSI_Coding\y_inline>
#include <YSI_Data\y_iterate>
#include <PawnPlus>
#include <sscanf2>
#include <bcrypt>
#include <strlib>
#include <easyDialog>
#include <zmessage>
#include <YSI_Extra\y_inline_mysql>
#include <YSI_Coding\y_utils>
#include <chrono>
#include <samp-precise-timers>
#include <skintags>
#include <colandreas>

#include <config/colors>

#include <mysql>

#include "./player_utility.inc"
#include "./player_auth.inc"
#include "./player_stats.inc"
#include "./player_spawn.inc"
#include "./hand/hand.inc"
#include "./phone/phone.inc"
#include "./chat/chat.inc"

#include "./player_death.inc"


#include <YSI_Coding\y_hooks>

hook OnPlayerDisconnect(playerid, reason) {
	for(new i = 0; i != 10; i++) {
		RemovePlayerAttachedObject(playerid, i);
	}
	return 1;
}
