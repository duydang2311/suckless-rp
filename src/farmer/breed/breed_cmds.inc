#if defined breed_cmds
	#endinput
#endif

#define breed_cmds

CMD:anml(playerid) {
	Breed_CreateAnimal(random(3));
	return 1;
}