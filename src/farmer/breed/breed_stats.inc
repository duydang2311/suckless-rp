#if defined breed_stats_included
	#endinput
#endif

#define breed_stats_included

enum _:BreedAnimalSpecies {
	anmlCow,
	anmlPig,
	anmlChicken
}

enum _:BreedAnimalStats {
	anmlNPCId,
	anmlSpecies,
	anmlBreederId,
	Float:anmlMaturity,
	Float:anmlHunger,
	Float:anmlThirst
}

static Map:m_animals;

stock Breed_GetAnimalSpeciesName(species, name[], size = sizeof(name)) {
	name[0] = EOS;
	switch(species) {
		case anmlCow: {
			strcat(name, "Cow", size);
			return 1;
		}
		case anmlPig: {
			strcat(name, "Pig", size);
			return 1;
		}
		case anmlChicken: {
			strcat(name, "Chicken", size);
			return 1;
		}
	}
	strcat(name, "Animal", size);
	return 0;
}

stock Breed_GetAnimalSpeciesModel(species) {
	switch(species) {
		case anmlCow: {
			return 20001;
		}
		case anmlPig: {
			return 20002;
		}
		case anmlChicken: {
			return 20003;
		}
	}
	return 0;
}

stock Task:Breed_CreateAnimal(species) {
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
				stats[anmlSpecies] = species;
				stats[anmlMaturity] = 0.0;
				stats[anmlHunger] = 0.3;
				stats[anmlThirst] = 0.3;
				if(!map_valid(m_animals)) {
					m_animals = map_new();
				}
				map_set_arr(m_animals, id, stats);
				FCNPC_Spawn(npcid, Breed_GetAnimalSpeciesModel(species), 0.0, 0.0, 2.0);
				Breed_LifeCycle(id);
				task_set_result(t, id);
			}
		} else {
			task_set_result(t, -1);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO breed_stats(species) VALUES (%d)", species);
	task_set_error_ms(t, amx_err_exit, 3000);
	return t;
}

stock Breed_RemoveAnimal(id) {
	if(!map_valid(m_animals)) {
		return 0;
	}
	if(map_has_key(m_animals, id)) {
		new stats[BreedAnimalStats];
		FCNPC_Destroy(stats[anmlNPCId]);
		mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM breed_stats WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query);
		map_remove(m_animals, id);
		return 1;
	}
	return 0;
}

stock Breed_HasId(id) {
	return (map_valid(m_animals) && map_has_key(m_animals, id));
}

stock Breed_GetNPCId(id) {
	if(!map_valid(m_animals) || !map_has_key(m_animals, id)) {
		return -1;
	}
	new stats[BreedAnimalStats];
	map_get_arr(m_animals, id, stats);
	return stats[anmlNPCId];
}

forward Breed_InitAnimal(id);
public Breed_InitAnimal(id) {
	if(!map_valid(m_animals) || !map_has_key(m_animals, id)) {
		return 0;
	}
	new
		stats[BreedAnimalStats],
		name[48];
	map_get_arr(m_animals, id, stats);
	if(stats[anmlNPCId] == -1) {
		Breed_GetAnimalSpeciesName(stats[anmlSpecies], name);
		format(name, sizeof(name), "%s_%d", name, id);
		stats[anmlNPCId] = FCNPC_Create(name);
		if(stats[anmlNPCId] != INVALID_PLAYER_ID) {
			map_set_cell(m_animals, id, anmlNPCId, stats[anmlNPCId]);
			FCNPC_Spawn(stats[anmlNPCId], Breed_GetAnimalSpeciesModel(stats[anmlSpecies]), 0.0, 0.0, 2.0);
			Breed_LifeCycle(id);
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
		m_animals = map_new();
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
			map_set_arr(m_animals, id, stats);
			SetPreciseTimer("Breed_InitAnimal", 1000, false, "i", id);
		}
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_animals)) {
		for(new Iter:it = map_iter(m_animals), stats[BreedAnimalStats]; iter_inside(it); iter_move_next(it)) {
			if(stats[anmlNPCId] != -1) {
				FCNPC_Destroy(stats[anmlNPCId]);
			}
		}
	}
	map_delete(m_animals);
}