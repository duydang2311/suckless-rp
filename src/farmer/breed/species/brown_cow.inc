#if defined brown_cow_included
	#endinput
#endif

#define brown_cow_included

static Map:m_quality;

stock Float:BrownCow_GetQuality(id) {
	if(!map_valid(m_quality) || !map_has_key(m_quality, id)) {
		return 0.0;
	}
	return Float:map_get(m_quality, id);
}

stock BrownCow_UpdateQuality(id, Float:value) {
	if(!map_valid(m_quality) || !map_has_key(m_quality, id)) {
		return 0;
	}
	new Float:quality = Float:map_get(m_quality, id) + value;
	if(quality > 1.0) {
		quality = 1.0;
	} else if(quality < 0.0) {
		quality = 0.0;
	}
	map_set(m_quality, id, quality);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Breed_OnCreated(id) {
	switch(Breed_GetSpecies(id)) {
		case anmlBrownCow_1, anmlBrownCow_2: {
			if(!map_valid(m_quality)) {
				m_quality = map_new();
			}
			map_set(m_quality, id, 1.0);
			new query[128];
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO brown_cow_stats (id) VALUES (%d)", id);
			mysql_pquery(MySQL_Connection(), query);
		}
	}
	return 1;
}

hook Breed_OnRemoved(id) {
	if(map_valid(m_quality) && map_has_key(m_quality, id)) {
		map_remove(m_quality, id);
		if(!map_size(m_quality)) {
			map_delete(m_quality);
			m_quality = Map:-1;
		}
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS brown_cow_stats(\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	quality FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (id) REFERENCES breed_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}