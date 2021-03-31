#if defined breed_actor_included
	#endinput
#endif

#define breed_actor_included

static breedActor;

forward subSelectBreedActor(playerid, response, item);
public subSelectBreedActor(playerid, response, item) {
	printf("%d, %d, %d", playerid, response, item);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES && GetPlayerTargetActor(playerid) == breedActor) {
		SubtitleSelect_Show(playerid, subSelectBreedActor, "~g~Nong dan: ~w~Toi co the giup gi cho ban?", "Nuoi bo thit~n~Nuoi bo sua~n~Ban bo thit~n~Ban bo sua");
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