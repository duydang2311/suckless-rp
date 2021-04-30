#if defined item_included
	#endinput
#endif

#define item_included

#include <a_samp>
#include <a_mysql>
#include <streamer>
#include <YSI_Coding\y_inline>
#include <PawnPlus>
#include <Pawn.CMD>
#include <sscanf2>
#include <YSI_Extra\y_inline_mysql>
#include <samp-precise-timers>

#include <config\colors>
#include <config\cmd_flags>

#include <util>

#include <mysql>
#include <session>

#include "./item_stats.inc"
#include "./item_group.inc"

#include "./ability/gun.inc"
#include "./ability/melee.inc"
#include "./ability/backpack.inc"
#include "./ability/whisk.inc"
#include "./ability/sim/sim.inc"
#include "./ability/phone/phone.inc"

#include "./item_ground.inc"
#include "./item_cmds.inc"
#include "./item_carry_attach.inc"