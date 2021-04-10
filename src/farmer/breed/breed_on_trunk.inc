#if defined breed_on_trunk_included
	#endinput
#endif

#define breed_on_trunk_included

stock Breed_PutOnTrunk(id, vehicleid) {
	if(!IsValidVehicle(vehicleid)) {
		return 0;
	}

	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}

	new
		Float:vpos[3],
		Float:rpos[3];
	GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_WHEELSREAR, rpos[0], rpos[1], rpos[2]);
	GetVehiclePos(vehicleid, vpos[0], vpos[1], vpos[2]);
	FCNPC_SetPosition(npcid, rpos[0] + vpos[0], rpos[1] + vpos[1], vpos[2]);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Breed_OnFollowingUpdate(id) {
	if(!Breed_IsInSpawnSession(id)) {

	}
	return 1;
}