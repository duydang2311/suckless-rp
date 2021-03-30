#if defined breed_actor_included
	#endinput
#endif

#define breed_actor_included

static breedActor;

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES && GetPlayerTargetActor(playerid) == breedActor) {
		Subtitle_Show(playerid, "~w~Toi co the giup gi cho ban?", "~g~Nong dan");
		printf("You are targeting breedActor (ID %d)", breedActor);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook OnGameModeInit() {
	breedActor = CreateActor(239, -12.9521, 47.8490, 3.1172, 251.8990);
	return 1;
}