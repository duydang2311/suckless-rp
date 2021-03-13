#if defined door_cmds_included
	#endinput
#endif

#define door_cmds_included

CMD:adoor(playerid, params[]) {
	new
		id,
		option[16],
		value;
	if(sscanf(params, "s[16]I(-1)I(0)", option, id, value)) {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /adoor [option] [door id] (value || 0)");
		Player_Msg(playerid, COLOR_FADE2, "> option: create, delete, en, ex, enint, exint, envw, exvw, locked, vehicleallowed, sync, pickupmodel, type, houseid.");
		return 1;
	}

	if(!strcmp(option, "create", true)) {
		new string[128];
		task_unblock(1);
		format(string, sizeof(string), "INFO: Door ID %d da duoc tao.", task_await(Door_Create(DoorType:DOOR_TYPE_NONE)));
		Player_Msg(playerid, COLOR_GREEN, string);
		return 1;
	}

	if(!Door_Valid(id)) {
		Player_Msg(playerid, COLOR_RED, "ERROR: Door ID khong hop le.");
		return 1;
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
		if(Door_SetEntrance(id, x, y, z, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid))) {
			format(string, sizeof(string), "INFO: Loi vao cua Door ID %d da duoc dieu chinh.", id);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh loi vao.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "ex", true)) {
		new
			Float:x,
			Float:y,
			Float:z,
			Float:a,
			string[128];
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);
		if(Door_SetExit(id, x, y, z, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid))) {
			format(string, sizeof(string), "INFO: Loi ra cua Door ID %d da duoc dieu chinh.", id);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh loi ra.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "enint", true)) {
		new
			string[128];
		if(Door_SetEnInt(id, value)) {
			format(string, sizeof(string), "INFO: Entrance Interior cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh Entrance Interior.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "exint", true)) {
		new
			string[128];
		if(Door_SetExInt(id, value)) {
			format(string, sizeof(string), "INFO: Exit Interior cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh Exit Interior.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "envw", true)) {
		new
			string[128];
		if(Door_SetEnVW(id, value)) {
			format(string, sizeof(string), "INFO: Entrance VW cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh Entrance VW.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "exvw", true)) {
		new
			string[128];
		if(Door_SetExVW(id, value)) {
			format(string, sizeof(string), "INFO: Exit VW cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh Exit VW.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "locked", true)) {
		new
			string[128];
		if(Door_SetLocked(id, bool:value)) {
			format(string, sizeof(string), "INFO: Locked cua Door ID %d da duoc dieu chinh thanh '%s'.", id, (value ? "Khoa" : "Mo khoa"));
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh trang thai Locked.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "vehicleallowed", true)) {
		new
			string[128];
		if(Door_SetVehicleAllowed(id, bool:value)) {
			format(string, sizeof(string), "INFO: Locked cua Door ID %d da duoc dieu chinh thanh '%s'.", id, (value ? "Cho phep" : "Khong cho phep"));
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh trang thai Vehicle Allowed.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "sync", true)) {
		new
			string[128];
		if(Door_SetStreamingSync(id, bool:value)) {
			format(string, sizeof(string), "INFO: Locked cua Door ID %d da duoc dieu chinh thanh '%s'.", id, (value ? "Dong bo" : "Khong dong bo"));
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh trang thai Streaming Sync.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "pickupmodel", true)) {
		new
			string[128];
		if(Door_SetEnPickupModel(id, value)) {
			format(string, sizeof(string), "INFO: Pickup Model cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh Pickup Model.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "type", true)) {
		new
			string[128];
		if(Door_SetType(id, DoorType:value)) {
			Door_GetTypeName(DoorType:value, string, sizeof(string));
			format(string, sizeof(string), "INFO: Type cua Door ID %d da duoc dieu chinh thanh '%s'.", id, string);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai de dieu chinh Type.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "houseid", true)) {
		new string[128];
		if(Door_SetHouseId(id, value)) {
			format(string, sizeof(string), "INFO: House ID cua Door ID %d da duoc dieu chinh thanh %d.", id, value);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: Door ID %d khong ton tai/khong thuoc type 'House' de dieu chinh House ID.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /adoor [option] [door id] (value || 0)");
		Player_Msg(playerid, COLOR_FADE2, "> option: create, delete, en, ex, enint, exint, envw, exvw, locked, vehicleallowed, sync, pickupmodel, type, houseid.");
	}
	return 1;
}

CMD:enter(playerid) {
	new id = Door_GetPlayerEntranceDoorId(playerid);
	if(id == -1) {
		Player_Msg(playerid, COLOR_YELLOW, "Ban khong o gan canh cua nao de vao.");
		return 1;
	}

	Door_StreamPlayerToExit(id, playerid);
	return 1;
}

CMD:exit(playerid) {
	new id = Door_GetPlayerExitDoorId(playerid);
	if(id == -1) {
		Player_Msg(playerid, COLOR_YELLOW, "Ban khong o gan canh cua nao de vao.");
		return 1;
	}

	Door_StreamPlayerToEntrance(id, playerid);
	return 1;
}