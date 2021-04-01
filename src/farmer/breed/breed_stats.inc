#if defined breed_stats_included
	#endinput
#endif

#define breed_stats_included

enum _:BreedAnimalSpecies {
	anmlDiaryCow_1,
	anmlBrownCow_1,
	anmlDiaryCow_2,
	anmlBrownCow_2
}

enum _:BreedAnimalStats {
	anmlNPCId,
	anmlSpecies,
	anmlBreederId,
	Float:anmlMaturity,
	Float:anmlHunger,
	Float:anmlThirst
}

static
	Map:m_animalStats,
	Map:m_npcAnimalId,
	Map:m_animalFoodObject;

forward Breed_FinishEating(id);
public Breed_FinishEating(id) {
	if(map_valid(m_animalFoodObject) && map_has_key(m_animalFoodObject, id)) {
		DestroyDynamicObject(map_get(m_animalFoodObject, id));
		map_remove(m_animalFoodObject, id);
		if(!map_size(m_animalFoodObject)) {
			map_delete(m_animalFoodObject);
			m_animalFoodObject = Map:-1;
		}
	}

	if(map_valid(m_animalStats) && map_has_key(m_animalStats, id)) {
		new stats[BreedAnimalStats];
		map_get_arr(m_animalStats, id, stats);
		map_set_cell(m_animalStats, id, anmlHunger, (stats[anmlHunger] + 0.33) > 1.0 ? 1.0 : stats[anmlHunger] + 0.3);
	}
	Breed_Move(id);
	return 1;
}

stock Breed_UpdateHunger(id, Float:value) {
	if(!map_valid(m_animalStats) || !map_has_key(m_animalStats, id)) {
		return 0;
	}
	new stats[BreedAnimalStats];
	map_get_arr(m_animalStats, id, stats);
	stats[anmlHunger] += value;
	if(stats[anmlHunger] > 1.0) {
		stats[anmlHunger] = 1.0;
	}
	map_set_cell(m_animalStats, id, anmlHunger, stats[anmlHunger]);
	return 1;
}

stock Float:Breed_GetHunger(id) {
	if(!map_valid(m_animalStats) || !map_has_key(m_animalStats, id)) {
		return 0.0;
	}
	new stats[BreedAnimalStats];
	map_get_arr(m_animalStats, id, stats);
	return stats[anmlHunger];
}

stock Breed_GetAnimalSpeciesName(species, name[], size = sizeof(name)) {
	name[0] = EOS;
	switch(species) {
		case anmlDiaryCow_1, anmlDiaryCow_2: {
			strcat(name, "Diary_Cow", size);
			return 1;
		}
		case anmlBrownCow_1, anmlBrownCow_2: {
			strcat(name, "Brown_Cow", size);
			return 1;
		}
	}
	strcat(name, "Animal", size);
	return 0;
}

stock Breed_GetAnimalSpeciesModel(species) {
	switch(species) {
		case anmlDiaryCow_1: {
			return 20001;
		}
		case anmlBrownCow_1: {
			return 20002;
		}
		case anmlDiaryCow_2: {
			return 20003;
		}
		case anmlBrownCow_2: {
			return 20004;
		}
	}
	return 0;
}

stock Task:Breed_CreateAnimal(species, breeder_id = -1) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		if(cache_affected_rows()) {
			new
				name[48],
				id = cache_insert_id();
			Breed_GetAnimalSpeciesName(species, name);
			format(name, sizeof(name), "%s_%d", name, id);
			new npcid = FCNPC_Create(name);
			if(npcid == INVALID_PLAYER_ID) {
				task_set_result(t, -1);
			} else {
				new stats[BreedAnimalStats];
				stats[anmlNPCId] = npcid;
				stats[anmlBreederId] = breeder_id;
				stats[anmlSpecies] = species;
				stats[anmlMaturity] = 0.0;
				stats[anmlHunger] = 0.3;
				stats[anmlThirst] = 0.3;
				if(!map_valid(m_animalStats)) {
					m_animalStats = map_new();
				}
				map_set_arr(m_animalStats, id, stats);
				if(!map_valid(m_npcAnimalId)) {
					m_npcAnimalId = map_new();
				}
				map_set(m_npcAnimalId, npcid, id);
				switch(species) {
					case anmlBrownCow_1, anmlBrownCow_2: {
						CallLocalFunction("BrownCow_OnCreated", "i", id);
					}
					case anmlDiaryCow_1, anmlDiaryCow_2: {
						CallLocalFunction("DiaryCow_OnCreated", "i", id);
					}
				}
				SetPreciseTimer("Breed_Spawn", 0, false, "i", id);
				task_set_result(t, id);
			}
		} else {
			task_set_result(t, -1);
		}
	}
	if(breeder_id == -1) {
		MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO breed_stats(species) VALUES (%d)", species);
	} else {
		MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO breed_stats(species, breeder_id) VALUES (%d, %d)", species, breeder_id);
	}
	task_set_result_ms(t, -1, 2000);
	return t;
}

stock Breed_RemoveAnimal(id) {
	if(!map_valid(m_animalStats)) {
		return 0;
	}
	if(map_has_key(m_animalStats, id)) {
		new
			stats[BreedAnimalStats],
			query[128];
		map_get_arr(m_animalStats, id, stats);
		FCNPC_Destroy(stats[anmlNPCId]);
		mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM breed_stats WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query);
		map_remove(m_animalStats, id);
		if(!map_size(m_animalStats)) {
			map_delete(m_animalStats);
			m_animalStats = Map:-1;
		}
		if(map_has_key(m_npcAnimalId, stats[anmlNPCId])) {
			map_remove(m_npcAnimalId, stats[anmlNPCId]);
			if(!map_size(m_npcAnimalId)) {
				map_delete(m_npcAnimalId);
				m_npcAnimalId = Map:-1;
			}
		}
		switch(stats[anmlSpecies]) {
			case anmlBrownCow_1, anmlBrownCow_2: {
				CallLocalFunction("BrownCow_OnRemoved", "i", id);
			}
			case anmlDiaryCow_1, anmlDiaryCow_2: {
				CallLocalFunction("DiaryCow_OnRemoved", "i", id);
			}
		}
		return 1;
	}
	return 0;
}

