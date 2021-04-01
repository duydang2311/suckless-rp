#if defined breed_spawn_included
	#endinput
#endif

#define breed_spawn_included

static const Float:BreedSpawnPos[][4] = {
	{-2.0429, 74.7125, 3.1172, 250.0000},
	{-9.7621, 54.7767, 3.1172, 250.0000}
};

static
	Map:m_inSpawnSession,
	Map:m_distCheckTimer;

/*
	m_inSpawnSession's value meaning:
	-1: just get spawned and in standby
	playerid: following playerid
*/

forward Breed_Spawn(id);
public Breed_Spawn(id) {
	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}

	new rand = random(2);
	FCNPC_Spawn(npcid, Breed_GetAnimalSpeciesModel(Breed_GetSpecies(id)), BreedSpawnPos[rand][0], BreedSpawnPos[rand][1], BreedSpawnPos[rand][2]);
	FCNPC_SetAngle(npcid, BreedSpawnPos[rand][3]);
	if(!map_valid(m_inSpawnSession)) {
		m_inSpawnSession = map_new();
	}
	map_set(m_inSpawnSession, id, -1);
	return 1;
}

forward Breed_DistanceCheckTimer(id);
public Breed_DistanceCheckTimer(id) {
	new npcid = Breed_GetNPCId(id);
	if(npcid == -1 || !map_valid(m_inSpawnSession) || !map_has_key(m_inSpawnSession, id)) {
		if(map_valid(m_distCheckTimer) && map_has_key(m_distCheckTimer, id)) {
			DeletePreciseTimer(map_get(m_distCheckTimer, id));
			map_remove(m_distCheckTimer, id);
			if(!map_size(m_distCheckTimer)) {
				map_delete(m_distCheckTimer);
				m_distCheckTimer = Map:-1;
			}
		}
		return 0;
	}

	new
		followTarget = map_get(m_inSpawnSession, id),
		Float:x,
		Float:y,
		Float:z;
	FCNPC_GetPosition(npcid, x, y, z);
	if(GetPlayerDistanceFromPoint(followTarget, x, y, z) >= 1.5) {
		new Float:angle;
		GetPlayerPos(followTarget, x, y, z);
		GetPlayerFacingAngle(followTarget, angle);
		Util_GetPointInFront2D(x, y, angle, -1.0, x, y);
		FCNPC_GoTo(npcid, x, y, z, .speed = FCNPC_MOVE_SPEED_WALK);
		SetPlayerCheckpoint(followTarget, x, y, z, 2.0);
		if(map_valid(m_distCheckTimer) && map_has_key(m_distCheckTimer, id)) {
			DeletePreciseTimer(map_get(m_distCheckTimer, id));
			map_remove(m_distCheckTimer, id);
			if(!map_size(m_distCheckTimer)) {
				map_delete(m_distCheckTimer);
				m_distCheckTimer = Map:-1;
			}
		}
	}
	return 1;
}

stock Breed_IsInSpawnSession(id) {
	return (map_valid(m_inSpawnSession) && map_has_key(m_inSpawnSession, id));
}

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT) {
		if(!map_valid(m_inSpawnSession)) {
			return 1;
		}

		new
			id = Player_GetId(playerid),
			Float:x,
			Float:y,
			Float:z;
		for(new Iter:it = map_iter(m_inSpawnSession); iter_inside(it); iter_move_next(it)) {
			if(iter_get(it) != -1) {
				continue;
			}
			if(Breed_GetBreederId(iter_get_key(it)) == id) {
				new
					breed_id = iter_get_key(it),
					npcid = Breed_GetNPCId(breed_id);
				if(npcid == -1) {
					continue;
				}
				FCNPC_GetPosition(npcid, x, y, z);
				if(GetPlayerDistanceFromPoint(playerid, x, y, z) <= 2.5) {
					new
						string[128],
						name[32];
					iter_set(it, playerid);
					GetPlayerPos(playerid, x, y, z);
					FCNPC_GoTo(npcid, x, y, z, .speed = FCNPC_MOVE_SPEED_WALK);
					Breed_GetAnimalSpeciesName(Breed_GetSpecies(breed_id), name);
					format(string, sizeof(string), "~y~%s (%d) ~w~dang di theo ban, hay dan vat nuoi vao khu dat chan nuoi.", name, breed_id);
					SubtitleText_Show(playerid, string);
					return Y_HOOKS_BREAK_RETURN_1;
				}
			}
		}
	}
	return 1;
}

hook FCNPC_OnReachDest(npcid) {
	new id = Breed_GetIdByNPCId(npcid);
	if(id != -1 && map_valid(m_inSpawnSession) && map_has_key(m_inSpawnSession, id)) {
		new followTarget = map_get(m_inSpawnSession, id);
		if(followTarget == -1) {
			return Y_HOOKS_BREAK_RETURN_1;
		}
		new
			Float:x,
			Float:y,
			Float:z;
		FCNPC_GetPosition(npcid, x, y, z);
		if(Breed_IsPointInArea(x, y, z)) {
			map_remove(m_inSpawnSession, id);
			if(!map_size(m_inSpawnSession)) {
				map_delete(m_inSpawnSession);
				m_inSpawnSession = Map:-1;
			}
			SubtitleText_Show(followTarget, "Vat nuoi da duoc dua vao ~y~khu dat chan nuoi~w~, hay theo doi va cham soc.");
			Breed_Move(id);
			return Y_HOOKS_BREAK_RETURN_1;
		}

		if(GetPlayerDistanceFromPoint(followTarget, x, y, z) > 7.0) {
			SubtitleText_Show(followTarget, "~r~Ban dang di qua nhanh, vat nuoi khong theo kip. ~w~Hay tro lai de dan vat nuoi theo.");
			map_set(m_inSpawnSession, id, -1);
		} else {
			if(GetPlayerDistanceFromPoint(followTarget, x, y, z) < 2.5) {
				if(!map_valid(m_distCheckTimer)) {
					m_distCheckTimer = map_new();
				}
				map_set(m_distCheckTimer, id, SetPreciseTimer("Breed_DistanceCheckTimer", 500, true, "i", id));
				return Y_HOOKS_BREAK_RETURN_1;
			}
			new Float:angle;
			GetPlayerPos(followTarget, x, y, z);
			GetPlayerFacingAngle(followTarget, angle);
			Util_GetPointInFront2D(x, y, angle, -1.0, x, y);
			FCNPC_GoTo(npcid, x, y, z, .speed = FCNPC_MOVE_SPEED_WALK);
			SetPlayerCheckpoint(followTarget, x, y, z, 2.0);
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}