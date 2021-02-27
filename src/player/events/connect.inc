#include <a_samp>
#include <easyDialog>

#include <mysql>
#include <player_utility>

static Map:m_authName;

forward Player_OnAuthenticated(playerid);
public Player_OnAuthenticated(playerid) {
	new count = 0;
	cache_get_value_name_int(0, "COUNT(*)", count);
	new name[MAX_PLAYER_NAME + 1];
	GetPlayerName(playerid, name);
	if(count) {
		Dialog_Show(playerid, Auth_LoginDialog, DIALOG_STYLE_PASSWORD, "Player | Login", sprintf("Tai khoan '%s' da duoc dang ky.\n> Nhap mat khau de tien hanh dang nhap.", name), "Dang nhap", "<<");
	} else {
		Dialog_Show(playerid, Auth_RegisterDialog, DIALOG_STYLE_PASSWORD, "Player | Register", sprint("Tai khoan '%s' chua duoc dang ky.\n> Nhap mat khau de tien hanh dang ky.", name), "Dang ky", "<<");
	}
	return 1;
}

Dialog:Auth_LoginDialog(playerid, response, listitem, inputtext[]) {
	if(response) {

	} else {
		Dialog_Show(playerid, Auth_NameRequestDialog, DIALOG_STYLE_INPUT, "Player | Name Request", "> Nhap mot ten nguoi choi ban muon dang nhap/dang ky.", "Tiep tuc", "Ngat ket noi");
	}
	return 1;
}

Dialog:Auth_NameRequestDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(!Player_IsValidName(inputtext)) {
			Dialog_Show(playerid, Auth_NameRequestDialog, DIALOG_STYLE_INPUT, "Player | Name Request", "> Nhap mot ten nguoi choi ban muon dang nhap/dang ky.", "Tiep tuc", "Ngat ket noi");
			return 1;
		}

		map_set_str(m_authName, playerid, inputtext);
		mysql_pquery(MySQL_Connection(), sprintf("SELECT COUNT(*) FROM player_auth WHERE name = '%e' LIMIT 1", name), "Player_OnAuthenticated", "i", playerid);
	} else {
		Player_KickMsg(playerid, "Ban da ngat ket noi voi may chu.");
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerConnect(playerid) {
	if(!map_valid(m_authName)) {
		m_authName = map_new();
	}

	new name[MAX_PLAYER_NAME + 1];
	GetPlayerName(playerid, name);
	map_set_str(m_authName, playerid, name);
	mysql_pquery(MySQL_Connection(), sprintf("SELECT COUNT(*) FROM player_auth WHERE name = '%e' LIMIT 1", name), "Player_OnAuthenticated", "i", playerid);
	return 1;
}