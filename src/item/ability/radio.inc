#if defined item_radio_included
	#endinput
#endif

#define item_radio_included

static Map:m_radioStats;

enum ItemRadioModel {
    minimalModelRadio,
    proModelRadio
}

enum _:ItemRadioStats {
    radioFrequency,
    ItemRadioModel:radioModelType
};

stock Task:IRadio_Create(frequency = 0, ItemRadioModel:type) {
    new Task:t = task_new();
    task_detach();

    new query[83 + 3 + 11 + 3 - 8 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", _:ITEM_ABILITY:iAbilityRadio, reliability, occupied_space);
    mysql_pquery(MySQL_Connection(), query, "IRadio_OnCreated", "dd", frequency, _:type);
	task_set_result_ms(t, -1, 2000);
    return t;
}

stock IRadio_GetObjectModel() {
    return 19942;
}

forward IRadio_OnDeleted(id);
public IRadio_OnDeleted(id) {
    if(map_valid(m_radioStats) && map_has_key(m_radioStats, id)) {
        map_remove(m_radioStats, id);
        if(!map_size(m_radioStats)) {
            map_delete(m_radioStats);
            m_radioStats = Map:-1;
        }
    }
    return 1;
}

forward IRadio_OnCreated(frequency, ItemRadioModel:type);
public IRadio_OnCreated(frequency, ItemRadioModel:type) {
    new insert_id = -1;
    if(cache_affected_rows()) {
        if(!map_valid(m_radioStats)) {
            m_radioStats = map_new();
        }

        insert_id = cache_insert_id();
        new stats[ItemRadioStats];
        stats[radioFrequency] = frequency;
        stats[radioModelType] = type;
        map_set_arr(m_radioStats, insert_id, stats);

        new query[67 + 11 + 3 + 11 - 6 + 1];
        mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO `item_radio` (id, model, frequency) VALUES (%d, %d, %d)", insert_id, _:type, frequency);
        mysql_pquery(MySQL_Connection(), query);
        CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityRadio, 1.0, 2);
    }

    return 1;
}

forward IRadio_OnStatsRetrieving(id, Task:t);
public IRadio_OnStatsRetrieving(id, Task:t) {
    if(cache_num_rows()) {
        if(!map_valid(m_radioStats)) {
            m_radioStats = map_new();
        }

        new stats[ItemRadioStats];
        cache_get_value_name_int(0, "model", _:stats[radioModelType]);
        cache_get_value_name_int(0, "frequency", stats[radioFrequency]);
        map_set_arr(m_radioStats, id, stats);
    }
    if(task_valid(t)) {
        task_set_result(t, 1);
    }
    return 1;
}

hook OnGameModeInit@6() {
    mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_radio (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
    model TINYINT UNSIGNED DEFAULT 0,\
    frequency INT UNSIGNED DEFAULT 0,\
    FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE)", false);
    return 1;
}

hook OnGameModeExit@6() {
    if(map_valid(m_radioStats)) {
        map_delete_deep(m_radioStats);
        m_radioStats = Map:-1;
    }
    return 1;
}

#include <YSI_Coding\y_hooks>

hook Item_OnStatsRetrieved(id, Task:t) {
	if(Item_GetAbility(id) == iAbilityRadio) {
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM item_radio WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query, "IRadio_OnStatsRetrieving", "ii", id, _:t);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnModelGetAttempt(id, &model) {
	if(Item_GetAbility(id) == iAbilityRadio) {
		model = IRadio_GetObjectModel();
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnNameGetAttempt(id, name[], size) {
    if(Item_GetAbility(id) == iAbilityRadio) { 
        new stats[ItemRadioStats];
        if(map_get_arr_safe(m_radioStats, id, stats)) {
            name[0] = EOS;
            if(stats[radioModelType] == minimalModelRadio) {
                strcat(name, "Back 'n Forth Minimal Radio", size);
            } else if(stats[radioModelType] == proModelRadio) {
                strcat(name, "Back 'n Forth Pro Radio", size);
            } else {
                strcat(name, "", size);
            }

            return Y_HOOKS_BREAK_RETURN_1;
        }
    }
    return 1;
}