#if defined item_radio_included
	#endinput
#endif

#define item_radio_included

static Map:m_radioStats;

enum _:ItemRadioStats {
    Float:radioFrequency
};

stock Task:IRadio_Create(Float:frequency = 0.0) {
    new Task:t = task_new();
    task_detach();

    new query[83 + 3 + 11 + 3 - 8 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", _:ITEM_ABILITY:iAbilityMelee, reliability, occupied_space);
    mysql_pquery(MySQL_Connection(), query, "AbilityItem_OnRadioCreated", "f", frequency);
	task_set_result_ms(t, -1, 2000);
    return t;
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

forward AbilityItem_OnRadioCreated(Float:frequency);
public AbilityItem_OnRadioCreated(Float:frequency) {
    new insert_id = -1;
    if(cache_affected_rows()) {
        if(!map_valid(m_radioStats)) {
            m_radioStats = map_new();
        }

        insert_id = cache_insert_id();
        new stats[ItemRadioStats] = {0.0};
        map_set_arr(m_radioStats, insert_id, stats);

        new query[58 + 10 + 11 - 6 + 1];
        mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO `item_radio` (id, frequency) VALUES (%d, %.4f)", insert_id, frequency);
        mysql_pquery(MySQL_Connection(), query);
        CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityRadio, 1.0, 2);
    }

    return 1;
}

hook OnGameModeInit@6() {
    mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_radio (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
    frequency FLOAT(11, 4) DEFAULT 0.0000)", false);
    return 1;
}

hook OnGameModeExit@6() {
    if(map_valid(m_radioStats)) {
        map_delete_deep(m_radioStats);
        m_radioStats = Map:-1;
    }
    return 1;
}
