#if defined faction_cmds_included
	#endinput
#endif

#define faction_cmds_included

static Map:m_factionEditId;

flags:afac(ADMIN_FLAGS_4 | ADMIN_FLAGS_5)
CMD:afac(playerid, params) {
	new
		Iter:it = Faction_GetIter(),
		string[1024] = "ID\tTen Faction";
	if(iter_valid(it)) {
		new fac[FactionStats];
		for(; iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, fac);
			if(isnull(fac[facName])) {
				format(string, sizeof(string), "%s\n%d\t%s", string, iter_get_key(it), "Empty");
			} else {
				format(string, sizeof(string), "%s\n%d\t%s", string, iter_get_key(it), fac[facName]);
			}
		}
		iter_release(it);
	}
	strcat(string, "\n> Tao mot faction moi.");
	Dialog_Show(playerid, FactionListDialog, DIALOG_STYLE_TABLIST_HEADERS, "Faction Management", string, ">>", "<<");
	return 1;
}
alias:afac("afaction")

Dialog:FactionListDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		new count = Faction_Count();
		if(listitem == count) {
			Dialog_Show(playerid, FactionTypeDialog, DIALOG_STYLE_LIST, "Faction Management | Create", "Legal\nIllegal\n> Chon loai faction ban muon tao.", ">>", "<<");
		} else {
			new Iter:it = Faction_GetIter(listitem);
			if(iter_valid(it)) {
				new
					fac[FactionStats],
					string[128];
				iter_get_arr(it, fac);
				format(string, sizeof(string),
				"Name: %s\n\
				Type: %s\n\
				Ranks\n\
				Divisions",
				fac[facName],
				fac[facType] ? "Legal" : "Illegal");
				Dialog_Show(playerid, FactionManage, DIALOG_STYLE_LIST, "Faction Manage", string, ">>", "<<");
				if(!map_valid(m_factionEditId)) {
					m_factionEditId = map_new();
				}
				map_set(m_factionEditId, playerid, iter_get_key(it));
			} else {
				PC_EmulateCommand(playerid, "/afac");
			}
		}
	}
	return 1;
}

Dialog:FactionTypeDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(listitem > 1) {
			Dialog_Show(playerid, FactionTypeDialog, DIALOG_STYLE_LIST, "Faction Management | Create", "Legal\nIllegal\n> Chon loai faction ban muon tao.", ">>", "<<");
			return 1;
		}
		task_unblock(1);
		new id = await Faction_Create(bool:listitem);
		new string[128];
		format(string, sizeof(string), "INFO: Faction ID %d da duoc tao.", id);
		Player_Msg(playerid, COLOR_GREEN, string);
	}
	PC_EmulateCommand(playerid, "/afac");
	return 1;
}

Dialog:FactionManage(playerid, response, listitem, inputtext[]) {
	if(response) {
		switch(listitem) {
			case 0: { // Name
				Dialog_Show(playerid, FactionNameManage, DIALOG_STYLE_INPUT, "Faction Manage | Name", "> Nhap ten moi cho Faction nay.", "Xac nhan", "<<");
			}
			case 1: { // Type
				if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
					new id = map_get(m_factionEditId, playerid);
					if(Faction_Valid(id)) {
						new
							Iter:it = Iter:Faction_GetIterByKey(id),
							fac[FactionStats],
							string[128];
						iter_get_arr(it, fac);
						format(string, sizeof(string),
						"Name: %s\n\
						Type: %s\n\
						Ranks\n\
						Divisions",
						fac[facName],
						fac[facType] ? "Legal" : "Illegal");
						Dialog_Show(playerid, FactionManage, DIALOG_STYLE_LIST, "Faction Manage", string, ">>", "<<");
						iter_release(it);
					}
				}
				Player_Msg(playerid, COLOR_YELLOW, "<!>: Type cua Faction la thong tin khong the dieu chinh.");
			}
		}
		return 1;
	}
	if(map_valid(m_factionEditId)) {
		if(map_has_key(m_factionEditId, playerid)) {
			map_remove(m_factionEditId, playerid);
			if(!map_size(m_factionEditId)) {
				map_delete(m_factionEditId);
			}
		}
	}
	PC_EmulateCommand(playerid, "/afac");
	return 1;
}

Dialog:FactionNameManage(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(isnull(inputtext)) {
			Dialog_Show(playerid, FactionNameManage, DIALOG_STYLE_INPUT, "Faction Manage | Name", "> Nhap ten moi cho Faction nay.", "Xac nhan", "<<");
			return 1;
		}

		if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
			new id = map_get(m_factionEditId, playerid);
			Faction_SetName(id, inputtext);
		}
	}
	if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
		new id = map_get(m_factionEditId, playerid);
		if(Faction_Valid(id)) {
			new
				Iter:it = Iter:Faction_GetIterByKey(id),
				fac[FactionStats],
				string[128];
			iter_get_arr(it, fac);
			format(string, sizeof(string),
			"Name: %s\n\
			Type: %s\n\
			Ranks\n\
			Divisions",
			fac[facName],
			fac[facType] ? "Legal" : "Illegal");
			Dialog_Show(playerid, FactionManage, DIALOG_STYLE_LIST, "Faction Manage", string, ">>", "<<");
			iter_release(it);
		}
	} else {
		PC_EmulateCommand(playerid, "/afac");
	}
	return 1;
}