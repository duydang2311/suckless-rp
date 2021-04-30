#if defined player_phone_cmds_included
	#endinput
#endif

#define player_phone_cmds_included

CMD:phone(playerid) {
	for(new i = 1, item; i != -1; i--) {
		item = Player_GetHandItemId(playerid, i);
		if(item != -1 && IPhone_Has(item)) {
			IPhone_ShowUIForPlayer(item, playerid, RandomFloat(200.0, 470.0), RandomFloat(150.0, 230.0));
			task_unblock(1);
			await IPApp_InitStatsAsync(item, AppMenu);
			print("init stats async awaited");
			IPApp_ShowUIForPlayer(item, AppMenu, playerid);
			SelectTextDraw(playerid, 0x115599FF);
			break;
		}
	}
	return 1;
}