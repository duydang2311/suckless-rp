#if defined player_event_connect_included
	#endinput
#endif

#define player_event_connect_included

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

Dialog:Auth_LoginDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		new name[MAX_PLAYER_NAME + 1];
		GetPlayerName(playerid, name);
		if(!Player_IsValidPassword(inputtext)) {
			Dialog_Show(playerid, Auth_LoginDialog, DIALOG_STYLE_PASSWORD, "Player | Login", sprintf("Tai khoan '%s' da duoc dang ky.\n> Nhap mat khau de tien hanh dang nhap.", name), "Dang nhap", "<<");
			return 1;
		}

		new password[64];
		strcopy(password, inputtext);
		inline const QueryFinished() {
			inline const Checked(bool:same) {
				if(same) {
					Player_OnLogin(playerid);
				} else {
					Dialog_Show(playerid, Auth_LoginDialog, DIALOG_STYLE_PASSWORD, "Player | Login", sprintf("Tai khoan '%s' da duoc dang ky.\n> Nhap mat khau de tien hanh dang nhap.\n\nMat khau khong dung, hay thu lai.", name), "Dang nhap", "<<");
				}
			}

			if(cache_num_rows()) {
				new hash[BCRYPT_HASH_LENGTH];
				cache_get_value_name(0, "hash", hash);
				BCrypt_CheckInline(password, hash, using inline Checked);
			} else {
				Dialog_Show(playerid, Auth_LoginDialog, DIALOG_STYLE_PASSWORD, "Player | Login", sprintf("Tai khoan '%s' da duoc dang ky.\n> Nhap mat khau de tien hanh dang nhap.\n\nDang nhap that bai, hay thu lai.", name), "Dang nhap", "<<");
			}
		}
		MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT hash FROM player_auth WHERE name = '%e' LIMIT 1", name);
	} else {
		Dialog_Show(playerid, Auth_NameRequestDialog, DIALOG_STYLE_INPUT, "Player | Name Request", "> Nhap mot ten nguoi choi ban muon dang nhap/dang ky.", "Tiep tuc", "Ngat ket noi");
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

		inline const Hashed(string:hash[]) {
			new name[MAX_PLAYER_NAME + 1];
			GetPlayerName(playerid, name);

			inline const Inserted() {
				new query[128];
				mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM player_auth WHERE name = '%e'", name);
				mysql_pquery(MySQL_Connection(), query, "Player_OnAuthenticated", "i", playerid);
			}
			MySQL_PQueryInline(MySQL_Connection(), using inline Inserted, "INSERT INTO player_auth (name, hash) VALUES ('%e', '%e')", name, hash);
		}
		BCrypt_HashInline(inputtext, 12, using inline Hashed);
	} else {
		Dialog_Show(playerid, Auth_NameRequestDialog, DIALOG_STYLE_INPUT, "Player | Name Request", "> Nhap mot ten nguoi choi ban muon dang nhap/dang ky.", "Tiep tuc", "Ngat ket noi");
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
	new
		name[MAX_PLAYER_NAME + 1],
		query[128];
	GetPlayerName(playerid, name);
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM player_auth WHERE name = '%e'", name);
	mysql_pquery(MySQL_Connection(), query, "Player_OnAuthenticated", "i", playerid);
	return 1;
}