#if defined breed_animal_interact_included
	#endinput
#endif

#define breed_animal_interact_included

static Map:m_breedId;

SubtitleSelect:BreedAnimalInteractMenu(playerid, response, listitem) {
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
			}
		}
	}
	Breed_Move(id);
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
		new npcid = GetPlayerTargetPlayer(playerid);
		if(npcid == INVALID_PLAYER_ID) {
			return 1;
		}
		new id = Breed_GetIdByNPCId(npcid);
		if(id == -1) {
			return 1;
		}
		if(Player_GetCarryingItem(playerid) != -1) {
			return 1;
		}

		new
			string[128],
			name[32];
		FCNPC_Stop(npcid);
		FCNPC_SetAngleToPlayer(npcid, playerid);
		Breed_GetAnimalSpeciesName(Breed_GetSpecies(id), name);
		format(string, sizeof(string), "~g~%s (%d): ~w~??", name, id);
		SubtitleSelect_Show(playerid, BreedAnimalInteractMenu, string, "Tinh trang suc khoe nhu nao?");
		if(!map_valid(m_breedId)) {
			m_breedId = map_new();
		}
		map_set(m_breedId, playerid, id);
	}
	return 1;
}