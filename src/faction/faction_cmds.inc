#if defined faction_cmds_included
	#endinput
#endif

#define faction_cmds_included

static
	Map:m_factionEditId,
	Map:m_factionRankEditOrder,
	Map:m_factionDivEditOrder;

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
				format(string, sizeof(string), "%s\n%d\tEmpty", string, iter_get_key(it));
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

static faction_manage_show(playerid) {
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

static faction_rank_manage_show(playerid) {
	if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
		new id = map_get(m_factionEditId, playerid),
			Iter:rank_it = Faction_GetRankIterByKey(id);
		new string[1024] = "Thu tu\tTen Rank";
		if(iter_valid(rank_it)) {
			new
				name[32],
				LinkedList:ll = LinkedList:iter_get(rank_it);
			for(new Iter:it = linked_list_iter(ll), order = 1; iter_inside(it); iter_move_next(it)) {
				iter_get_str(it, name);
				if(isnull(name)) {
					format(string, sizeof(string), "%s\n%d\tEmpty", string, order++);
				} else {
					format(string, sizeof(string), "%s\n%d\t%s", string, order++, name);
				}
			}
		}
		strcat(string, "\n>\tThem mot rank moi.");
		Dialog_Show(playerid, FactionRankManage, DIALOG_STYLE_TABLIST_HEADERS, "Faction Manage | Rank", string, ">>", "<<");
		return 1;
	}
	return 0;
}

static faction_division_manage_show(playerid) {
	if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
		new
			Iter:div_it = Faction_GetDivisionIterByKey(map_get(m_factionEditId, playerid)),
			string[1024] = "Thu tu\tTen don vi";
		if(iter_valid(div_it)) {
			new
				name[32],
				LinkedList:ll = LinkedList:iter_get(div_it);
			for(new Iter:it = linked_list_iter(ll), order = 0; iter_inside(it); iter_move_next(it)) {
				iter_get_str(it, name);
				format(string, sizeof(string), "%s\n%d\t%s", string, order++, name);
			}
		}
		strcat(string, "\n>\tTao mot don vi moi.");
		Dialog_Show(playerid, FactionDivisionManage, DIALOG_STYLE_TABLIST_HEADERS, "Faction Manage | Division", string, ">>", "<<");
		return 1;
	}
	return 0;
}

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
		new id = await Faction_Create(!bool:listitem);
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
				return 1;
			}
			case 1: { // Type
				Player_Msg(playerid, COLOR_YELLOW, "<!>: Type cua Faction la thong tin khong the dieu chinh.");
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
						return 1;
					}
				}
			}
			case 2: { // Ranks
				if(faction_rank_manage_show(playerid)) {
					return 1;
				}
			}
			case 3: { // Divisions
				if(faction_division_manage_show(playerid)) {
					return 1;
				}
			}
		}
	}
	if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
		map_remove(m_factionEditId, playerid);
		if(!map_size(m_factionEditId)) {
			map_delete(m_factionEditId);
			m_factionEditId = Map:-1;
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
	faction_manage_show(playerid);
	return 1;
}

Dialog:FactionRankManage(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
			new
				id = map_get(m_factionEditId, playerid),
				count = Faction_CountRanks(id);
			if(!map_valid(m_factionRankEditOrder)) {
				m_factionRankEditOrder = map_new();
			}
			if(listitem == count) { // Add a new rank
				Dialog_Show(playerid, FactionRankAdd, DIALOG_STYLE_INPUT, "Faction Manage | Rank | Add", "> Nhap ten chuc vu ban muon them.", "Xac nhan", "<<");
			} else { // Edit/Delete selected rank
				Dialog_Show(playerid, FactionRankActionList, DIALOG_STYLE_LIST, "Faction Manage | Rank | Action", "Chinh sua ten chuc vu\nXoa chuc vu", ">>", "<<");
				map_set(m_factionRankEditOrder, playerid, listitem);
			}
			return 1;
		}
	}
	faction_manage_show(playerid);
	return 1;
}

Dialog:FactionRankAdd(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(isnull(inputtext) || strlen(inputtext) > 31) {
			Dialog_Show(playerid, FactionRankAdd, DIALOG_STYLE_INPUT, "Faction Manage | Rank | Add", "> Nhap ten chuc vu ban muon them.", "Xac nhan", "<<");
			return 1;
		}

		if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
			new id = map_get(m_factionEditId, playerid);
			Faction_AddRank(id, inputtext);
		}
	}
	faction_rank_manage_show(playerid);
	return 1;
}

Dialog:FactionRankActionList(playerid, response, listitem, inputtext[]) {
	if(response) {
		switch(listitem) {
			case 0: { // Chinh sua ten chuc vu
				Dialog_Show(playerid, FactionRankNameEdit, DIALOG_STYLE_INPUT, "Faction Manage | Rank | Edit", "> Nhap ten chuc vu ban muon chinh sua.", "Xac nhan", "<<");
				return 1;
			}
			case 1: { // Xoa chuc vu
				if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid) && map_valid(m_factionRankEditOrder) && map_has_key(m_factionRankEditOrder, playerid)) {
					Faction_RemoveRank(map_get(m_factionEditId, playerid), map_get(m_factionRankEditOrder, playerid));
				}
			}
		}
	}
	if(map_valid(m_factionRankEditOrder) && map_has_key(m_factionRankEditOrder, playerid)) {
		map_remove(m_factionRankEditOrder, playerid);
		if(!map_size(m_factionRankEditOrder)) {
			map_delete(m_factionRankEditOrder);
			m_factionRankEditOrder = Map:-1;
		}
	}
	faction_rank_manage_show(playerid);
	return 1;
}

