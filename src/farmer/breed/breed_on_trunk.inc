#if defined breed_on_trunk_included
	#endinput
#endif

#define breed_on_trunk_included

static enum _:SelectData {
	sd_breedId,
	sd_vehicleId
}

enum _:BreedTrunkStats {
	breedOnVehicleId,
	breedZAngleTimer
}

static
	Map:m_selectData,
	Map:m_breedTrunkStats;

SubtitleSelect:BreedTrunkSelect(playerid, response, listitem) {
	if(!map_valid(m_selectData) || !map_has_key(m_selectData, playerid)) {
		return 1;
	}

	if(response) {
		if(listitem == 0) {
			new data[SelectData];
			map_get_arr(m_selectData, playerid, data);
			Breed_StopFollowing(data[sd_breedId]);
			Breed_PutOnTrunk(data[sd_breedId], data[sd_vehicleId]);
		}
	}
	map_remove(m_selectData, playerid);
	if(!map_size(m_selectData)) {
		map_delete(m_selectData);
		m_selectData = Map:-1;
	}
	return 1;
}

forward Breed_OnTrunkZAngleUpdate(id);
public Breed_OnTrunkZAngleUpdate(id) {
	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		if(map_valid(m_breedTrunkStats) && map_has_key(m_breedTrunkStats, id)) {
			new stats[BreedTrunkStats];
			map_get_arr(m_breedTrunkStats, id, stats);
			DeletePreciseTimer(stats[breedZAngleTimer]);
		}
		return 1;
	}

	if(map_valid(m_breedTrunkStats) && map_has_key(m_breedTrunkStats, id)) {
		new
			stats[BreedTrunkStats],
			Float:z_angle;
		map_get_arr(m_breedTrunkStats, id, stats);
		GetVehicleZAngle(stats[breedOnVehicleId], z_angle);
		FCNPC_SetAngle(npcid, z_angle);
	}
	return 1;
}

stock Breed_PutOnTrunk(id, vehicleid) {
	if(!IsValidVehicle(vehicleid)) {
		return 0;
	}

	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}

	new
		Float:rpos[3],
		Float:z_angle,
		stats[BreedTrunkStats];
	stats[breedOnVehicleId] = vehicleid;
	stats[breedZAngleTimer] = SetPreciseTimer("Breed_OnTrunkZAngleUpdate", 500, true, "i", id);
	GetVehicleZAngle(vehicleid, z_angle);
	GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_WHEELSREAR, rpos[0], rpos[1], rpos[2]);
	FCNPC_SetSurfingVehicle(npcid, vehicleid);
	FCNPC_SetSurfingOffsets(npcid, 0.0, rpos[1], 0.8);
	FCNPC_SetAngle(npcid, z_angle);
	if(!map_valid(m_breedTrunkStats)) {
		m_breedTrunkStats = map_new();
	}
	map_set_arr(m_breedTrunkStats, id, stats);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Breed_OnFollowingUpdate(id, bool:moving) {
	if(!moving && !Breed_IsInSpawnSession(id)) {
		new playerid = Breed_GetFollowingPlayer(id);
		if(playerid != -1) {
			new
				Float:x,
				Float:y,
				Float:z;
			GetPlayerPos(playerid, x, y, z);
			new vehicleid = Util_GetClosestVehicle(x, y, z, 3.5);
			if(vehicleid != -1) {
				new
					string[128],
					name[32];
				Vehicle_GetName(GetVehicleModel(vehicleid), string);
				Breed_GetAnimalSpeciesName(Breed_GetSpecies(id), name);
				format(string, sizeof(string), "~y~%s (%d)~w~: Ban muon dua toi len phuong tien ~y~%s~w~?", name, id, string);
				SubtitleSelect_Show(playerid, BreedTrunkSelect, string, "Dung roi, len di~n~Khong, dung yen o day");
				if(!map_valid(m_selectData)) {
					m_selectData = map_new();
				}
				new data[SelectData];
				data[sd_breedId] = id;
				data[sd_vehicleId] = vehicleid;
				map_set_arr(m_selectData, playerid, data);
			}
		}
	}
	return 1;
}