stock Breed_HasId(id) {
	return (map_valid(m_animalStats) && map_has_key(m_animalStats, id));
}

stock Breed_GetBreederId(id) {
	if(!map_valid(m_animalStats) || !map_has_key(m_animalStats, id)) {
		return -1;
	}

	new stats[BreedAnimalStats];
	map_get_arr(m_animalStats, id, stats);
	return stats[anmlBreederId];
}

stock Breed_GetNPCId(id) {
	if(!map_valid(m_animalStats) || !map_has_key(m_animalStats, id)) {
		return -1;
	}
	new stats[BreedAnimalStats];
	map_get_arr(m_animalStats, id, stats);
	return stats[anmlNPCId];
}

stock Breed_GetIdByNPCId(npcid) {
	if(!map_valid(m_npcAnimalId) || !map_has_key(m_npcAnimalId, npcid)) {
		return -1;
	}
	return map_get(m_npcAnimalId, npcid);
}

stock Breed_Eat(id) {
	if(!map_valid(m_animalStats) || !map_has_key(m_animalStats, id)) {
		return 0;
	}
	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}

	new
		Float:x,
		Float:y,
		Float:z,
		Float:angle;
	GetPlayerFacingAngle(npcid, angle);
	FCNPC_GetPosition(npcid, x, y, z);
	Util_GetPointInFront2D(x, y, angle, 1.1, x, y);
	FCNPC_Stop(npcid);
	if(!map_valid(m_animalFoodObject)) {
		m_animalFoodObject = map_new();
	}
	map_set(m_animalFoodObject, id, CreateDynamicObject(2901, x, y, z, 0.0, 0.0, angle, FCNPC_GetVirtualWorld(npcid), FCNPC_GetInterior(npcid)));
	SetPreciseTimer("Breed_FinishEating", 5000, false, "i", id);
	return 1;
}

stock Breed_GetFoodObjectId(id) {
	if(!map_valid(m_animalFoodObject) || !map_has_key(m_animalFoodObject, id)) {
		return -1;
	}
	return map_get(m_animalFoodObject, id);
}

stock Breed_GetSpecies(id) {
	if(!map_valid(m_animalStats) || !map_has_key(m_animalStats, id)) {
		return -1;
	}
	new stats[BreedAnimalStats];
	map_get_arr(m_animalStats, id, stats);
	return stats[anmlSpecies];
}

forward Breed_InitAnimal(id);
public Breed_InitAnimal(id) {
	if(!map_valid(m_animalStats) || !map_has_key(m_animalStats, id)) {
		return 0;
	}
	new
		stats[BreedAnimalStats],
		name[48];
	map_get_arr(m_animalStats, id, stats);
	if(stats[anmlNPCId] == -1) {
		Breed_GetAnimalSpeciesName(stats[anmlSpecies], name);
		format(name, sizeof(name), "%s_%d", name, id);
		stats[anmlNPCId] = FCNPC_Create(name);
		if(stats[anmlNPCId] != INVALID_PLAYER_ID) {
			if(!map_valid(m_npcAnimalId)) {
				m_npcAnimalId = map_new();
			}
			map_set(m_npcAnimalId, stats[anmlNPCId], id);
			map_set_cell(m_animalStats, id, anmlNPCId, stats[anmlNPCId]);
			FCNPC_Spawn(stats[anmlNPCId], Breed_GetAnimalSpeciesModel(stats[anmlSpecies]), 0.0, 0.0, 2.0);
			Breed_Move(id);
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS breed_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	species TINYINT UNSIGNED DEFAULT 0,\
	breeder_id INT UNSIGNED NULL,\
	maturity FLOAT(11, 4) DEFAULT 0.0000,\
	hunger FLOAT(11, 4) DEFAULT 0.0000,\
	thirst FLOAT(11, 4) DEFAULT 0.0000,\
	FOREIGN KEY(breeder_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM breed_stats", true);
	new rows = cache_num_rows();
	if(rows) {
		m_animalStats = map_new();
		for(new i = 0, stats[BreedAnimalStats], id, bool:field_null; i != rows; i++) {
			cache_get_value_name_int(i, "id", id);
			cache_get_value_name_int(i, "species", stats[anmlSpecies]);
			cache_is_value_name_null(i, "breeder_id", field_null);
			if(!field_null) {
				cache_get_value_name_int(i, "breeder_id", stats[anmlBreederId]);
			} else {
				stats[anmlBreederId] = -1;
			}
			cache_get_value_name_float(i, "maturity", stats[anmlMaturity]);
			cache_get_value_name_float(i, "hunger", stats[anmlHunger]);
			cache_get_value_name_float(i, "thirst", stats[anmlThirst]);
			stats[anmlNPCId] = -1;
			map_set_arr(m_animalStats, id, stats);
			SetPreciseTimer("Breed_InitAnimal", 1000, false, "i", id);
		}
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_animalStats)) {
		for(new Iter:it = map_iter(m_animalStats), stats[BreedAnimalStats]; iter_inside(it); iter_move_next(it)) {
			if(stats[anmlNPCId] != -1) {
				FCNPC_Destroy(stats[anmlNPCId]);
			}
		}
		map_delete(m_animalStats);
	}
	return 1;
}