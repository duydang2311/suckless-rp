#if defined breed_cmds
	#endinput
#endif

#define breed_cmds

CMD:anml(playerid) {
	Breed_CreateAnimal(random(4));
	return 1;
}