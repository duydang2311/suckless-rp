#if defined sunday_mysql_included
	#endinput
#endif

#define sunday_mysql_included

#include <a_samp>
#include <a_mysql>
#include <env>

static MySQL:mysql_conn = MySQL:MYSQL_INVALID_HANDLE;

stock MySQL:MySQL_Connection() {
	return mysql_conn;
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
	if(mysql_conn == MySQL:MYSQL_INVALID_HANDLE || mysql_errno(mysql_conn)) {
		print("ERROR: Failed to connect to database server.");
		SendRconCommand("exit");
		return Y_HOOKS_BREAK_RETURN_0;
	}

	print("INFO: Connected to database server successfully.");
	return 1;
}