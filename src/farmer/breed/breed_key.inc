#if defined breed_key_included
	#endinput
#endif

#define breed_key_included

static Map:m_breedId;

SubtitleSelect:BreedSS(playerid, response, listitem) {
	if(!map_valid(m_breedId) || !map_has_key(m_breedId, playerid)) {
		return 1;
	}

	new id = map_get(m_breedId, playerid);
	if(response) {
		switch(listitem) {
			case 0: { // Tinh trang suc khoe
				new string[128];
				format(string, sizeof(string), "~y~Hunger: ~w~%.2f~n~~y~Thirst: ~w~%.2f~n~~y~Maturity: ~w~%.2f", Breed_GetHunger(id), Breed_GetThirst(id), Breed_GetMaturity(id));
				switch(Breed_GetSpecies(id)) {
					case anmlBrownCow_1, anmlBrownCow_2: {
						format(string, sizeof(string), "%s~n~~y~Quality: ~w~%.2f", string, BrownCow_GetQuality(id));
					}
					case anmlDiaryCow_1, anmlDiaryCow_2: {
						format(string, sizeof(string), "%s~n~~y~Milk: ~w~%.2f", string, DiaryCow_GetMilk(id));
					}
				}
				SubtitleText_Show(playerid, string);
				new npcid = Breed_GetNPCId(id);
				if(npcid != -1) {
					new
						Float:x,
						Float:y,
						Float:z;
					FCNPC_GetPosition(npcid, x, y, z);
					if(Breed_IsPointInArea(x, y, z)) {
						Breed_Move(id);
					}
				}
			}
			case 1: { // Dan di theo
				new
					string[128],
					name[32];
				Breed_Follow(id, playerid);
				Breed_GetAnimalSpeciesName(Breed_GetSpecies(id), name);
				format(string, sizeof(string), "~y~%s (%d) ~w~dang di theo ban.", name, id);
				SubtitleText_Show(playerid, string);
			}
		}
	} else {
		new npcid = Breed_GetNPCId(id);
		if(npcid != -1) {
			new
				Float:x,
				Float:y,
				Float:z;
			FCNPC_GetPosition(npcid, x, y, z);
			if(Breed_IsPointInArea(x, y, z)) {
				Breed_Move(id);
			}
		}
	}
	map_remove(m_breedId, playerid);
	if(!map_size(m_breedId)) {
		map_delete(m_breedId);
		m_breedId = Map:-1;
	}
	return 1;
}

SubtitleSelect:BreedTrunkSS(playerid, response, listitem) {
	if(!map_valid(m_breedId) || !map_has_key(m_breedId, playerid)) {
		return 1;
	}

	if(response) {
		new id = map_get(m_breedId, playerid);
		switch(listitem) {
			case 0: { // Xuong khoi thung xe
				if(Breed_IsOnTrunk(id)) {
					Breed_RemoveFromTrunk(id);
				}
			}
		}
	}
	map_remove(m_breedId, playerid);
	if(!map_size(m_breedId)) {
		map_delete(m_breedId);
		m_breedId = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES) {
		new
			npcid = GetPlayerTargetPlayer(playerid),
			id = -1;
		if(npcid == INVALID_PLAYER_ID) {
			new Iter:it = Breed_GetNPCIter();
			if(!iter_valid(it)) {
				return 1;
			}
			iter_release(it);
			new
				Float:x,
				Float:y,
				Float:z,
				vw = GetPlayerVirtualWorld(playerid),
				int = GetPlayerInterior(playerid),
				Float:dist,
				Float:max_dist = 4.0 + 0.01;
			for(; iter_inside(it); iter_move_next(it)) {
				npcid = iter_get_key(it);
				if(!Breed_IsOnTrunk(iter_get(it))) {
					continue;
				}
				if(FCNPC_GetInterior(npcid) == int && FCNPC_GetVirtualWorld(npcid) == vw) {
					FCNPC_GetPosition(npcid, x, y, z);
					if((dist = GetPlayerDistanceFromPoint(playerid, x, y, z)) < max_dist) {
						max_dist = dist;
						id = iter_get(it);
					}
				}
			}
			if(id == -1) {
				return 1;
			}
			new
				string[128],
				name[32];
			Breed_GetAnimalSpeciesName(Breed_GetSpecies(id), name);
			format(string, sizeof(string), "~y~%s (%d): ~w~??", name, id);
			SubtitleSelect_Show(playerid, BreedTrunkSS, string, "Xuong khoi thung xe");
			if(!map_valid(m_breedId)) {
				m_breedId = map_new();
			}
			map_set(m_breedId, playerid, id);
			return Y_HOOKS_BREAK_RETURN_1;
		}

		id = Breed_GetIdByNPCId(npcid);
		if(id == -1) {
			return 1;
		}

		new followTarget = Breed_GetFollowingPlayer(id);
		if(followTarget == -1) {
			new item_id = Player_GetCarryingItem(playerid);
			if(item_id != -1 && task_await(Item_GetAbility(item_id)) == iAbilityWhisk) {
				if(Breed_Eat(id)) {
					Player_StopCarrying(playerid);
					Item_Delete(item_id);
				}
				return Y_HOOKS_BREAK_RETURN_1;
			}
			new
				string[128],
				name[32];
			FCNPC_Stop(npcid);
			FCNPC_SetAngleToPlayer(npcid, playerid);
			Breed_GetAnimalSpeciesName(Breed_GetSpecies(id), name);
			format(string, sizeof(string), "~g~%s (%d): ~w~??", name, id);
			SubtitleSelect_Show(playerid, BreedSS, string, "Xem tinh trang suc khoe~n~Dan di theo");
			if(!map_valid(m_breedId)) {
				m_breedId = map_new();
			}
			map_set(m_breedId, playerid, id);
			return Y_HOOKS_BREAK_RETURN_1;
		}
		if(followTarget == playerid) {
			Breed_StopFollowing(id);

			new
				Float:x,
				Float:y,
				Float:z;
			FCNPC_GetPosition(npcid, x, y, z);
			if(Breed_IsPointInArea(x, y, z)) {
				Breed_Move(id);
			}
			new
				string[128],
				name[32];
			Breed_GetAnimalSpeciesName(Breed_GetSpecies(id), name);
			format(string, sizeof(string), "~y~%s (%d)~w~ da ngung di theo ban.", name, id);
			SubtitleText_Show(playerid, string);
			return Y_HOOKS_BREAK_RETURN_1;
		}
	}
	return 1;
}
