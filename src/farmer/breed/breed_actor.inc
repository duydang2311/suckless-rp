#if defined breed_actor_included
	#endinput
#endif

#define breed_actor_included

static
	storageActor,
	sellingActor[2];

SubtitleSelect:BreedStorageMenu(playerid, response, listitem) {
	if(response) {
		new id = -1;
		switch(listitem) {
			case 0: { // Nuoi bo thit
				id = await Breed_CreateAnimal(random(2) ? anmlBrownCow_1 : anmlBrownCow_2, Player_GetId(playerid));
			}
			case 1: { // Nuoi bo sua
				id = await Breed_CreateAnimal(random(2) ? anmlDiaryCow_1 : anmlDiaryCow_2, Player_GetId(playerid));
			}
		}
		if(id != -1) {
			new
				string[128],
				name[32];
			Breed_GetAnimalSpeciesName(Breed_GetSpecies(id), name);
			format(string, sizeof(string), "~g~Nong dan: ~y~%s (%d) ~w~da duoc mang ra, hay dan vat nuoi vao khu dat chan nuoi.", name, id);
			SubtitleText_Show(playerid, string);
		}
	}
	return 1;
}

SubtitleSelect:BreedSellingMenu(playerid, response, listitem) {
	if(response) {
		switch(listitem) {
			case 0: { //Ban bo
				new Iter:it = Breed_GetIter();
				if(iter_valid(it) && iter_release(it)) {
					for(new id; iter_inside(it); iter_move_next(it)) {
						id = iter_get_key(it);
						if(Breed_GetFollowingPlayer(id) == playerid) {
							if(Breed_GetMaturity(id) < 0.5) {
								SubtitleText_Show(playerid, "Vat nuoi chua truong thanh.");
								return 1;
							}

							new
								Float:price = Breed_GetMaturity(id) * 120 + BrownCow_GetQuality(id) * 100,
								string[128];
							format(string, sizeof(string), "Ban nhan duoc ~y~%d~g~$~w~ tu so tien ban duoc vat nuoi.", floatround(price));
							SubtitleText_Show(playerid, string);
							Breed_Remove(id);
							return 1;
						}
					}
				}
				SubtitleText_Show(playerid, "Ban khong dan theo vat nuoi nao de ban.");
			}
		}
 	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES) {
		new target = GetPlayerTargetActor(playerid);
		if(target == storageActor) {
			SubtitleSelect_Show(playerid, BreedStorageMenu, "~g~Nong dan: ~w~Ban muon gi?", "Nuoi bo thit~n~Nuoi bo sua");
			return Y_HOOKS_BREAK_RETURN_1;
		} else if(target == sellingActor[0] || target == sellingActor[1]) {
			SubtitleSelect_Show(playerid, BreedSellingMenu, "~y~Nong dan: ~w~Ban muon gi?", "Ban bo");
			return Y_HOOKS_BREAK_RETURN_1;
		}
	}
	return 1;
}

hook OnGameModeInit() {
	storageActor = CreateActor(239, -12.9521, 47.8490, 3.1172, 251.8990);
	sellingActor[0] = CreateActor(239, 1022.8801, -316.0418, 73.9889, 180.8697);
	sellingActor[1] = CreateActor(239, 1048.6515, -316.6358, 73.9922, 179.4911);
	return 1;
}