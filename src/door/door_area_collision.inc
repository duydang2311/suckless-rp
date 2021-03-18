#if defined door_area_collision_included
	#endinput
#endif

#define door_area_collision_included

static
	Map:m_playerEnDoorId,
	Map:m_playerExDoorId;

stock Door_GetPlayerEntranceDoorId(playerid) {
	if(map_valid(m_playerEnDoorId)) {
		if(map_has_key(m_playerEnDoorId, playerid)) {
			return map_get(m_playerEnDoorId, playerid);
		}
	}
	return -1;
}

stock Door_GetPlayerExitDoorId(playerid) {
	if(map_valid(m_playerExDoorId)) {
		if(map_has_key(m_playerExDoorId, playerid)) {
			return map_get(m_playerExDoorId, playerid);
		}
	}
	return -1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerEnterDynArea(playerid, areaid) {
	if(Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID) == Door_GetEntranceExtraId()) {
		if(!map_valid(m_playerEnDoorId)) {
			m_playerEnDoorId = map_new();
		}
		map_set(m_playerEnDoorId, playerid, Door_GetIdByAreaId(areaid));
		return Y_HOOKS_BREAK_RETURN_1;
	} else if(Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID) == Door_GetExitExtraId()) {
		if(!map_valid(m_playerExDoorId)) {
			m_playerExDoorId = map_new();
		}
		map_set(m_playerExDoorId, playerid, Door_GetIdByAreaId(areaid));
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook OnPlayerLeaveDynArea(playerid, areaid) {
	if(Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID) == Door_GetEntranceExtraId()) {
		if(map_valid(m_playerEnDoorId)) {
			map_remove(m_playerEnDoorId, playerid);
			if(!map_size(m_playerEnDoorId)) {
				map_delete(m_playerEnDoorId);
				m_playerEnDoorId = Map:-1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	} else if(Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID) == Door_GetExitExtraId()) {
		if(map_valid(m_playerExDoorId)) {
			map_remove(m_playerExDoorId, playerid);
			if(!map_size(m_playerExDoorId)) {
				map_delete(m_playerExDoorId);
				m_playerExDoorId = Map:-1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}