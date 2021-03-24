#if defined player_hand_carry_included
	#endinput
#endif

#define player_hand_carry_included

enum _:CarryStats {
	crItemId,
	crAttachSlot
}

stock Player_CarryItem(playerid, id) {
	if(map_valid(m_carryStats) && map_has_key(m_carryStats, playerid)) {
		return 0;
	}

	if(!Item_Has(id)) {
		return 0;
	}

	new slot = Player_GetFreeAttachmentSlot(playerid, true);
	if(slot == -1) {
		return 0;
	}

	new
		stats[CarryStats],
		query[128];
	stats[crItemId] = id;
	stats[crAttachSlot] = slot;
	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_CARRY);
	SetPlayerAttachedObject(playerid, slot, Item_GetModel(id), 1);
	if(!map_valid(m_carryStats)) {
		m_carryStats = map_new();
	}
	map_set_arr(m_carryStats, playerid, stats);
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO player_hand_carry(id, item_id) VALUES (%d, %d)", playerid, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Player_StopCarry(playerid) {
	if(!map_valid(m_carryStats) || !map_has_key(m_carryStats, playerid)) {
		return 0;
	}

	new
		stats[CarryStats],
		query[128];
	map_get_arr(m_carryStats, playerid, stats);
	RemovePlayerAttachedObject(playerid, stats[crAttachSlot]);
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM player_hand_carry WHERE id = %d LIMIT 1", playerid);
	mysql_pquery(MySQL_Connection(), query);
	map_remove(m_carryStats, playerid);
	if(!map_size(m_carryStats)) {
		map_delete(m_carryStats);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Player_OnStatsLoaded(playerid) {
	inline const QueryFinished() {
		if(cache_num_rows()) {
			new slot = Player_GetFreeAttachmentSlot(playerid, true);
			if(slot != -1) {
				new stats[CarryStats];
				cache_get_value_name_int(0, "item_id", stats[crItemId]);
				stats[crAttachSlot] = slot;
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_CARRY);
				SetPlayerAttachedObject(playerid, slot, Item_GetModel(stats[crItemId]), 1);
				if(!map_valid(m_carryStats)) {
					m_carryStats = map_new();
				}
				map_set_arr(m_carryStats, playerid, stats);
			} else {
				new
					item_id,
					Float:x,
					Float:y,
					Float:angle,
					query[128];
				cache_get_value_name_int(0, "item_id", item_id);
				mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM player_hand_carry WHERE id = %d LIMIT 1", playerid);
				mysql_pquery(MySQL_Connection(), query);
				GetPlayerPos(playerid, x, y, angle);
				GetPlayerFacingAngle(playerid, angle);
				IGround_CAUpdate(item_id, x, y, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
			}
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_hand_carry WHERE id = %d LIMIT 1", playerid);
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_hand_carry (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	item_id INT UNSIGNED,\
	FOREIGN KEY (id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (item_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}