#if defined farmer_employee_included
	#endinput
#endif

#define farmer_employee_included

static Map:m_farmerEmployees;

forward Farmer_OnEmployeeRetrieved(playerid);
public Farmer_OnEmployeeRetrieved(playerid) {
	if(cache_num_rows()) {
		if(!map_valid(m_farmerEmployees)) {
			m_farmerEmployees = map_new();
		}
		map_set(m_farmerEmployees, Player_GetId(playerid), 1);
	}
	return 1;
}

stock Farmer_HasEmployee(id) {
	return (map_valid(m_farmerEmployees) && map_has_key(m_farmerEmployees, id));
}

stock Farmer_AddEmployee(id) {
	new query[128];
	if(!map_valid(m_farmerEmployees)) {
		m_farmerEmployees = map_new();
	}
	map_set(m_farmerEmployees, id, 1);
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO farmer_employees(employee_id) VALUES (%d)", id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Farmer_DeleteEmployee(id) {
	if(!map_valid(m_farmerEmployees)) {
		return 0;
	}
	new query[128];
	map_remove(m_farmerEmployees, id);
	if(!map_size(m_farmerEmployees)) {
		map_delete(m_farmerEmployees);
		m_farmerEmployees = Map:-1;
	}
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM farmer_employees WHERE employee_id = %d LIMIT 1", id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Player_OnStatsRetrieved(playerid) {
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM farmer_employees WHERE employee_id = %d LIMIT 1", Player_GetId(playerid));
	mysql_pquery(MySQL_Connection(), query, "Farmer_OnEmployeeRetrieved", "i", playerid);
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS farmer_employees (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	employee_id INT UNSIGNED PRIMARY KEY,\
	FOREIGN KEY(employee_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
	if(map_valid(m_farmerEmployees) && map_has_key(m_farmerEmployees, Player_GetId(playerid))) {
		map_remove(m_farmerEmployees, Player_GetId(playerid));
		if(!map_size(m_farmerEmployees)) {
			map_delete(m_farmerEmployees);
			m_farmerEmployees = Map:-1;
		}
	}
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_farmerEmployees)) {
		map_delete(m_farmerEmployees);
		m_farmerEmployees = Map:-1;
	}
	return 1;
}