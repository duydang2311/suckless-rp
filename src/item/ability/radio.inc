#if defined item_radio_included
	#endinput
#endif

#define item_radio_included

static Map:m_radioStats;

enum _:ItemRadioStats {
    Float:radioReliability,
    radioOccupiedSpace,
    Float:radioFrequency
};

stock Task:IRadio_Create(Float:reliability = 1.0, occupied_space = 2) {
    new Task:t = task_new();
    task_detech();

    using inline QueryFinished() {
        new insert_id = -1;
        if(cache_affected_rows()) {
            if(!map_valid(m_radioStats)) {
                m_radioStats = map_new();
            }

            insert_id = cache_insert_id();
            new stats[ItemRadioStats] = {1.0, 2, 0.0};
            map_set_arr(m_radioStats, insert_id, stats);

            new query[80 + 10 + 10 + 3 - 7 + 1];
            mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO `item_radio` (id, reliability, occupied_space) VALUES (%d, %d, %.4f)", insert_id, reliability, occupied_space);
            mysql_pquery(MySQL_Connection(), query);
            CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityRadio, reliability, occupied_space);
        }
    }
    MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", _:ITEM_ABILITY:iAbilityMelee, reliability, occupied_space);
	task_set_result_ms(t, -1, 2000);
    return t;
}

