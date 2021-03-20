#if defined farmer_breed_stats_included
	#endinput
#endif

#define farmer_breed_stats_included

enum _:BreedAnimalSpecies {
	anmlCow,
	anmlPig,
	anmlChicken
}

enum _:BreedAnimalStats {
	anmlId,
	anmlSpecies,
	anmlBreederId,
	Float:anmlMaturity,
	Float:anmlHunger,
	Float:anmlThirst
}

static
	Iterator:it_animals<MAX_BREED_ANIMALS>,
	LinkedList:ll_animals;

stock Breed_GetAnimalSpeciesName(species, name[], size = sizeof(name)) {
	name[0] = EOS;
	switch(species) {
		case anmlCow: {
			strcat(name, "Cow");
			return 1;
		}
		case anmlPig: {
			strcat(name, "Pig");
			return 1;
		}
		case anmlChicken: {
			strcat(name, "Chicken");
			return 1;
		}
	}
	strcat(name, "Animal");
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
			format(name, sizeof(name), "%s\#%d", name, id);
			new id = FCNPC_Create(name);
			if(id == INVALID_PLAYER_ID) {
				return -1;
			}
			new stats[BreedAnimalStats];
			stats[anmlId] = id;
			stats[anmlSpecies] = species;
			stats[anmlMaturity] = 0.0;
			stats[anmlHunger] = 0.3;
			stats[anmlThirst] = 0.3;
			if(!linked_list_valid(ll_animals)) {
				ll_animals = linked_list_new();
			}
			linked_list_add_arr(ll_animals, id);
			task_set_result(t, id);
		}
		task_set_result(t, -1);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO breed_stats(species) VALUES (%d)", species);
	task_set_error_ms(t, amx_err_exit, 3000);
	return t;
}

stock Breed_RemoveAnimal(id) {
	if(!linked_list_valid(ll_animals)) {
		return 0;
	}
	for(new Iter:it = linked_list_iter(ll_animals), stats[BreedAnimalStats]; iter_inside(it); iter_move_next(it)) {
		iter_get_arr(it, stats);
		if(stats[anmlId] == id) {
			new query[128];
			iter_erase(it);
			mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM breed_stats WHERE id = %d LIMIT 1", id);
			mysql_pquery(MySQL_Connection(), query);
			return 1;
		}
	}
	return 0;
}

stock Breed_HasAnimal(id) {
	if(!linked_list_valid(ll_animals)) {
		return 0;
	}
	for(new Iter:it = linked_list_iter(ll_animals), stats[BreedAnimalStats]; iter_inside(it); iter_move_next(it)) {
		iter_get_arr(it, stats);
		if(stats[anmlId] == id) {
			return 1;
		}
	}
	return 0;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS breed_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	species TINYINT UNSIGNED DEFAULT 0,\
	breeder_id INT UNSIGNED,\
	maturity FLOAT(11, 4) DEFAULT 0.0000,\
	hunger FLOAT(11, 4) DEFAULT 0.0000,\
	thirst FLOAT(11, 4) DEFAULT 0.0000,\
	FOREIGN KEY(breeder_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM breed_stats", true);
	for(new i = 0, rows = cache_num_rows(), stats[BreedAnimalStats], name[48], index; i != rows; i++) {
		cache_get_value_name_int(i, "id", stats[anmlId]);
		cache_get_value_name_int(i, "species", stats[anmlSpecies]);
		cache_get_value_name_int(i, "breeder_id", stats[anmlBreederId]);
		cache_get_value_name_float(i, "maturity", stats[anmlMaturity]);
		cache_get_value_name_float(i, "hunger", stats[anmlHunger]);
		cache_get_value_name_float(i, "thirst", stats[anmlThirst]);
		Breed_GetAnimalSpeciesName(stats[anmlSpecies], name);
		format(name, sizeof(name), "%s\#%d", name, stats[anmlId]);
		new id = FCNPC_Create(name);
		if(id != INVALID_PLAYER_ID) {
			if(!linked_list_valid(ll_animals)) {
				ll_animals = linked_list_new();
			}
			linked_list_add(ll_animals, stats);
		}
	}
	return 1;
}