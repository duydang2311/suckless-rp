#if defined player_auth_included
	#endinput
#endif

#define player_auth_included

static Map:m_playerId;

forward PlayerAuth_OnDisconnected(playerid);
public PlayerAuth_OnDisconnected(playerid) {
	if(map_valid(m_playerId) && map_has_key(m_playerId, playerid)) {
		map_remove(m_playerId, playerid);
	}
	return 1;
}

forward Player_OnAuthRetrieving(playerid);
public Player_OnAuthRetrieving(playerid) {
	if(cache_num_rows()) {
		new id;
		cache_get_value_name_int(0, "id", id);
		map_set(m_playerId, playerid, id);
		CallLocalFunction("Player_OnAuthRetrieved", "i", playerid);
	} else {
		new
			query[128],
			name[MAX_PLAYER_NAME + 1];

		Player_Msg(playerid, COLOR_LIGHTYELLOW, "Dang nhap khong thanh cong, xin hay thu lai.");
		GetPlayerName(playerid, name);
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM player_auth WHERE name = '%e' LIMIT 1", name);
		mysql_pquery(MySQL_Connection(), query, "Player_OnAuthenticated", "i", playerid);
	}
	return 1;
}

forward Player_OnAuthenticated(playerid);
public Player_OnAuthenticated(playerid) {
	new
		count = 0,
		name[MAX_PLAYER_NAME + 1];
	cache_get_value_name_int(0, "COUNT(*)", count);
	GetPlayerName(playerid, name);
	if(count) {
		Dialog_Show(playerid, Auth_LoginDialog, DIALOG_STYLE_PASSWORD, "Player | Login", sprintf("Tai khoan '%s' da duoc dang ky.\n> Nhap mat khau de tien hanh dang nhap.", name), "Dang nhap", "<<");
	} else {
		Dialog_Show(playerid, Auth_RegisterDialog, DIALOG_STYLE_PASSWORD, "Player | Register", sprintf("Tai khoan '%s' chua duoc dang ky.\n> Nhap mat khau de tien hanh dang ky.", name), "Dang ky", "<<");
	}
	return 1;
}

forward Player_OnPasswordHashed(playerid);
public Player_OnPasswordHashed(playerid) {
	new 
		hash[BCRYPT_HASH_LENGTH], name[MAX_PLAYER_NAME + 1],
		query[128];

	bcrypt_get_hash(hash);
	GetPlayerName(playerid, name);
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO player_auth (name, hash) VALUES ('%e', '%e')", name, hash);
	mysql_pquery(MySQL_Connection(), query, "Player_OnAuthRegistered", "is", playerid, name);
	return 1;
}

forward Player_OnAuthRegistered(playerid, name[]);
public Player_OnAuthRegistered(playerid, name[]) {
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM player_auth WHERE name = '%e'", name);
	mysql_pquery(MySQL_Connection(), query, "Player_OnAuthenticated", "i", playerid);
	return 1;
}

forward Player_OnHashRetrieving(playerid, const raw[]);
public Player_OnHashRetrieving(playerid, const raw[]) {
	if(cache_num_rows()) {
		new hash[BCRYPT_HASH_LENGTH];
		cache_get_value_name(0, "hash", hash);
		bcrypt_verify(playerid, "Player_OnHashVerified", raw, hash);
	} else {
		new name[MAX_PLAYER_NAME + 1];
		GetPlayerName(playerid, name);
		Dialog_Show(playerid, Auth_LoginDialog, DIALOG_STYLE_PASSWORD, "Player | Login", sprintf("Tai khoan '%s' da duoc dang ky.\n> Nhap mat khau de tien hanh dang nhap.\n\nDang nhap that bai, hay thu lai.", name), "Dang nhap", "<<");
	}
	return 1;
}

forward Player_OnHashVerified(playerid, bool:success);
public Player_OnHashVerified(playerid, bool:success) {
	if(success) {
		Player_Login(playerid);
	} else {
		new name[MAX_PLAYER_NAME + 1];
		GetPlayerName(playerid, name);
		Dialog_Show(playerid, Auth_LoginDialog, DIALOG_STYLE_PASSWORD, "Player | Login", sprintf("Tai khoan '%s' da duoc dang ky.\n> Nhap mat khau de tien hanh dang nhap.\n\nMat khau khong dung, hay thu lai.", name), "Dang nhap", "<<");
	}
	return 1;
}

stock Player_GetId(playerid) {
	if(map_valid(m_playerId) && map_has_key(m_playerId, playerid)) {
		return map_get(m_playerId, playerid);
	}
	return -1;
}

stock Player_GetById(id) {
	if(map_valid(m_playerId)) {
		foreach(new i : Player) {
			if(map_has_key(m_playerId, i) && map_get(m_playerId, i) == id) {
				return i;
			}
		}
	}
	return -1;
}

