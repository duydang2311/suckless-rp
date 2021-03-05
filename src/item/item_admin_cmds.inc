#if defined item_admin_cmds_included
	#endinput
#endif

#define item_admin_cmds_included

CMD:agivegun(playerid, params[]) {
	extract params -> new targetid, weapon, ammo, Float:damage; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agivegun [targetid] [weapon] [ammo] [damage]");
		return 1;
	}

	task_unblock(1);
	new itemid = await ItemGun_Create(Weapon:weapon, damage, ammo);
	Inventory_AddItem(targetid, itemid);
	return 1;
}