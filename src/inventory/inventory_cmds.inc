#if defined inventory_cmds_included
	#endinput
#endif

#define inventory_cmds_included

CMD:inv(playerid) {
	new
		Iter:it = Inventory_GetIter(playerid),
		name[64],
		string[512] = "ID vat pham\tTen vat pham";
	if(iter_valid(it)) {
		for(; iter_inside(it); iter_move_next(it)) {
			Item_GetName(iter_get(it), name);
			format(string, sizeof(string), "%s\n%d\t%s", string, iter_get(it), name);
		}
		iter_release(it);
	}

	for(new i = 0, id = 0; i != 2; i++) {
		id = Player_GetHandItemId(playerid, i);
		if(id == -1) {
			format(name, sizeof(name), "-1\tTrong(%s)", (i ? "Tay trai" : "Tay phai"));
		} else {
			Item_GetName(id, name);
		}
		format(string, sizeof(string), "%s\n%d\t%s(%s)", string, iter_get(it), name, (i ? "Tay trai" : "Tay phai"));
	}
	Dialog_Show(playerid, InventoryDialog, DIALOG_STYLE_TABLIST_HEADERS, "Inventory", string, ">>", "Dong");
	return 1;
}

Dialog:InventoryDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		new size = Inventory_GetSize(playerid);
		if(size < listitem) {
			switch(listitem) {

			}
		} else {

		}
	}
	return 1;
}