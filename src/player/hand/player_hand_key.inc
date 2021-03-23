#if defined player_hand_key_included
	#endinput
#endif

#define player_hand_key_included

stock Float:CompressRotation(Float:rotation){
	return (rotation - floatround(rotation/360.0,floatround_floor)*360.0);
}

static get_rotation_for_2point_3d(Float:x,Float:y,Float:z,Float:tx,Float:ty,Float:tz,&Float:rx,&Float:rz){
	new Float:radius = VectorSize(tx-x,ty-y,tz-z);
	if(radius <= 0.0) return 0;
	rx = CompressRotation(-(acos((tz-z)/radius)-90.0));
	rz = CompressRotation(atan2(ty-y,tx-x)-90.0);
	return 1;
}

stock get_point_in_front_2d(Float:x,Float:y,Float:rz,Float:radius,&Float:tx,&Float:ty){
	tx = x - (radius * floatsin(rz,degrees));
	ty = y + (radius * floatcos(rz,degrees));
}

static get_ground_rotation(Float:x,Float:y,Float:size,&Float:rx,&Float:ry){
		new Float:tpx1,Float:tpy1,Float:tpz1,
			Float:tpx2,Float:tpy2,Float:tpz2,
			Float:tmp;
		get_point_in_front_2d(x,y,0.0,size,tpx1,tpy1);
		get_point_in_front_2d(x,y,180.0,size,tpx2,tpy2);
		CA_FindZ_For2DCoord(tpx1,tpy1,tpz1);
		CA_FindZ_For2DCoord(tpx2,tpy2,tpz2);
		get_rotation_for_2point_3d(tpx1,tpy1,tpz1,tpx2,tpy2,tpz2,rx,tmp);

		get_point_in_front_2d(x,y,90.0,size,tpx1,tpy1);
		get_point_in_front_2d(x,y,270.0,size,tpx2,tpy2);
		CA_FindZ_For2DCoord(tpx1,tpy1,tpz1);
		CA_FindZ_For2DCoord(tpx2,tpy2,tpz2);
		get_rotation_for_2point_3d(tpx1,tpy1,tpz1,tpx2,tpy2,tpz2,ry,tmp);
	}

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_NO && !IsPlayerInAnyVehicle(playerid)) {
		for(new i = 1, id; i != -1; i--) {
			id = Player_GetHandItemId(playerid, i);
			if(id != -1) {
				new
					Float:x,
					Float:y,
					Float:z,
					Float:rx,
					Float:ry;
				GetPlayerPos(playerid, x, y, z);
				get_ground_rotation(x, y, -1.0, rx, ry);
				printf("rx %.2f, ry %.2f", rx, ry);
				CA_FindZ_For2DCoord(x, y, z);
				Player_UnequipItem(playerid, i);
				IGround_Update(id, x, y, z, rx, ry, 0.0, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
				return Y_HOOKS_BREAK_RETURN_1;
			}
		}
	}
	return 1;
}