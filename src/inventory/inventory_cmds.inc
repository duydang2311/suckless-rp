#if defined inventory_cmds_included
	#endinput
#endif

#define inventory_cmds_included

static Map:m_selectedItemId;

static item_group_root_show(playerid) {
	new
		Iter:it = Inventory_GetIter(playerid),
		name[64],
		string[512] = "ID\tTen vat pham";
	if(!iter_valid(it)) {
		return 0;
	}
	for(; iter_inside(it); iter_move_next(it)) {
		Item_GetName(iter_get(it), name);
		format(string, sizeof(string), "%s\n%d\t%s (%s)", string, iter_get(it), name, (Inventory_IsAttached(playerid, iter_get(it)) ? "Da gan" : "Chua gan"));
	}
	iter_release(it);
	strcat(string, "\n"E_COLOR_FADE2"> Chon nhom ban muon dua vat pham da chon vao.");
	Dialog_Show(playerid, InventoryItemGroupRoot, DIALOG_STYLE_TABLIST_HEADERS, "Inventory Item Group | Select Root", string, ">>", "<<");
	return 1;
}

static item_group_members_show(playerid, group_id) {
	new
		Iter:it = Item_GetGroupIter(group_id),
		name[32],
		string[512] = "ID\tTen vat pham";
	Item_GetName(group_id, name);
	format(string, sizeof(string), "%s\n"E_COLOR_GREEN"%d\t%s", string, group_id, name);
	if(iter_valid(it)) {
		for(; iter_inside(it); iter_move_next(it)) {
			Item_GetName(iter_get(it), name);
			format(string, sizeof(string), "%s\n%d\t%s", string, iter_get(it), name);
		}
	}
	if(!map_valid(m_selectedItemId)) {
		m_selectedItemId = map_new();
	}
	map_set(m_selectedItemId, playerid, group_id);
	Dialog_Show(playerid, InventoryItemGroupTake, DIALOG_STYLE_TABLIST_HEADERS, "Inventory Item Group | Take", string, "Lay", "<<");
	return 1;
}

