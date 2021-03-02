#if defined player_stats_included
	#endinput
#endif

#define player_stats_included

enum _:PlayerStats {
	pSkin,
	pLevel,
	Timestamp:pBirthday
}

static Map:m_playerStats;

stock Player_GetStats(playerid, stats[PlayerStats]) {
	if(map_has_key(m_playerStats, playerid)) {
		return map_get_arr_safe(m_playerStats, playerid, stats);
	}
	return 0;
}

stock Player_GetSkin(playerid) {
	if(map_has_key(m_playerStats, playerid)) {
		new stats[PlayerStats];
		map_get_arr(m_playerStats, playerid, stats);
		return stats[pSkin];
	}
	return -1;
}

stock Player_GetLevel(playerid) {
	if(map_has_key(m_playerStats, playerid)) {
		new stats[PlayerStats];
		map_get_arr(m_playerStats, playerid, stats);
		return stats[pLevel];
	}
	return -1;
}

stock Player_SetupStats(playerid) {
	TogglePlayerSpectating(playerid, true);
	Dialog_Show(playerid, StatsSetupSkin, DIALOG_STYLE_INPUT, "Stats Setup | Skin", "Ban dang trong qua trinh setup thong tin nhan vat.\n> Nhap ID trang phuc cho nhan vat cua ban.", ">>", "<<");
	return 1;
}

Dialog:StatsSetupSkin(playerid, response, listitem, inputtext[]) {
	if(response) {
		extract inputtext -> new model; else {
			Dialog_Show(playerid, StatsSetupSkin, DIALOG_STYLE_INPUT, "Stats Setup | Skin", "Ban dang trong qua trinh setup thong tin nhan vat.\n> Nhap ID trang phuc cho nhan vat cua ban.", ">>", "<<");
			return 1;
		}

		if(model < 1 || model > 311 || model == 74) {
			Dialog_Show(playerid, StatsSetupSkin, DIALOG_STYLE_INPUT, "Stats Setup | Skin", "Ban dang trong qua trinh setup thong tin nhan vat.\n> Nhap ID trang phuc cho nhan vat cua ban.", ">>", "<<");
			return 1;
		}

		new stats[PlayerStats];
		stats[pSkin] = model;
		stats[pLevel] = 0;
		stats[pBirthday] = Timestamp:0;
		map_set_arr(m_playerStats, playerid, stats);
		Dialog_Show(playerid, StatsSetupBirthday, DIALOG_STYLE_INPUT, "Stats Setup | Birthday", "Ban dang trong qua trinh setup thong tin nhan vat.\n> Nhap ngay-thang-nam sinh cho nhan vat cua ban (vi du: 1-1-2020).", "Hoan tat", "<<");
	} else {
		Player_RequestAuthName(playerid);
	}
	return 1;
}

Dialog:StatsSetupBirthday(playerid, response, listitem, inputtext[]) {
	if(response) {
		new
			day,
			month,
			year;
		if(sscanf(inputtext, "p<->iii", day, month, year)) {
			Dialog_Show(playerid, StatsSetupBirthday, DIALOG_STYLE_INPUT, "Stats Setup | Birthday", "Ban dang trong qua trinh setup thong tin nhan vat.\n> Nhap ngay-thang-nam sinh cho nhan vat cua ban (vi du: 1-1-2020).\n\nBan can nhap ngay-thang-nam sinh theo cu phap nhu vi du.", "Hoan tat", "<<");
			return 1;
		}

		if(day < 1 || day > 31) {
			Dialog_Show(playerid, StatsSetupBirthday, DIALOG_STYLE_INPUT, "Stats Setup | Birthday", "Ban dang trong qua trinh setup thong tin nhan vat.\n> Nhap ngay-thang-nam sinh cho nhan vat cua ban (vi du: 1-1-2020).\n\nSo ngay khong hop le, gia tri hop le nam trong khoang [1, 31].", "Hoan tat", "<<");
			return 1;
		}

		if(month < 1 || month > 12) {
			Dialog_Show(playerid, StatsSetupBirthday, DIALOG_STYLE_INPUT, "Stats Setup | Birthday", "Ban dang trong qua trinh setup thong tin nhan vat.\n> Nhap ngay-thang-nam sinh cho nhan vat cua ban (vi du: 1-1-2020).\n\nSo thang khong hop le, gia tri hop le nam trong khoang [1, 12].", "Hoan tat", "<<");
			return 1;
		}

		if(year < 1950 || year > 2010) {
			Dialog_Show(playerid, StatsSetupBirthday, DIALOG_STYLE_INPUT, "Stats Setup | Birthday", "Ban dang trong qua trinh setup thong tin nhan vat.\n> Nhap ngay-thang-nam sinh cho nhan vat cua ban (vi du: 1-1-2020).\n\nSo nam khong hop le, gia tri hop le nam trong khoang [1950, 2010].", "Hoan tat", "<<");
			return 1;
		}

		new
			stats[PlayerStats],
			time[32];
		format(time, sizeof(time), "%d-%d-%d", year, month, day);
		map_get_arr(m_playerStats, playerid, stats);
		TimeParse(time, ISO6801_DATE, stats[pBirthday]);
		map_set_arr(m_playerStats, playerid, stats);
		inline const QueryFinished() {
			new name[MAX_PLAYER_NAME + 1];
			GetPlayerName(playerid, name);
			printf("INFO: Player %s (SQL ID %d) has completed their stats setup (%d affected rows).", name, Player_GetId(playerid), cache_affected_rows());
			Player_Spawn(playerid);
		}
		MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO player_stats (id, skin, birthday) VALUES (%d, %d, '%e')", Player_GetId(playerid), stats[pSkin], time);
	} else {
		if(map_has_key(m_playerStats, playerid)) {
			map_remove(m_playerStats, playerid);
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Player_OnSpawn(playerid) {
	inline const QueryFinished() {
		if(cache_num_rows()) {
			new
				stats[PlayerStats],
				Float:health,
				Float:armour;
			cache_get_value_name_int(0, "skin", stats[pSkin]);
			cache_get_value_name_int(0, "level", stats[pLevel]);
			cache_get_value_name_float(0, "health", health);
			cache_get_value_name_float(0, "armour", armour);

			SetPlayerHealth(playerid, health);
			SetPlayerArmour(playerid, armour);
			SetPlayerSkin(playerid, stats[pSkin]);
			SetPlayerScore(playerid, stats[pLevel]);

			map_set_arr(m_playerStats, playerid, stats);
		} else {
			Player_Msg(playerid, COLOR_YELLOW, "Nhan vat cua ban chua duoc setup, hay tien hanh setup nhan vat ngay bay gio.");
			Player_SetupStats(playerid);
			return Y_HOOKS_BREAK_RETURN_1;
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_stats WHERE id = %d LIMIT 1", Player_GetId(playerid));
	return 1;
}

hook OnGameModeInit() {
	m_playerStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	skin INT UNSIGNED DEFAULT 0,\
	health FLOAT(11, 4) DEFAULT 100.0000,\
	armour FLOAT(11, 4) DEFAULT 0.0000,\
	birthday DATE DEFAULT 0,\
	level TINYINT UNSIGNED DEFAULT 1,\
	FOREIGN KEY (id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}