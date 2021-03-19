#if defined item_admin_cmds_included
	#endinput
#endif

#define item_admin_cmds_included

CMD:agiveweap(playerid, params[]) {
	extract params -> new targetid, weapon, ammo, Float:damage; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agiveweap [targetid] [weapon] [ammo] [damage]");
		return 1;
	}

	new
		id,
		name[MAX_PLAYER_NAME + 1],
		string[128];
	GetPlayerName(playerid, name);
	if(Weapon_IsGun(Weapon:weapon)) {
		task_unblock(1);
		id = await IGun_Create(Weapon:weapon, damage, ammo);
		Inventory_AddItem(targetid, id);
		format(string, sizeof(string), "INFO: Item Gun (ID: %d) da duoc dua vao Inventory cua '%s'.", id, name);
	} else {
		task_unblock(1);
		id = await IMelee_Create(Weapon:weapon, damage);
		Inventory_AddItem(targetid, id);
		format(string, sizeof(string), "INFO: Item Melee (ID: %d) da duoc dua vao Inventory cua '%s'.", id, name);
	}
	Player_Msg(playerid, COLOR_GREEN, string);
	return 1;
}

CMD:agivebp(playerid, params[]) {
	extract params -> new targetid, model, capacity; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agivebp [targetid] [model] [capacity]");
		return 1;
	}

	task_unblock(1);
	new id = await IBackpack_Create(model, capacity);
	new
		string[128],
		name[MAX_PLAYER_NAME + 1];
	Inventory_AddItem(playerid, id);
	GetPlayerName(playerid, name);
	format(string, sizeof(string), "INFO: Item Backpack (ID: %d) da duoc dua vao Inventory cua '%s'.", id, name);
	Player_Msg(playerid, COLOR_GREEN, string);
	return 1;
}