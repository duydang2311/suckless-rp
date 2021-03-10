#if defined door_cmds_included
	#endinput
#endif

#define door_cmds_included

CMD:adoor(playerid, params[]) {
	new
		id,
		option[16],
		value;
	if(sscanf(params, "S[16]()I(-1)I(0)", option, id, value)) {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /adoor [door id] [option] (value || 0)");
		Player_Msg(playerid, COLOR_FADE2, "> option: create, delete, en, ex, enint, exint, envw, exvw, locked, vehicleallowed, sync");
		return 1;
	}

	if(!strcmp(option, "create", true)) {
		new string[128];
		task_unblock(1);
		format(string, sizeof(string), "INFO: Door ID %d da duoc tao.", task_await(Door_Create(0)));
		Player_Msg(playerid, COLOR_GREEN, string);
	}

	if(!Door_Valid(id)) {
		Player_Msg(playerid, COLOR_RED, "ERROR: Door ID khong hop le.");
	}

	if(!strcmp(option, "delete", true)) {
		new string[128];
		Door_Delete(id);
		format(string, sizeof(string), "INFO: Door ID %d da duoc xoa.", id);
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "en", true)) {
		new
			Float:x,
			Float:y,
			Float:z,
			Float:a,
			string[128];
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);
		Door_SetEntrance(id, x, y, z, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
		format(string, sizeof(string), "INFO: Loi vao cua Door ID %d da duoc dieu chinh.", id);
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "ex", true)) {
		new
			Float:x,
			Float:y,
			Float:z,
			Float:a,
			string[128];
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);
		Door_SetExit(id, x, y, z, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
		format(string, sizeof(string), "INFO: Loi ra cua Door ID %d da duoc dieu chinh.", id);
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "enint", true)) {
		new
			string[128];
		Door_SetEnInt(id, value);
		format(string, sizeof(string), "INFO: Entrance Interior cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "exint", true)) {
		new
			string[128];
		Door_SetExInt(id, value);
		format(string, sizeof(string), "INFO: Exit Interior cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "envw", true)) {
		new
			string[128];
		Door_SetEnVW(id, value);
		format(string, sizeof(string), "INFO: Entrance VW cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "exvw", true)) {
		new
			string[128];
		Door_SetExVW(id, value);
		format(string, sizeof(string), "INFO: Exit VW cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "locked", true)) {
		new
			string[128];
		Door_SetLocked(id, value);
		format(string, sizeof(string), "INFO: Locked cua Door ID %d da duoc dieu chinh thanh '%s'.", id, (value ? "Khoa" : "Mo khoa"));
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "vehicleallowed", true)) {
		new
			string[128];
		Door_SetVehicleAllowed(id, value);
		format(string, sizeof(string), "INFO: Locked cua Door ID %d da duoc dieu chinh thanh '%s'.", id, (value ? "Cho phep" : "Khong cho phep"));
		Player_Msg(playerid, COLOR_GREEN, string);
	} else if(!strcmp(option, "sync", true)) {
		new
			string[128];
		Door_SetSync(id, value);
		format(string, sizeof(string), "INFO: Locked cua Door ID %d da duoc dieu chinh thanh '%s'.", id, (value ? "Dong bo" : "Khong dong bo"));
		Player_Msg(playerid, COLOR_GREEN, string);
	} else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /adoor [door id] [option] (value || 0)");
		Player_Msg(playerid, COLOR_FADE2, "> option: create, delete, en, ex, enint, exint, envw, exvw, locked, vehicleallowed, sync");
	}
	return 1;
}