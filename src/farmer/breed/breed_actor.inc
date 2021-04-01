#if defined breed_actor_included
	#endinput
#endif

#define breed_actor_included

static breedActor;

SubtitleSelect:BreedActorMenu(playerid, response, listitem) {
	if(response) {
		switch(listitem) {
			case 0: { // Nuoi bo thit
				new id = await Breed_CreateAnimal(random(2) ? anmlBrownCow_1 : anmlBrownCow_2, Player_GetId(playerid));
				printf("Brown Cow %d, Breeder Id %d", id, Player_GetId(playerid));
			}
			case 1: { // Nuoi bo sua
				new id = await Breed_CreateAnimal(random(2) ? anmlDiaryCow_1 : anmlDiaryCow_2, Player_GetId(playerid));
				printf("Diary Cow %d, Breeder Id %d", id, Player_GetId(playerid));
			}
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES && GetPlayerTargetActor(playerid) == breedActor) {
		SubtitleSelect_Show(playerid, BreedActorMenu, "~g~Nong dan: ~w~Toi co the giup gi cho ban?", "Nuoi bo thit~n~Nuoi bo sua");
		// SubtitleSelect_Show(playerid, "~g~Nong dan: ~w~Toi co the giup gi cho ban?", "Nuoi bo thit~n~Nuoi bo sua~n~Ban bo thit~n~Ban bo sua", "OnBreedActorResponse");
		printf("You are targeting breedActor (ID %d)", breedActor);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook OnGameModeInit() {
	breedActor = CreateActor(239, -12.9521, 47.8490, 3.1172, 251.8990);
	return 1;
}