Dialog:FactionRankNameEdit(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(isnull(inputtext) || strlen(inputtext) > 31) {
			Dialog_Show(playerid, FactionRankNameEdit, DIALOG_STYLE_INPUT, "Faction Manage | Rank | Edit", "> Nhap ten moi cho chuc vu ban muon chinh sua.", "Xac nhan", "<<");
			return 1;
		}
		if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid) && map_valid(m_factionRankEditOrder) && map_has_key(m_factionRankEditOrder, playerid)) {
			Faction_SetRankName(map_get(m_factionEditId, playerid), map_get(m_factionRankEditOrder, playerid), inputtext);
		}
	}
	if(map_valid(m_factionRankEditOrder) && map_has_key(m_factionRankEditOrder, playerid)) {
		map_remove(m_factionRankEditOrder, playerid);
		if(!map_size(m_factionRankEditOrder)) {
			map_delete(m_factionRankEditOrder);
			m_factionRankEditOrder = Map:-1;
		}
	}
	faction_rank_manage_show(playerid);
	return 1;
}

Dialog:FactionDivisionManage(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
			new count = Faction_CountDivisions(map_get(m_factionEditId, playerid));
			if(listitem == count) {
				Dialog_Show(playerid, FactionDivisionAdd, DIALOG_STYLE_INPUT, "Faction Manage | Division | Add", "> Nhap ten don vi ban muon them.", "Xac nhan", "<<");
			} else {
				if(!map_valid(m_factionDivEditOrder)) {
					m_factionDivEditOrder = map_new();
				}
				map_set(m_factionDivEditOrder, playerid, listitem);
				Dialog_Show(playerid, FactionDivActionList, DIALOG_STYLE_LIST, "Faction Manage | Division | Action", "Chinh sua ten don vi\nXoa don vi", ">>", "<<");
			}
			return 1;
		}
	}
	faction_manage_show(playerid);
	return 1;
}

Dialog:FactionDivisionAdd(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(isnull(inputtext)) {
			Dialog_Show(playerid, FactionDivisionAdd, DIALOG_STYLE_INPUT, "Faction Manage | Division | Add", "> Nhap ten don vi ban muon them.", "Xac nhan", "<<");
			return 1;
		}

		if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid)) {
			Faction_AddDivision(map_get(m_factionEditId, playerid), inputtext);
		}
	}
	if(map_valid(m_factionDivEditOrder) && map_has_key(m_factionDivEditOrder, playerid)) {
		map_remove(m_factionDivEditOrder, playerid);
		if(!map_size(m_factionDivEditOrder)) {
			map_delete(m_factionDivEditOrder);
			m_factionDivEditOrder = Map:-1;
		}
	}
	faction_division_manage_show(playerid);
	return 1;
}

Dialog:FactionDivActionList(playerid, response, listitem, inputtext[]) {
	if(response) {
		switch(listitem) {
			case 0: { // Chinh sua ten don vi
				Dialog_Show(playerid, FactionDivisionNameEdit, DIALOG_STYLE_INPUT, "Faction Manage | Division | Name", "> Nhap ten moi cho don vi ban muon chinh sua.", "Xac nhan", "<<");
				return 1;
			}
			case 1: { // Xoa don vi
				if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid) && map_valid(m_factionDivEditOrder) && map_has_key(m_factionDivEditOrder, playerid)) {
					Faction_RemoveDivision(map_get(m_factionEditId, playerid), map_get(m_factionDivEditOrder, playerid));
				}
			}
		}
	}
	if(map_valid(m_factionDivEditOrder) && map_has_key(m_factionDivEditOrder, playerid)) {
		map_remove(m_factionDivEditOrder, playerid);
		if(!map_size(m_factionDivEditOrder)) {
			map_delete(m_factionDivEditOrder);
			m_factionDivEditOrder = Map:-1;
		}
	}
	faction_division_manage_show(playerid);
	return 1;
}

Dialog:FactionDivisionNameEdit(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(isnull(inputtext)) {
			Dialog_Show(playerid, FactionDivisionNameEdit, DIALOG_STYLE_INPUT, "Faction Manage | Division | Name", "> Nhap ten moi cho don vi ban muon chinh sua.", "Xac nhan", "<<");
			return 1;
		}

		if(map_valid(m_factionEditId) && map_has_key(m_factionEditId, playerid) && map_valid(m_factionDivEditOrder) && map_has_key(m_factionDivEditOrder, playerid)) {
			Faction_SetDivisionName(map_get(m_factionEditId, playerid), map_get(m_factionDivEditOrder, playerid), inputtext);
		}
	}
	if(map_valid(m_factionDivEditOrder) && map_has_key(m_factionDivEditOrder, playerid)) {
		map_remove(m_factionDivEditOrder, playerid);
		if(!map_size(m_factionDivEditOrder)) {
			map_delete(m_factionDivEditOrder);
			m_factionDivEditOrder = Map:-1;
		}
	}
	faction_division_manage_show(playerid);
	return 1;
}