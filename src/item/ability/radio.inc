#if defined item_radio_included
	#endinput
#endif

#define item_radio_included

static Map:m_radioStats;

enum _:ItemRadioStats {
    Float:radioFrequency
};

stock Task:IRadio_Create(Float:reliability = 1.0, occupied_space = 2) {
    new Task:t = task_new();
    task_detach();

    new query[83 + 10 + 11 + 3 + 11 - 7 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", _:ITEM_ABILITY:iAbilityMelee, reliability, occupied_space);
    mysql_pquery(MySQL_Connection(), query, "AbilityItem_OnRadioCreated", "fd", reliability, occupied_space);
	task_set_result_ms(t, -1, 2000);
    return t;
}

forward AbilityItem_OnRadioCreated(Float:reliability, occupied_space);
public AbilityItem_OnRadioCreated(Float:reliability, occupied_space) {
    new insert_id = -1;
    if(cache_affected_rows()) {
        if(!map_valid(m_radioStats)) {
            m_radioStats = map_new();
        }

        insert_id = cache_insert_id();
        new stats[ItemRadioStats] = {0.0};
        map_set_arr(m_radioStats, insert_id, stats);

        new query[97 + 10 + 11 + 3 + 11 - 12 + 1];
        mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO `item_radio` (id, reliability, occupied_space, frequency) VALUES (%d, %d, %.4f, %.4f)", insert_id, 1.0, 2, 0.0);
        mysql_pquery(MySQL_Connection(), query);
        CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityRadio, reliability, occupied_space);
    }

    return 1;
}

hook OnGameModeInit@6() {
    mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_radio (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	ability TINYINT UNSIGNED DEFAULT 0,\
	reliability FLOAT(11, 4) DEFAULT 0.0000,\
	occupied_space TINYINT UNSIGNED DEFAULT 0,\
    frequency FLOAT(11, 4) DEFAULT 0.0000)", false);

    new id = await IRadio_Create(1.0, 2);
    return 1;
}

hook OnGameModeExit@6() {
    if(map_valid(m_radioStats)) {
        map_delete_deep(m_radioStats);
        m_radioStats = Map:-1;
    }
    return 1;
}
