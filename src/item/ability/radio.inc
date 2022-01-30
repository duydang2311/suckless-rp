#if defined item_radio_included
	#endinput
#endif

#define item_radio_included

static Map:m_radioStats;

static const Float:iReliabilityRadio = 1.0;
static const occupiedSpaceRadio = 1;

enum ItemRadioModel {
    modelBnFMinimal,
    modelBnFPro
}

enum _:ItemRadioStats {
    radioFrequency,
    ItemRadioModel:radioModel
};

stock Task:IRadio_Create(frequency = 0, ItemRadioModel:model) {
    new Task:t = task_new();
    task_detach();

    new query[83 + 3 + 11 + 3 - 8 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", _:ITEM_ABILITY:iAbilityRadio, iReliabilityRadio, occupiedSpaceRadio);
    mysql_pquery(MySQL_Connection(), query, "IRadio_OnCreated", "ddd", frequency, _:model, _:t);
	task_set_result_ms(t, -1, 2000);
    return t;
}

stock ItemRadioModel:IRadio_GetModelType(model) {
    if(model == 0) {
        return modelBnFMinimal;
    }

    return modelBnFPro;
}

stock IRadio_GetModel() {
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

forward IRadio_OnCreated(frequency, ItemRadioModel:model, Task:t);
public IRadio_OnCreated(frequency, ItemRadioModel:model, Task:t) {
    new insert_id = -1;
    if(cache_affected_rows()) {
        if(!map_valid(m_radioStats)) {
            m_radioStats = map_new();   
        }

        new 
            radio_stats[ItemRadioStats],
            query[67 + 11 + 3 + 11 - 6 + 1];

        insert_id = cache_insert_id();
        mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO `item_radio` (id, model, frequency) VALUES (%d, %d, %d)", insert_id, _:model, frequency);
        mysql_pquery(MySQL_Connection(), query);

        radio_stats[radioFrequency] = frequency;
        radio_stats[radioModel] = model;
        map_set_arr(m_radioStats, insert_id, radio_stats);
        CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityRadio, iReliabilityRadio, occupiedSpaceRadio);
        task_set_result(t, insert_id);
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
        cache_get_value_name_int(0, "model", _:stats[radioModel]);
        cache_get_value_name_int(0, "frequency", stats[radioFrequency]);
        map_set_arr(m_radioStats, id, stats);
        printf("[debug] Map size: %d | model: %d | frequency: %d", map_size(m_radioStats), _:stats[radioModel], stats[radioFrequency]);
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

    new query[150];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM item_stats WHERE NOT EXISTS ( SELECT * FROM item_radio WHERE item_stats.id = item_radio.id ) AND item_stats.ability = %d", iAbilityRadio);
	new Cache:result = mysql_query(MySQL_Connection(), query, true);
	if(cache_affected_rows()) {
		printf("INFO: Deleted %d rows with non-existing referenced `id` to item_radio in item_stats.", cache_affected_rows());
	}
	cache_delete(result);
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
	if(task_await(Item_GetAbility(id)) == iAbilityRadio) {
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT `model`, `frequency` FROM item_radio WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query, "IRadio_OnStatsRetrieving", "ii", id, _:t);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnModelGetAttempt(id, &model) {
	if(task_await(Item_GetAbility(id)) == iAbilityRadio) {
		model = IRadio_GetModel();
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnNameGetAttempt(id, name[], size) {
    if(task_await(Item_GetAbility(id)) == iAbilityRadio) { 
        new stats[ItemRadioStats];
        if(map_get_arr_safe(m_radioStats, id, stats)) {
            name[0] = EOS;
            if(stats[radioModel] == modelBnFMinimal) {
                strcat(name, "Back 'n Forth Minimal Radio", size);
            } else if(stats[radioModel] == modelBnFPro) {
                strcat(name, "Back 'n Forth Pro Radio", size);
            } else {
                strcat(name, "", size);
            }

            return Y_HOOKS_BREAK_RETURN_1;
        }
    }
    return 1;
}
