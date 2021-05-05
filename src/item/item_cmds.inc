#if defined item_admin_cmds_included
	#endinput
#endif

#define item_admin_cmds_included

flags:agiveweap(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:agiveweap(playerid, params[]) {
	extract params -> new targetid, weapon, ammo, Float:damage; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agiveweap [targetid] [weapon] [ammo] [damage]");
		return 1;
	}

	if(!Player_IsStatsLoaded(targetid)) {
		Player_Msg(playerid, COLOR_FADE2, "Nguoi choi chua ket noi.");
		return 1;
	}

	new
		id,
		name[MAX_PLAYER_NAME + 1],
		string[128];
	GetPlayerName(targetid, name);
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

flags:agivebp(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:agivebp(playerid, params[]) {
	extract params -> new targetid, model, capacity; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agivebp [targetid] [model] [capacity]");
		return 1;
	}

	if(!Player_IsStatsLoaded(targetid)) {
		Player_Msg(playerid, COLOR_FADE2, "Nguoi choi chua ket noi.");
		return 1;
	}

	task_unblock(1);
	new id = await IBackpack_Create(model, capacity);
	new
		string[128],
		name[MAX_PLAYER_NAME + 1];
	Inventory_AddItem(targetid, id);
	GetPlayerName(targetid, name);
	format(string, sizeof(string), "INFO: Item Backpack (ID: %d) da duoc dua vao Inventory cua '%s'.", id, name);
	Player_Msg(playerid, COLOR_GREEN, string);
	return 1;
}

flags:agivewhisk(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:agivewhisk(playerid, params[]) {
	extract params -> new targetid; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agivewhisk [playerid]");
		return 1;
	}

	if(!Player_IsStatsLoaded(targetid)) {
		Player_Msg(playerid, COLOR_FADE2, "Nguoi choi chua ket noi.");
		return 1;
	}

	if(Player_GetHandItemId(targetid, 0) != -1 || Player_GetHandItemId(targetid, 1) != -1 || Player_GetCarryingItem(targetid) != -1) {
		Player_Msg(playerid, COLOR_YELLOW, "Nguoi choi khong duoc cam vat pham nao tren tay.");
		return 1;
	}

	new slot = Player_GetFreeAttachmentSlot(targetid, true);
	if(slot == -1) {
		Player_Msg(playerid, COLOR_FADE2, "Nguoi choi khong con slot gan vat pham.");
		return 0;
	}
	task_unblock(1);
	new id = await IWhisk_Create();
	if(id == -1) {
		Player_Msg(playerid, COLOR_RED, "ERROR: Tao item Whisk that bai.");
		return 1;
	}
	new
		string[128],
		name[MAX_PLAYER_NAME + 1];
	SetPreciseTimer("Player_CarryItem", 0, false, "iii", targetid, id, slot);
	GetPlayerName(targetid, name);
	format(string, sizeof(string), "INFO: Item Whisk (ID %d) da duoc trao cho '%s' o trang thai 'Carry'.", id, name);
	Player_Msg(playerid, COLOR_GREEN, string);
	return 1;
}

flags:agivephone(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
cmd:agivephone(playerid, params[]) {
	extract params -> new targetid, model; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agivephone [targetid] [model]");
		return 1;
	}

	if(!Player_IsStatsLoaded(targetid)) {
		Player_Msg(playerid, COLOR_FADE2, "Nguoi choi chua ket noi.");
		return 1;
	}

	task_unblock(1);
	new id = await IPhone_Create(model);
	new
		string[128],
		name[MAX_PLAYER_NAME + 1];
	Inventory_AddItem(targetid, id);
	GetPlayerName(targetid, name);
	format(string, sizeof(string), "INFO: Item Phone (ID: %d) da duoc dua vao Inventory cua '%s'.", id, name);
	Player_Msg(playerid, COLOR_GREEN, string);
	return 1;
}

flags:agivesim(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
cmd:agivesim(playerid, params[]) {
	extract params -> new targetid; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agivesim [targetid]");
		return 1;
	}

	if(!Player_IsStatsLoaded(targetid)) {
		Player_Msg(playerid, COLOR_FADE2, "Nguoi choi chua ket noi.");
		return 1;
	}

	task_unblock(1);
	new id = await ISim_Create();
	new
		string[128],
		name[MAX_PLAYER_NAME + 1];
	Inventory_AddItem(targetid, id);
	GetPlayerName(targetid, name);
	format(string, sizeof(string), "INFO: Item Sim (ID: %d) da duoc dua vao Inventory cua '%s'.", id, name);
	Player_Msg(playerid, COLOR_GREEN, string);
	return 1;
}