CMD:inv(playerid) {
	new
		Iter:it = Inventory_GetIter(playerid),
		name[64],
		string[512] = "ID\tTen vat pham";
	if(iter_valid(it)) {
		for(new id; iter_inside(it); iter_move_next(it)) {
			id = iter_get(it);
			Item_GetName(id, name);
			if(Item_HasGroup(id)) {
				format(string, sizeof(string), "%s\n"E_COLOR_GREEN"%d\t"E_COLOR_GREEN"%s", string, id, name);
			} else {
				format(string, sizeof(string), "%s\n%d\t%s", string, id, name);
			}
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

CMD:attachinv(playerid) {
	new
		Iter:it = Inventory_GetIter(playerid),
		name[64],
		string[512] = "ID\tTen vat pham";
	if(iter_valid(it)) {
		for(; iter_inside(it); iter_move_next(it)) {
			Item_GetName(iter_get(it), name);
			format(string, sizeof(string), "%s\n%d\t%s (%s)", string, iter_get(it), name, (Inventory_IsAttached(playerid, iter_get(it)) ? "Da gan" : "Chua gan"));
		}
		iter_release(it);
	}
	Dialog_Show(playerid, InventoryAttachDialog, DIALOG_STYLE_TABLIST_HEADERS, "Inventory Attachment", string, ">>", "Dong");
	return 1;
}

Dialog:InventoryDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		new size = Inventory_GetSize(playerid);
		if(listitem < size) {
			new hand = Player_GetFreeHand(playerid);
			if(hand != -1) {
				new Iter:it = Inventory_GetIter(playerid, listitem);
				if(iter_valid(it)) {
					new id = iter_get(it);
					iter_release(it);
					if(Item_HasGroup(id)) {
						item_group_members_show(playerid, id);
						return 1;
					} else {
						Inventory_RemoveItemByIter(playerid, it);
						Player_EquipItem(playerid, hand, id);
					}
				}
			}
		} else {
			new id = Player_GetHandItemId(playerid, listitem - size);
			if(id != -1) {
				if(Item_GetOccupiedSpace(id) > 1) {
					Player_Msg(playerid, COLOR_YELLOW, "Day la loai vat pham khong the bo vao Inventory, hay mang no hoac vut no xuong dat (nhan N khi cam tren tay).");
				} else {
					Inventory_AddItem(playerid, id);
					Player_UnequipItem(playerid, listitem - size);
				}
			}
		}
		PC_EmulateCommand(playerid, "/inv");
	}
	return 1;
}

CMD:groupitem(playerid) {
	new
		Iter:it = Inventory_GetIter(playerid),
		name[64],
		string[512] = "ID\tTen vat pham";
	if(iter_valid(it)) {
		for(; iter_inside(it); iter_move_next(it)) {
			Item_GetName(iter_get(it), name);
			format(string, sizeof(string), "%s\n%d\t%s (%s)", string, iter_get(it), name, (Inventory_IsAttached(playerid, iter_get(it)) ? "Da gan" : "Chua gan"));
		}
		iter_release(it);
	}
	strcat(string, "\n"E_COLOR_FADE2"> Chon vat pham ban muon dua vao nhom.");
	Dialog_Show(playerid, InventoryItemGroupMember, DIALOG_STYLE_TABLIST_HEADERS, "Inventory Item Group | Select Member", string, ">>", "Dong");
	return 1;
}

Dialog:InventoryAttachDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		new Iter:it = Inventory_GetIter(playerid, listitem);
		if(iter_valid(it)) {
			new id = iter_get(it);
			iter_release(it);
			if(!map_valid(m_selectedItemId)) {
				m_selectedItemId = map_new();
			}
			map_set(m_selectedItemId, playerid, id);
			if(Inventory_IsAttached(playerid, id)) {
				Dialog_Show(playerid, InventoryAttachAction, DIALOG_STYLE_LIST, "Inventory Attachment | Action", "Thao vat pham\nChinh sua vi tri\nDieu chinh bodypart", ">>", "Dong");
			} else {
				new slot = Player_GetFreeAttachmentSlot(playerid);
				if(slot == -1) {
					Player_Msg(playerid, COLOR_RED, "Ban da gan qua nhieu vat pham tren co the.");
					PC_EmulateCommand(playerid, "/attachinv");
					return 1;
				}
				task_unblock(1);
				if(task_await(Inventory_AttachItem(playerid, slot, id))) {
					PC_EmulateCommand(playerid, "/attachinv");
				} else {
					Player_Msg(playerid, COLOR_YELLOW, "[!]: He thong chua co du lieu Bodypart de gan vat pham vi day la lan dau tien ban gan vat pham nay len nguoi.");
					Player_Msg(playerid, COLOR_YELLOW, "[!]: Hay tien hanh thiet lap Bodypart de gan vat pham bang cach chon vi tri ban muon gan len co the.");
					Dialog_Show(playerid, InventoryAttachBoneSetup, DIALOG_STYLE_LIST, "Inventory Attachment | Bodypart Setup", "Than\nDau\nBap tay trai\nBap tay phai\nDui trai\nDui phai\nChan trai\nChan phai\nBap chan trai\nBap chan phai\nCang tay trai\nCang tay phai\nVai trai\nVai phai\nCo\nHam", ">>", "<<");
				}
			}
		}
	}
	return 1;
}

Dialog:InventoryAttachBoneSetup(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
			new
				id = map_get(m_selectedItemId, playerid),
				bone = (listitem < 4) ? listitem + 1 : listitem + 3;
			task_unblock(1);
			await Inventory_InsertAttachment(id, bone);
			new slot = Player_GetFreeAttachmentSlot(playerid);
			if(slot == -1) {
				Player_Msg(playerid, COLOR_YELLOW, "Co the nhan vat da het slot gan vat pham, he thong se khong tu dong gan.");
			} else {
				SetPreciseTimer("Inventory_AttachItem", 0, false, "iii", playerid, slot, id);
				Player_Msg(playerid, COLOR_YELLOW, "Vat pham da duoc gan len Bodypart ban vua chon.");
			}
		} else {
			PC_EmulateCommand(playerid, "/attachinv");
		}
	}
	if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
		map_remove(m_selectedItemId, playerid);
		if(!map_size(m_selectedItemId)) {
			map_delete(m_selectedItemId);
			m_selectedItemId = Map:-1;
		}
	}
	return 1;
}

