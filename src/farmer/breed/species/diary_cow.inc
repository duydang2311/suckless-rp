#if defined diary_cow_included
	#endinput
#endif

#define diary_cow_included

static Map:m_milk;

stock Float:DiaryCow_GetMilk(id) {
	if(!map_valid(m_milk) || !map_has_key(m_milk, id)) {
		return 0.0;
	}
	return Float:map_get(m_milk, id);
}

stock DiaryCow_UpdateMilk(id, Float:value) {
	if(!map_valid(m_milk) || !map_has_key(m_milk, id)) {
		return 0;
	}
	new Float:milk = Float:map_get(m_milk, id) + value;
	if(milk > 1.0) {
		milk = 1.0;
	} else if(milk < 0.0) {
		milk = 0.0;
	}
	map_set(m_milk, id, milk);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Breed_OnCreated(id) {
	switch(Breed_GetSpecies(id)) {
		case anmlDiaryCow_1, anmlDiaryCow_2: {
			if(!map_valid(m_milk)) {
				m_milk = map_new();
			}
			map_set(m_milk, id, 0.0);
			new query[128];
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO diary_cow_stats (id) VALUES (%d)", id);
			mysql_pquery(MySQL_Connection(), query);
		}
	}
	return 1;
}

hook Breed_OnRemoved(id) {
	if(map_valid(m_milk) && map_has_key(m_milk, id)) {
		map_remove(m_milk, id);
		if(!map_size(m_milk)) {
			map_delete(m_milk);
			m_milk = Map:-1;
		}
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS diary_cow_stats(\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	milk FLOAT(11, 4) DEFAULT 0.0000,\
	FOREIGN KEY (id) REFERENCES breed_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}