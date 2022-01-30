#if defined phone_stats_included
	#endinput
#endif

#define phone_stats_included

enum _:PhoneState {
	phScreenOff,
	phScreenOn,
	phPowerOff
}

enum _:ItemPhoneStats {
	iPhoneModel,
	Float:iPhonePower,
	iPhoneState
}

static Map:m_phoneStats;

stock Task:IPhone_Create(model, Float:power = 1.0, phoneState = phScreenOff, Float:reliability = 1.0, occupied_space = 1) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new insert_id = -1;
		if(cache_affected_rows()) {
			new
				stats[ItemPhoneStats],
				query[150];
			insert_id = cache_insert_id();
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_phone (id, model, power, phone_state) VALUES (%d, %d, %.4f, %d)", insert_id, model, power, phoneState);
			mysql_pquery(MySQL_Connection(), query);

			stats[iPhonePower] = power;
			stats[iPhoneModel] = model;
			stats[iPhoneState] = phoneState;
			if(!map_valid(m_phoneStats)) {
				m_phoneStats = map_new();
			}
			map_set_arr(m_phoneStats, insert_id, stats);
			CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityPhone, reliability, occupied_space);
		}
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", iAbilityPhone, reliability, occupied_space);
	task_set_result_ms(t, -1, 2000);
	return t;
}

stock IPhone_Has(id) {
	return (map_valid(m_phoneStats) && map_has_key(m_phoneStats, id));
}

stock IPhone_GetModel(id) {
	if(!IPhone_Has(id)) {
		return -1;
	}
	new stats[ItemPhoneStats];
	map_get_arr(m_phoneStats, id, stats);
	return stats[iPhoneModel];
}

hook OnGameModeInit@8() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS item_phone (\
		created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
		modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
		id INT UNSIGNED PRIMARY KEY,\
		model TINYINT UNSIGNED DEFAULT 0,\
		power FLOAT(11, 4) DEFAULT 1.0000,\
		phone_state TINYINT UNSIGNED DEFAULT 0,\
		FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new query[150];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM item_stats WHERE NOT EXISTS ( SELECT * FROM item_phone WHERE item_stats.id = item_phone.id ) AND item_stats.ability = %d", iAbilityPhone);
	new Cache:result = mysql_query(MySQL_Connection(), query, true);
	if(cache_affected_rows()) {
		printf("INFO: Deleted %d rows with non-existing referenced `id` to item_phone in item_stats.", cache_affected_rows());
	}
	cache_delete(result);
	return 1;
}

#include <YSI_Coding\y_hooks>

forward IPhone_OnStatsRetrieving(id, Task:t);
public IPhone_OnStatsRetrieving(id, Task:t) {
	if(cache_num_rows()) {
		new stats[ItemPhoneStats];
		cache_get_value_name_int(0, "model", stats[iPhoneModel]);
		cache_get_value_name_int(0, "phone_state", stats[iPhoneState]);
		cache_get_value_name_float(0, "power", stats[iPhonePower]);

		if(!map_valid(m_phoneStats)) {
			m_phoneStats = map_new();
		}
		map_set_arr(m_phoneStats, id, stats);
	}
	if(task_valid(t)) {
		task_set_result(t, 1);
	}
	return 1;
}

hook Item_OnStatsRetrieved(id, Task:t) {
	if(task_await(Item_GetAbility(id)) == iAbilityPhone) {
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM item_phone WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query, "IPhone_OnStatsRetrieving", "ii", id, _:t);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnModelGetAttempt(id, &model) {
	if(task_await(Item_GetAbility(id)) == iAbilityPhone) {
		model = 330;
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnNameGetAttempt(id, name[], size) {
	if(task_await(Item_GetAbility(id)) == iAbilityPhone) {
		name[0] = EOS;
		strcat(name, "Nokia Phone", size);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IGroup_OnAddingAttempt(group, member, &error) {
	if(IPhone_Has(group)) {
		if(!ISim_Has(member)) {
			error = grErrorNotSupported;
		} else if(IGroup_Has(group)) {
			error = grErrorCapacityLack;
		} else {
			error = grErrorNone;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	} else {
		error = grErrorNotSupported;
	}
	return 1;
}
