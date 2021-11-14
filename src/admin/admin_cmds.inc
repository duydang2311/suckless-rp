static LinkedList:ll_adminVehicles;

static temp_object = 0;

//flags:aveh(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:aveh(playerid, params[]) {
	new
		model,
		put_in,
		primary,
		secondary,
		siren;
	if(sscanf(params, "iI(0)I(1)I(0)I(1)", model, primary, secondary, siren, put_in)) {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /aveh [model] (primary color [1]) (secondary color [0]) (siren 1/[0]) (put in [1]/0)");
		return 1;
	}

	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	if(put_in) {
		GetPlayerPos(playerid, x, y, z);
	}
	GetPlayerFacingAngle(playerid, a);
	new vehicleid = CreateVehicle(model, x, y, z, a, primary, secondary, -1, siren);
	if(vehicleid == INVALID_VEHICLE_ID) {
		Player_Msg(playerid, COLOR_RED, "Server da dat gioi han phuong tien.");
		return 1;
	}

	if(!linked_list_valid(ll_adminVehicles)) {
		ll_adminVehicles = linked_list_new();
	}

	linked_list_add(ll_adminVehicles, vehicleid);
	return 1;
}
alias:aveh("avehicle", "av")

flags:adestroyvehicle(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:adestroyvehicle(playerid, params[]) {
	new vehicleid;
	sscanf(params, "I(0)", vehicleid);
	if(!vehicleid) {
		if(!IsPlayerInAnyVehicle(playerid)) {
			Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /adestroyvehicle (vehicleid)");
			Player_Msg(playerid, COLOR_FADE2, "[!] Ban co the ngoi tren phuong tien va su dung lenh khong can nhap vehicleid.");
			return 1;
		}
		vehicleid = GetPlayerVehicleID(playerid);
	}

	if(linked_list_valid(ll_adminVehicles)) {
		for(new Iter:it = linked_list_iter(ll_adminVehicles); iter_inside(it); iter_move_next(it)) {
			if(iter_get(it) == vehicleid) {
				DestroyVehicle(vehicleid);
				iter_erase(it);
				if(!linked_list_size(ll_adminVehicles)) {
					linked_list_delete(ll_adminVehicles);
					ll_adminVehicles = LinkedList:-1;
				}
				return 1;
			}
		}
	}
	Player_Msg(playerid, COLOR_RED, "Phuong tien ban chon khong do admin tao ra.");
	return 1;
}
alias:adestroyvehicle("adesveh", "adesv", "adestroyveh")

CMD:agetinveh(playerid, params[]) {
	new vehicleid, seatid;
	if(sscanf(params, "ii", vehicleid, seatid)) {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /agetinveh (vehicleid) (seatid)");
		Player_Msg(playerid, COLOR_FADE2, "[!] Ban co the su dung lenh nay de ngoi tren mot phuong tien duoc nhap.");
		return 1;
	}
	
	if(vehicleid) {
		if(seatid > 4 || seatid < 0) {
			Player_Msg(playerid, COLOR_RED, "Seat Index khong hop le, vui long thu lai.");
			return 1;
		}
	} else {
		Player_Msg(playerid, COLOR_RED, "Khong ton tai phuong tien trong may chu.");
		return 1;
	}

	if(Vehicle_GetInSeat(playerid, vehicleid, seatid) == 0) {
		Player_Msg(playerid, COLOR_RED, "Seat Index nay da co nguoi ngoi.");
	} else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Ban da di chuyen den Vehicleid & Seatid da nhap.");
	}
	return 1;
}

flags:askin(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:askin(playerid, params[]) {
	extract params -> new model; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /askin [model]");
		return 1;
	}
	SetPlayerSkin(playerid, model);
	return 1;
}

flags:aobject(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:aobject(playerid, params[]) {
	extract params -> new model; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /aobject [model]");
		return 1;
	}
	if(temp_object != 0) {
		DestroyDynamicObject(temp_object);
	}
	new Float:pos[3];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	temp_object = CreateDynamicObject(model, pos[0], pos[1], pos[2], 0.0, 0.0, 0.0);
	return 1;
}

flags:aaction(ADMIN_FLAGS_1 | ADMIN_FLAGS_2 | ADMIN_FLAGS_3 | ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:aaction(playerid, params[]) {
	extract params -> new action; else {
		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /aaction [special action]");
		return 1;
	}
	SetPlayerSpecialAction(playerid, action);
	return 1;
}
public OnPlayerCommandReceived(playerid, cmd[], params[], flags) {
    if(flags && !(flags & Admin_GetFlags(playerid))) {
		Player_Msg(playerid, COLOR_RED, "Lenh khong ton tai.");
        return 0;
    }

    return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags) {
    if(result == -1) {
        SendClientMessage(playerid, COLOR_RED, "Lenh khong ton tai.");
        return 0;
    }
    return 1;
}

#include <YSI_Coding\y_hooks>