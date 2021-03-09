#if defined inventory_cmds_included
	#endinput
#endif

#define inventory_cmds_included

CMD:inv(playerid) {
	new
		Iter:it = Inventory_GetIter(playerid),
		name[64],
		string[512] = "ID\tTen vat pham";
	if(iter_valid(it)) {
		new stats[InventoryItemStats];
		for(; iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, stats);
			Item_GetName(stats[inv_itemId], name);
			format(string, sizeof(string), "%s\n%d\t%s", string, stats[inv_itemId], name);
		}
		iter_release(it);
	}

	for(new i = 0, id = 0; i != 2; i++) {
		id = Player_GetHandItemId(playerid, i);
		if(id == -1) {
			format(string, sizeof(string), "%s\n--\tTrong (%s)", string, (i ? "Tay phai" : "Tay trai"));
		} else {
			Item_GetName(id, name);
			format(string, sizeof(string), "%s\n%d\t%s (%s)", string, id, name, (i ? "Tay phai" : "Tay trai"));
		}
	}
	Dialog_Show(playerid, InventoryDialog, DIALOG_STYLE_TABLIST_HEADERS, "Inventory", string, ">>", "Dong");
	return 1;
}

Dialog:InventoryDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		new size = Inventory_GetSize(playerid);
		printf("size: %d, listitem: %d", size, listitem);
		if(listitem < size) {
			new hand = Player_GetFreeHand(playerid);
			printf("free hand: %d", hand);
			if(hand != -1) {
				new Iter:it = Inventory_GetIter(playerid, listitem);
				if(iter_valid(it)) {
					new stats[InventoryItemStats];
					iter_get_arr(it, stats);
					printf("iter valid, selected item id %d", stats[inv_itemId]);
					Player_EquipItem(playerid, hand, stats[inv_itemId]);
					iter_release(it);
				}
			}
		} else {
			Player_UnequipItem(playerid, listitem - size);
		}
		PC_EmulateCommand(playerid, "/inv");
	}
	return 1;
}