#if defined breed_actor_included
	#endinput
#endif

#define breed_actor_included

static breedActor;

SubtitleSelect:BreedActorMenu(playerid, response, listitem) {
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

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES && GetPlayerTargetActor(playerid) == breedActor) {
		SubtitleSelect_Show(playerid, BreedActorMenu, "~g~Nong dan: ~w~Toi co the giup gi cho ban?", "Nuoi bo thit~n~Nuoi bo sua");
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook OnGameModeInit() {
	breedActor = CreateActor(239, -12.9521, 47.8490, 3.1172, 251.8990);
	return 1;
}