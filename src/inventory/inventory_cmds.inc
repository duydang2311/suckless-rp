#if defined inventory_cmds_included
	#endinput
#endif

#define inventory_cmds_included

CMD:inv(playerid) {
	new
		string[128] = "You have: ",
		Iter:it = Inventory_GetFirstItemIter(playerid);
	if(iter_valid(it)) {
		for(; iter_inside(it); iter_move_next(it)) {
			format(string, sizeof(string), "%s%d, ", string, iter_get(it));
		}
		iter_release(it);
	}
	string[strlen(string) - 1] = EOS;
	Player_Msg(playerid, COLOR_LIGHTYELLOW, string);
	return 1;
}

CMD:lefthand(playerid, params[]) {
	Player_EquipItem(playerid, false, strval(params));
	return 1;
}

CMD:righthand(playerid, params[]) {
	Player_EquipItem(playerid, true, strval(params));
	return 1;
}