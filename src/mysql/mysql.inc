#if defined sunday_mysql_included
	#endinput
#endif

#define sunday_mysql_included

#include <a_samp>
#include <a_mysql>
#include <env>
#include <samp-precise-timers>

static MySQL:mysql_conn = MySQL:MYSQL_INVALID_HANDLE;

stock MySQL:MySQL_Connection() {
	return mysql_conn;
}

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle) {
	printf("ERR: Source - OnQueryError [errorid: %d]", errorid);
	printf("ERR: Description - %s", error);
	printf("ERR: Callback - %s", callback);
	printf("ERR: Query - %s", query);
	return 1;
}

forward MySQL_OnGameModeExit();
public MySQL_OnGameModeExit() {
	mysql_close(mysql_conn);
	mysql_conn = MySQL:MYSQL_INVALID_HANDLE;
	return 1;
}

#include <YSI_Coding\y_hooks>

HOOK__ OnGameModeInit() {
	new host[32] = "",
		user[32] = "",
		password[32] = "",
		database[32] = "";

	if(Env_Has("MYSQL_HOST")) {
		Env_Get("MYSQL_HOST", host);
	}
	if(Env_Has("MYSQL_USER")) {
		Env_Get("MYSQL_USER", user);
	}
	if(Env_Has("MYSQL_PASSWORD")) {
		Env_Get("MYSQL_PASSWORD", password);
	}
	if(Env_Has("MYSQL_DATABASE")) {
		Env_Get("MYSQL_DATABASE", database);
	}

	new MySQLOpt:options = mysql_init_options();
	mysql_set_option(options, POOL_SIZE, 8);
	mysql_conn = mysql_connect(host, user, password, database, options);
	printf("oke - %s %s %s %s", host, user, password, database);
	if(mysql_conn == MySQL:MYSQL_INVALID_HANDLE || mysql_errno(mysql_conn)) {
		print("ERROR: Failed to connect to database server, try checking the environment variables.");
		SendRconCommand("exit");
		return Y_HOOKS_BREAK_RETURN_0;
	}

	print("INFO: Connected to database server successfully.");
	return 1;
}

hook OnGameModeExit() {
	SetPreciseTimer("MySQL_OnGameModeExit", 0, false);
	return 1;
}