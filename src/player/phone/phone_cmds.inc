#if defined player_phone_cmds_included
	#endinput
#endif

#define player_phone_cmds_included

CMD:phone(playerid) {
	for(new i = 1, item; i != -1; i--) {
		item = Player_GetHandItemId(playerid, i);
		if(item != -1 && IPhone_Has(item)) {
			if(IGroup_Has(item)) { // this phone may have sim
				new Iter:it = IGroup_GetIter(item);
				if(iter_valid(it) && iter_release(it)) {
					for(new number[SIM_NUMBER_SIZE], id, string[64]; iter_inside(it); iter_move_next(it)) {
						id = iter_get(it);
						if(ISim_Has(id)) {
							ISim_GetNumber(id, number);
							format(string, sizeof(string), "Sim#%d: %s", id, number);
							Player_Msg(playerid, COLOR_YELLOW, string);
						}
					}
				}
			}
			IPhone_ShowUIForPlayer(item, playerid, RandomFloat(200.0, 470.0), RandomFloat(150.0, 230.0));
			task_unblock(1);
			await IPApp_InitStatsAsync(item, AppMenu);
			IPApp_ShowUIForPlayer(item, AppMenu, playerid);
			SelectTextDraw(playerid, 0x115599FF);
			break;
		}
	}
	return 1;
}