stock Player_RequestAuthName(playerid) {
	if(map_valid(m_playerId) && map_has_key(m_playerId, playerid)) {
		map_remove(m_playerId, playerid);
	}
	Dialog_Show(playerid, Auth_NameRequestDialog, DIALOG_STYLE_INPUT, "Player | Name Request", "> Nhap mot ten nguoi choi ban muon dang nhap/dang ky.", "Tiep tuc", "Ngat ket noi");
	return 1;
}

stock Player_Login(playerid) {
	new
		name[MAX_PLAYER_NAME + 1],
		query[128];
	GetPlayerName(playerid, name);
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT id FROM player_auth WHERE name = '%e' LIMIT 1", name);
	mysql_pquery(MySQL_Connection(), query, "Player_OnAuthRetrieving", "i", playerid);
	return 1;
}

Dialog:Auth_LoginDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		new name[MAX_PLAYER_NAME + 1];
		GetPlayerName(playerid, name);
		if(!Player_IsValidPassword(inputtext)) {
			Dialog_Show(playerid, Auth_LoginDialog, DIALOG_STYLE_PASSWORD, "Player | Login", sprintf("Tai khoan '%s' da duoc dang ky.\n> Nhap mat khau de tien hanh dang nhap.", name), "Dang nhap", "<<");
			return 1;
		}

		new
			password[64],
			query[128];
		strcopy(password, inputtext);
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT hash FROM player_auth WHERE name = '%e' LIMIT 1", name);
		mysql_pquery(MySQL_Connection(), query, "Player_OnHashRetrieving", "is", playerid, password);
	} else {
		Player_RequestAuthName(playerid);
	}
	return 1;
}

Dialog:Auth_RegisterDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(!Player_IsValidPassword(inputtext)) {
			new name[MAX_PLAYER_NAME + 1];
			GetPlayerName(playerid, name);
			Dialog_Show(playerid, Auth_RegisterDialog, DIALOG_STYLE_PASSWORD, "Player | Register", sprintf("Tai khoan '%s' chua duoc dang ky.\n> Nhap mat khau de tien hanh dang ky.\n\nMat khau khong duoc it hon 8 ky tu hoac lon hon 64 ky tu.", name), "Dang ky", "<<");
			return 1;
		}

		Player_Msg(playerid, COLOR_FADE2, "Vui long doi trong giay lat, he thong dang ma hoa mat khau cua ban.");
		bcrypt_hash(playerid, "Player_OnPasswordHashed", inputtext, 12);
	} else {
		Player_RequestAuthName(playerid);
	}
	return 1;
}

Dialog:Auth_NameRequestDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(!Player_IsValidName(inputtext)) {
			Dialog_Show(playerid, Auth_NameRequestDialog, DIALOG_STYLE_INPUT, "Player | Name Request", "> Nhap mot ten nguoi choi ban muon dang nhap/dang ky.\n\nTen nguoi choi vua nhap khong hop le, bat buoc tuan thu cu phap Ten_Ho.", "Tiep tuc", "Ngat ket noi");
			return 1;
		}

		new name[MAX_PLAYER_NAME + 1];
		foreach(new i : Player) {
			GetPlayerName(i, name);
			if(!strcmp(name, inputtext, true)) {
				Dialog_Show(playerid, Auth_NameRequestDialog, DIALOG_STYLE_INPUT, "Player | Name Request", sprintf("> Nhap mot ten nguoi choi ban muon dang nhap/dang ky.\nTen nguoi choi '%s' dang online, hay nhap ten khac.", name), "Tiep tuc", "Ngat ket noi");
				return 1;
			}
		}

		new query[128];
		SetPlayerName(playerid, inputtext);
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM player_auth WHERE name = '%e'", inputtext);
		mysql_pquery(MySQL_Connection(), query, "Player_OnAuthenticated", "i", playerid);
	} else {
		Player_KickMsg(playerid, "Ban da ngat ket noi voi may chu.");
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerConnect(playerid) {
	if(IsPlayerNPC(playerid)) {
		printf("%d is npc yes", playerid);
		return 1;
	}
	new
		name[MAX_PLAYER_NAME + 1],
		query[128];
	GetPlayerName(playerid, name);
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM player_auth WHERE name = '%e'", name);
	mysql_pquery(MySQL_Connection(), query, "Player_OnAuthenticated", "i", playerid);
	return 1;
}

hook OnPlayerDisconnect(playerid) {
	SetPreciseTimer("PlayerAuth_OnDisconnected", 0, false, "i", playerid);
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_playerId)) {
		map_delete(m_playerId);
		m_playerId = Map:-1;
	}
	return 1;
}

hook OnGameModeInit() {
	m_playerId = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_auth (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,\
	name VARCHAR(33) UNIQUE DEFAULT '',\
	hash VARCHAR(61) DEFAULT ''\
	);", false);
	return 1;
}
