#if defined house_cmds_included
	#endinput
#endif

#define house_cmds_included

CMD:ahouse(playerid, params[]) {
	new
		option[16],
		id,
		value;
	if(sscanf(params, "s[16]I(-1)I(-1)", option, id, value)) {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /ahouse [option] (house id) (value)");
		Player_Msg(playerid, COLOR_FADE2, "> option: create, delete, ownerid, doorid");
		return 1;
	}

	if(!strcmp(option, "create", true)) {
		new string[128];
		task_unblock(1);
		format(string, sizeof(string), "INFO: House ID %d da duoc tao.", task_await(House_Create()));
		Player_Msg(playerid, COLOR_GREEN, string);
		return 1;
	}

	if(id == -1) {
		Player_Msg(playerid, COLOR_RED, "ERROR: House ID khong hop le.");
		return 1;
	}

	if(!strcmp(option, "delete", true)) {
		new string[128];
		if(House_Delete(id)) {
			format(string, sizeof(string), "INFO: House ID %d da duoc xoa.", id);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: House ID %d khong ton tai de xoa.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "ownerid", true)) {
		new string[128];
		if(House_SetOwnerId(id, value)) {
			format(string, sizeof(string), "INFO: Owner ID cua House ID %d da duoc dieu chinh thanh %d.", id, value);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: House ID %d khong ton tai de dieu chinh Owner ID.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else if(!strcmp(option, "doorid", true)) {
		new string[128];
		if(House_SetDoorId(id, value)) {
			format(string, sizeof(string), "INFO: Door ID cua House ID %d da duoc dieu chinh thanh %d.", id, value);
			Player_Msg(playerid, COLOR_GREEN, string);
		} else {
			format(string, sizeof(string), "ERROR: House ID %d khong ton tai de dieu chinh Door ID.", id);
			Player_Msg(playerid, COLOR_RED, string);
		}
	} else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /ahouse [option] (house id) (value)");
		Player_Msg(playerid, COLOR_FADE2, "> option: create, delete, ownerid, doorid");
	}
	return 1;
}