#if defined item_admin_cmds_included
	#endinput
#endif

#define item_admin_cmds_included

CMD:agiveweap(playerid, params[]) {
	extract params -> new targetid, weapon, ammo, Float:damage; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agiveweap [targetid] [weapon] [ammo] [damage]");
		return 1;
	}

	if(Weapon_IsGun(Weapon:weapon)) {
		task_unblock(1);
		new itemid = await ItemGun_Create(Weapon:weapon, damage, ammo);
		Inventory_AddItem(targetid, itemid);
	} else {
		task_unblock(1);
		new itemid = await ItemMelee_Create(Weapon:weapon, damage);
		Inventory_AddItem(targetid, itemid);
	}
	return 1;
}