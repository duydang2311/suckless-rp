#if defined player_death_included
	#endinput
#endif

#define player_death_included

static actor;

forward Player_DeathSetup(playerid);
public Player_DeathSetup(playerid) {
	new
		Float:pos[3],
		Float:cam_pos[3],
		Float:look_pos[3],
		Float:a;
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	GetPlayerFacingAngle(playerid, a);
	GetPlayerCameraPos(playerid, cam_pos[0], cam_pos[1], cam_pos[2]);
	GetPlayerCameraFrontVector(playerid, look_pos[0], look_pos[1], look_pos[2]);
	look_pos[0] += cam_pos[0];
	look_pos[1] += cam_pos[1];
	look_pos[2] += cam_pos[2];

	actor = CreateActor(GetPlayerSkin(playerid), pos[0], pos[1], pos[2], a);
	TogglePlayerSpectating(playerid, true);
	InterpolateCameraLookAt(playerid, look_pos[0], look_pos[1], look_pos[2], pos[0], pos[1], pos[2], 2000, CAMERA_MOVE);
	InterpolateCameraPos(playerid, cam_pos[0], cam_pos[1], cam_pos[2], cam_pos[0], cam_pos[1], pos[2] + 2.0, 2000, CAMERA_MOVE);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerDeath(playerid, killerid, reason) {
	SetPreciseTimer("Player_DeathSetup", 2000, false, "i", playerid);
	return 1;
}