Dialog:InventoryAttachAction(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
			new id = map_get(m_selectedItemId, playerid);
			switch(listitem) {
				case 0: {
					Inventory_DetachItem(playerid, id);
				}
				case 1: {
					new slot = Inventory_GetAttachSlot(playerid, id);
					if(slot != -1) {
						EditAttachedObject(playerid, slot);
						return 1;
					}
				}
				case 2: {
					Inventory_DetachItem(playerid, id);
					Dialog_Show(playerid, InventoryAttachBoneSetup, DIALOG_STYLE_LIST, "Inventory Attachment | Bone Setup", "Than\nDau\nBap tay trai\nBap tay phai\nDui trai\nDui phai\nChan trai\nChan phai\nBap chan trai\nBap chan phai\nCang tay trai\nCang tay phai\nVai trai\nVai phai\nCo\nHam", ">>", "<<");
					return 1;
				}
			}
		}
	}
	if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
		map_remove(m_selectedItemId, playerid);
		if(!map_size(m_selectedItemId)) {
			map_delete(m_selectedItemId);
			m_selectedItemId = Map:-1;
		}
	}
	PC_EmulateCommand(playerid, "/attachinv");
	return 1;
}

Dialog:InventoryItemGroupMember(playerid, response, listitem, inputtext[]) {
	if(response) {
		new size = Inventory_GetSize(playerid);
		if(listitem == size) {
			PC_EmulateCommand(playerid, "/groupitem");
			return 1;
		}

		new Iter:it = Inventory_GetIter(playerid, listitem);
		if(!iter_valid(it)) {
			return 0;
		}
		if(!map_valid(m_selectedItemId)) {
			m_selectedItemId = map_new();
		}
		map_set(m_selectedItemId, playerid, iter_get(it));
		iter_release(it);
		item_group_root_show(playerid);
	}
	return 1;
}

Dialog:InventoryItemGroupRoot(playerid, response, listitem, inputtext[]) {
	if(response) {
		new size = Inventory_GetSize(playerid);
		if(listitem == size) {
			item_group_root_show(playerid);
			return 1;
		}
		if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
			new
				id = map_get(m_selectedItemId, playerid),
				Iter:it = Inventory_GetIter(playerid, listitem);
			if(iter_valid(it)) {
				new select_id = iter_get(it);
				iter_release(it);
				if(id == select_id) {
					item_group_root_show(playerid);
					return 1;
				}

				new capacity = Item_GetFreeCapacity(select_id);
				if(capacity < 0 || Item_GetOccupiedSpace(id) > capacity) {
					Player_Msg(playerid, COLOR_YELLOW, "Vat pham duoc chon lam Root khong du suc chua vat pham Member.");
					item_group_root_show(playerid);
					return 1;
				}

				Item_AddToGroupItem(id, select_id);
				Inventory_RemoveItem(playerid, id);
			}
		}
	}
	if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
		map_remove(m_selectedItemId, playerid);
		if(!map_size(m_selectedItemId)) {
			map_delete(m_selectedItemId);
			m_selectedItemId = Map:-1;
		}
	}
	PC_EmulateCommand(playerid, "/groupitem");
	return 1;
}

Dialog:InventoryItemGroupTake(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
			if(!listitem) {
				Player_Msg(playerid, COLOR_RED, "Ban khong the lay vat pham dang lam chu the cua group.");
				item_group_members_show(playerid, map_get(m_selectedItemId, playerid));
				return 1;
			}

			new
				group_id = map_get(m_selectedItemId, playerid),
				Iter:it = Item_GetGroupIter(group_id, listitem - 1);
			if(iter_valid(it)) {
				new id = iter_get(it);
				iter_release(it);
				Item_RemoveFromGroupItem(id, group_id);
				Inventory_AddItem(playerid, id);
			}
		}
	}
	if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
		map_remove(m_selectedItemId, playerid);
		if(!map_size(m_selectedItemId)) {
			map_delete(m_selectedItemId);
			m_selectedItemId = Map:-1;
		}
	}
	PC_EmulateCommand(playerid, "/inv");
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerEditAttachedObj(playerid, response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ) {
	if(map_valid(m_selectedItemId) && map_has_key(m_selectedItemId, playerid)) {
		new id = map_get(m_selectedItemId, playerid);
		if(response) {
			Inventory_SetAttachment(id, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
			SetPlayerAttachedObject(playerid, index, Item_GetModel(id), boneid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
		} else {
			Inventory_AttachItem(playerid, index, id);
		}
		map_remove(m_selectedItemId, playerid);
		if(!map_size(m_selectedItemId)) {
			map_delete(m_selectedItemId);
			m_selectedItemId = Map:-1;
		}
		PC_EmulateCommand(playerid, "/attachinv");
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}