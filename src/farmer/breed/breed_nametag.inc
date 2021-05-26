#if defined breed_nametag_included
	#endinput
#endif

#define breed_nametag_included

static
	Map:m_nametagLabel,
	Map:m_nametagForPlayerId;

stock Breed_ShowNameTagForPlayer(id, playerid) {
	new
		npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}

	new
		name[48],
		LinkedList:ll;
	Breed_GetAnimalSpeciesName(Breed_GetSpecies(id), name);
	format(name, sizeof(name), "%s (%d)", name, id);
	new Text3D:label = CreateDynamic3DTextLabel(name, -1, 0.0, 0.0, 0.0, 10.0, npcid, .worldid = FCNPC_GetVirtualWorld(npcid), .interiorid = FCNPC_GetInterior(npcid), .playerid = playerid, .streamdistance = 15.0);
	if(!map_valid(m_nametagLabel)) {
		m_nametagLabel = map_new();
	}
	if(!map_valid(m_nametagForPlayerId)) {
		m_nametagForPlayerId = map_new();
		ll = linked_list_new();
		map_set(m_nametagForPlayerId, playerid, ll);
	} else {
		ll = LinkedList:map_get(m_nametagForPlayerId, playerid);
	}
	map_set(m_nametagLabel, id, _:label);
	linked_list_add(ll, id);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Breed_OnCreated(id) {
	new breeder = Breed_GetBreederId(id);
	if(breeder != -1) {
		new playerid = Player_GetById(breeder);
		if(playerid != -1) {
			Breed_ShowNameTagForPlayer(id, playerid);
		}
	}
	return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
	if(!map_valid(m_nametagForPlayerId) || !map_has_key(m_nametagForPlayerId, playerid)) {
		return 1;
	}

	new LinkedList:ll = LinkedList:map_get(m_nametagForPlayerId, playerid);
	for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
		new breed_id = iter_get(it);
		if(map_valid(m_nametagLabel) && map_has_key(m_nametagLabel, breed_id)) {
			DestroyDynamic3DTextLabel(Text3D:map_get(m_nametagLabel, breed_id));
			map_remove(m_nametagLabel, breed_id);
			if(!map_size(m_nametagLabel)) {
				map_delete(m_nametagLabel);
				m_nametagLabel = Map:-1;
			}
		}
	}
	linked_list_delete(ll);
	if(!map_size(m_nametagForPlayerId)) {
		map_delete(m_nametagForPlayerId);
		m_nametagForPlayerId = Map:-1;
	}
	return 1;
}

hook Player_OnStatsRetrieved(playerid) {
	new Iter:it = Breed_GetIter();
	if(iter_valid(it) && iter_release(it)) {
		new
			stats[BreedAnimalStats],
			id = Player_GetId(playerid);
		if(id != -1) {
			for(; iter_inside(it); iter_move_next(it)) {
				iter_get_arr(it, stats);
				if(stats[anmlBreederId] == id) {
					Breed_ShowNameTagForPlayer(iter_get_key(it), playerid);
				}
			}
		}
	}
	return 1;
}