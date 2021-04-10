#if defined breed_spawn_included
	#endinput
#endif

#define breed_spawn_included

static const Float:BreedSpawnPos[][4] = {
	{-2.0429, 74.7125, 3.1172, 250.0000},
	{-9.7621, 54.7767, 3.1172, 250.0000}
};

static
	Map:m_inSpawnSession;

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
					Breed_Follow(breed_id, playerid);
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

hook Breed_OnFollowingUpdate(id) {
	if(map_valid(m_inSpawnSession) && map_has_key(m_inSpawnSession, id)) {
		new
			followTarget = Breed_GetFollowingPlayer(id),
			npcid = Breed_GetNPCId(id);
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
			Breed_StopFollowing(id);
			Breed_Move(id);
			return Y_HOOKS_BREAK_RETURN_1;
		}

		if(GetPlayerDistanceFromPoint(followTarget, x, y, z) > 10.0) {
			Breed_StopFollowing(id);
			SubtitleText_Show(followTarget, "Ban dang di ~r~qua nhanh~w~, vat nuoi khong theo kip. Hay tro lai de dan vat nuoi theo.");
			map_set(m_inSpawnSession, id, -1);
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}