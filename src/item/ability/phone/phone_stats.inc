#if defined phone_stats_included
	#endinput
#endif

#define phone_stats_included

enum _:PhoneModel {
	LS_Telefornica
}

enum _:ItemPhoneStats {
	iPhoneModel,
	Float:iPhonePower
}

static Map:m_phoneStats;

stock Task:IPhone_Create(model, Float:power = 1.0, Float:reliability = 1.0, occupieed_space = 1) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new insert_id = -1;
		if(cache_affected_rows()) {
			new
				stats[ItemPhoneStats],
				query[150];
			insert_id = cache_insert_id();
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_phone (id, model, power) VALUES (%d, %d, %.4f)", insert_id, model, power);
			mysql_pquery(MySQL_Connection(), query);

			stats[iPhonePower] = power;
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

hook OnGameModeInit@8() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS item_phone (\
		created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
		modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
		id INT UNSIGNED PRIMARY KEY,\
		model TINYINT UNSIGNED DEFAULT 0,\
		power FLOAT(11, 4) DEFAULT 1.0000,\
		FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}