#if defined breed_follow_included
	#endinput
#endif

#define breed_follow_included

static
	Map:m_followPlayerId,
	Map:m_distCheckTimer;

forward Breed_DistanceCheckTimer(id);
public Breed_DistanceCheckTimer(id) {
	new npcid = Breed_GetNPCId(id);
	if(npcid == -1 || !map_valid(m_followPlayerId) || !map_has_key(m_followPlayerId, id)) {
		if(map_valid(m_distCheckTimer) && map_has_key(m_distCheckTimer, id)) {
			DeletePreciseTimer(map_get(m_distCheckTimer, id));
			map_remove(m_distCheckTimer, id);
			if(!map_size(m_distCheckTimer)) {
				map_delete(m_distCheckTimer);
				m_distCheckTimer = Map:-1;
			}
		}
		return 0;
	}

	new
		followTarget = map_get(m_followPlayerId, id),
		Float:x,
		Float:y,
		Float:z;
	FCNPC_GetPosition(npcid, x, y, z);
	if(GetPlayerDistanceFromPoint(followTarget, x, y, z) >= 1.5) {
		new Float:angle;
		GetPlayerPos(followTarget, x, y, z);
		GetPlayerFacingAngle(followTarget, angle);
		Util_GetPointInFront2D(x, y, angle, -1.0, x, y);
		FCNPC_GoTo(npcid, x, y, z, .speed = FCNPC_MOVE_SPEED_WALK);
		if(map_valid(m_distCheckTimer) && map_has_key(m_distCheckTimer, id)) {
			DeletePreciseTimer(map_get(m_distCheckTimer, id));
			map_remove(m_distCheckTimer, id);
			if(!map_size(m_distCheckTimer)) {
				map_delete(m_distCheckTimer);
				m_distCheckTimer = Map:-1;
			}
		}
	}
	return 1;
}

stock Breed_Follow(id, playerid) {
	if(map_valid(m_followPlayerId) && map_has_key(m_followPlayerId, id)) {
		return 0;
	}

	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}
	if(!map_valid(m_distCheckTimer)) {
		m_distCheckTimer = map_new();
	}
	if(!map_valid(m_followPlayerId)) {
		m_followPlayerId = map_new();
	}
	map_set(m_followPlayerId, id, playerid);
	map_set(m_distCheckTimer, id, SetPreciseTimer("Breed_DistanceCheckTimer", 500, true, "i", id));
	return 1;
}

stock Breed_GetFollowingPlayer(id) {
	if(!map_valid(m_followPlayerId) || !map_has_key(m_followPlayerId, id)) {
		return -1;
	}
	return map_get(m_followPlayerId, id);
}

stock Breed_StopFollowing(id) {
	if(!map_valid(m_followPlayerId) || !map_has_key(m_followPlayerId, id)) {
		return 0;
	}
	new npcid = Breed_GetNPCId(id);
	if(npcid != -1) {
		FCNPC_Stop(npcid);
	}

	if(map_valid(m_distCheckTimer) && map_has_key(m_distCheckTimer, id)) {
		DeletePreciseTimer(map_get(m_distCheckTimer, id));
		map_remove(m_distCheckTimer, id);
		if(!map_size(m_distCheckTimer)) {
			map_delete(m_distCheckTimer);
			m_distCheckTimer = Map:-1;
		}
	}
	if(map_valid(m_followPlayerId) && map_has_key(m_followPlayerId, id)) {
		map_remove(m_followPlayerId, id);
		if(!map_size(m_followPlayerId)) {
			map_delete(m_followPlayerId);
			m_followPlayerId = Map:-1;
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook FCNPC_OnReachDest(npcid) {
	new id = Breed_GetIdByNPCId(npcid);
	if(id != -1 && map_valid(m_followPlayerId) && map_has_key(m_followPlayerId, id)) {
		new
			Float:x,
			Float:y,
			Float:z,
			followId = map_get(m_followPlayerId, id);
		FCNPC_GetPosition(npcid, x, y, z);
		if(GetPlayerDistanceFromPoint(followId, x, y, z) < 2.5) {
			if(!map_valid(m_distCheckTimer)) {
				m_distCheckTimer = map_new();
			}
			map_set(m_distCheckTimer, id, SetPreciseTimer("Breed_DistanceCheckTimer", 500, true, "i", id));
			return Y_HOOKS_BREAK_RETURN_1;
		}
		new Float:angle;
		GetPlayerPos(followId, x, y, z);
		GetPlayerFacingAngle(followId, angle);
		Util_GetPointInFront2D(x, y, angle, -1.0, x, y);
		FCNPC_GoTo(npcid, x, y, z, .speed = FCNPC_MOVE_SPEED_WALK);
		CallLocalFunction("Breed_OnFollowingUpdate", "i", id);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}