#if defined breed_lifecycle_included
	#endinput
#endif

#define breed_lifecycle_included

forward Breed_Move(id);
public Breed_Move(id) {
	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}

	if(Breed_GetFoodObjectId(id) != -1) {
		return 0;
	}

	Breed_UpdateHunger(id, -0.005);
	new Float:hunger = Breed_GetHunger(id);
	if(hunger < 0.0) {
		if(hunger < -0.5) {
			Breed_RemoveAnimal(id);
			return 1;
		}
		switch(Breed_GetSpecies(id)) {
			case anmlBrownCow_1, anmlBrownCow_2: {
				BrownCow_UpdateQuality(id, -0.05);
			}
		}
	} else {
		switch(Breed_GetSpecies(id)) {
			case anmlDiaryCow_1, anmlDiaryCow_2: {
				DiaryCow_UpdateMilk(id, 0.05);
			}
		}
	}
	new
		Float:x,
		Float:y,
		Float:z;
	Breed_GetRandomPointInArea(x, y);
	CA_FindZ_For2DCoord(x, y, z);
	FCNPC_GoTo(npcid, x, y, z, .speed = FCNPC_MOVE_SPEED_WALK);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook FCNPC_OnReachDest(npcid) {
	new id = Breed_GetIdByNPCId(npcid);
	if(id != -1 && Breed_GetFollowingPlayer(id) == -1) {
		SetPreciseTimer("Breed_Move", 1000, false, "i", id);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}