#if defined item_sim_included
	#endinput
#endif

#define item_sim_included

const SIM_NUMBER_SIZE = 11;

enum _:ItemSimStats {
	iSimNumber[SIM_NUMBER_SIZE]
}

static
	Map:m_simStats, // { item_id: {ItemSimStats} }
	Map:m_phoneByNumber; // { {ItemSimStats}.iSimNumber: item phone id }

stock ISim_Has(id) {
	return (map_valid(m_simStats) && map_has_key(m_simStats, id));
}

stock ISim_GenerateNumber(number[], size = sizeof(number)) {
	number[0] = EOS;
	strcat(number, "0318", size);
	for(new i = strlen(number); i != (size - 1); i++) {
		number[i] = random(10) + '0';
	}
	number[size - 1] = EOS;
	return 1;
}

stock Task:ISim_Create(const number[] = "", Float:reliability = 1.0, occupied_space = 1) {
	new Task:t = task_new();
	task_detach();

	new generated_number[SIM_NUMBER_SIZE] = "";
	if(isnull(number)) {
		ISim_GenerateNumber(generated_number);
	} else {
		strcat(generated_number, number);
	}

	inline const QueryFinished() {
		new insert_id = -1;
		if(cache_affected_rows()) {
			new
				stats[ItemSimStats],
				query[128];
			insert_id = cache_insert_id();
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_sim (id, number) VALUES (%d, '%e')", insert_id, generated_number);
			mysql_pquery(MySQL_Connection(), query);

			stats[iSimNumber][0] = EOS;
			strcat(stats[iSimNumber], generated_number);
			if(!map_valid(m_simStats)) {
				m_simStats = map_new();
			}
			map_set_arr(m_simStats, insert_id, stats);
			CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilitySim, reliability, occupied_space);
		}
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", iAbilitySim, reliability, occupied_space);
	task_set_result_ms(t, -1, 2000);
	return t;
}

stock ISim_GetName(item_id, name[], size = sizeof(name)) {
	name[0] = EOS;
	strcat(name, "Sim Card", size);
	return 1;
}

stock ISim_GetModel() {
	return 1581;
}

stock ISim_GetPhoneWithNumber(const number[]) {
	if(!map_valid(m_phoneByNumber) || !map_has_str_key(m_phoneByNumber, number)) {
		return -1;
	}
	return map_str_get(m_phoneByNumber, number);
}

forward ISim_OnDeleted(id);
public ISim_OnDeleted(id) {
	if(map_valid(m_simStats) && map_has_key(m_simStats, id)) {
		map_remove(m_simStats, id);
		if(!map_size(m_simStats)) {
			map_delete(m_simStats);
			m_simStats = Map:-1;
		}
	}
	return 1;
}

hook OnGameModeInit@2() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_sim (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	number VARCHAR(11) DEFAULT '',\
	reliability FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}

hook OnGameModeExit@2() {
	if(map_valid(m_simStats)) {
		map_delete(m_simStats);
		m_simStats = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

forward ISim_OnStatsRetrieving(id, Task:t);
public ISim_OnStatsRetrieving(id, Task:t) {
	if(cache_num_rows()) {
		if(!map_valid(m_simStats)) {
			m_simStats = map_new();
		}
		new stats[ItemSimStats];
		cache_get_value_name(0, "number", stats[iSimNumber], SIM_NUMBER_SIZE);
		map_set_arr(m_simStats, id, stats);
	}
	if(task_valid(t)) {
		task_set_result(t, 1);
	}
	return 1;
}

hook Item_OnStatsRetrieved(id, Task:t) {
	if(Item_GetAbility(id) == iAbilitySim) {
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM item_sim WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query, "ISim_OnStatsRetrieving", "ii", id, _:t);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnModelGetAttempt(id, &model) {
	if(Item_GetAbility(id) == iAbilitySim) {
		model = ISim_GetModel();
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnNameGetAttempt(id, name[], size) {
	if(Item_GetAbility(id) == iAbilitySim) {
		ISim_GetName(id, name, size);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnAddedToGroup(id, group_id) {
	if(ISim_Has(id) && IPhone_Has(group_id)) { // if item sim is added to item phone
		if(!map_valid(m_phoneByNumber)) {
			m_phoneByNumber = map_new();
		}
		new stats[ItemSimStats];
		map_get_arr(m_simStats, id, stats);
		map_str_set(m_phoneByNumber, stats[iSimNumber], group_id);
	}
	return 1;
}