#if defined player_hand_cmds_included
	#endinput
#endif

#define player_hand_cmds_included

static Map:m_selectedHandIndex;

CMD:attachhand(playerid) {
	new
		id,
		name[32],
		string[256] = "Tay\tID\tTen vat pham";
	id = Player_GetHandItemId(playerid, 0);
	if(id == -1) {
		strcat(string, "\nTay trai\t(Trong)\t(Trong)");
	} else {
		Item_GetName(id, name);
		format(string, sizeof(string), "%s\nTay trai\t%d\t%s", string, id, name);
	}
	id = Player_GetHandItemId(playerid, 1);
	if(id == -1) {
		strcat(string, "\nTay phai\t(Trong)\t(Trong)");
	} else {
		Item_GetName(id, name);
		format(string, sizeof(string), "%s\nTay phai\t%d\t%s", string, id, name);
	}
	Dialog_Show(playerid, HandAttachmentDialog, DIALOG_STYLE_TABLIST_HEADERS, "Hand Attachments", string, ">>", "<<");
	return 1;
}

Dialog:HandAttachmentDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		new id = Player_GetHandItemId(playerid, listitem);
		if(id == -1) {
			Player_Msg(playerid, COLOR_RED, "Tay ban chon hien dang trong.");
			PC_EmulateCommand(playerid, "/attachhand");
			return 1;
		}
		if(Player_GetHandAttachSlot(playerid, listitem) == -1) {
			task_unblock(1);
			if(!task_await(Player_AttachItemToHand(playerid, listitem, id))) {
				Player_Msg(playerid, COLOR_RED, "Tren co the ban da gan qua nhieu vat pham, khong con slot trong de gan vat pham cho tay.");
				PC_EmulateCommand(playerid, "/attachhand");
				return 1;
			}
		}
		if(!map_valid(m_selectedHandIndex)) {
			m_selectedHandIndex = map_new();
		}
		map_set(m_selectedHandIndex, playerid, listitem);
		EditAttachedObject(playerid, Player_GetHandAttachSlot(playerid, listitem));
	}
	return 1;
}

hook OnPlayerEditAttachedObj(playerid, response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ) {
	if(response && map_valid(m_selectedHandIndex) && map_has_key(m_selectedHandIndex, playerid)) {
		SetPlayerAttachedObject(playerid, index, modelid, boneid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
		Player_UpdateHandAttach(playerid, map_get(m_selectedHandIndex, playerid), Player_GetHandItemId(playerid, map_get(m_selectedHandIndex, playerid)), fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
		map_remove(m_selectedHandIndex, playerid);
		if(!map_size(m_selectedHandIndex)) {
			map_delete(m_selectedHandIndex);
		}
	}
	return 1;
}