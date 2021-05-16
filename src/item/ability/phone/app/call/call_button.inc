#if defined app_call_button_included
	#endinput
#endif

#define app_call_button_included

static enum _:Key {
	ItemId,
	PlayerId
}

static
	Map:m_buttonUI, // { {Key}: PlayerText: }
	Map:m_contactSim; // { playerid: sim_id }

Dialog:AppCallContactName(playerid, response, listitem, inputtext) {
	if(!map_valid(m_contactSim) || !map_has_key(m_contactSim, playerid)) {
		return 1;
	}

	if(response) {
		if(isnull(inputtext)) {
			Dialog_Show(playerid, AppCallContactName, DIALOG_STYLE_INPUT, "> Nhap ten ban muon luu cho lien he nay.", ">>", "<<");
			return 1;
		}

		if(strlen(inputtext) >= CONTACT_NAME_SIZE - 1) {
			Dialog_Show(playerid, AppCallContactName, DIALOG_STYLE_INPUT, "> Nhap ten ban muon luu cho lien he nay.\nTen lien he qua dai, hay thu rut ngan them.", ">>", "<<");
			return 1;
		}

		new id = IPUI_GetPlayerPhone(playerid);
		if(id == -1 || !IGroup_Has(id)) {
			return 1;
		}

		new Iter:it = IGroup_GetIter(id);
		if(iter_valid(it) && iter_release(it)) {
			for(new sim; iter_inside(it); iter_move_next(it)) {
				sim = iter_get(it);
				if(ISim_Has(sim)) {
					new saved_sim = map_get(m_contactSim, playerid);
					task_unblock(1);
					new rows = await AppCall_AddContact(sim, saved_sim, inputtext);
					if(rows) {
						CallLocalFunction("AppCall_OnContactAdded", "iii", id, saved_sim, inputtext);
						SetPreciseTimer("AppCall_UIOnContactAdded", 0, false, "iii", id, saved_sim, inputtext);
					}
					break;
				}
			}
		}
	}
	map_remove(m_contactSim, playerid);
	if(!map_size(m_contactSim)) {
		map_delete(m_contactSim);
		m_contactSim = Map:-1;
	}
	return 1;
}

Dialog:AppCallContactSim(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(isnull(inputtext)) {
			Dialog_Show(playerid, AppCallContactSim, DIALOG_STYLE_INPUT, "> Nhap so dien thoai ban muon luu.", ">>", "<<");
			return 1;
		}

		new number[SIM_NUMBER_SIZE] = "";
		strcat(number, inputtext);
		if(!ISim_ValidateNumber(number)) {
			Dialog_Show(playerid, AppCallContactSim, DIALOG_STYLE_INPUT, "> Nhap so dien thoai ban muon luu.\nSo dien thoai khong hop le.", ">>", "<<");
			return 1;
		}

		task_unblock(1);
		new id = await ISim_RetrieveByNumber(number);
		if(id == -1) {
			Dialog_Show(playerid, AppCallContactSim, DIALOG_STYLE_INPUT, "> Nhap so dien thoai ban muon luu.\nSo dien thoai khong ton tai.", ">>", "<<");
			return 1;
		}

		if(!map_new(m_contactSim)) {
			m_contactSim = map_new();
		}
		map_set(m_contactSim, playerid, id);
		Dialog_Show(playerid, AppCallContactName, DIALOG_STYLE_INPUT, "> Nhap ten ban muon luu cho lien he nay.", ">>", "<<");
	}
	return 1;
}

stock AppCall_HasButtonUIKey(const key[], size = sizeof(key)) {
	return if(map_valid(m_buttonUI) && map_has_arr_key(m_buttonUI, key, size));
}

stock AppCall_DestroyButtonUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasButtonUIKey(key)) {
		return 0;
	}

	PlayerTextDrawDestroy(playerid, PlayerText:map_arr_get(m_buttonUI, key));
	map_arr_remove(m_buttonUI, key);
	if(!map_size(m_buttonUI)) {
		map_delete(m_buttonUI);
		m_buttonUI = Map:-1;
	}
	return 1;
}

stock AppCall_UpdateButtonUI(id, playerid) {
	const Float:BUTTON_SIZE = 6.00;
	new
		key[Key],
		Float:anc[2],
		Float:safe_min[2],
		Float:safe_max[2];
	IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
	IPUI_GetAnchor(id, playerid, anc[0], anc[1]);

	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_buttonUI)) {
		m_buttonUI = map_new();
	} else if(map_has_arr_key(m_buttonUI, key)) {
		AppCall_DestroyButtonUI(id, playerid);
	}

	new PlayerText:button = CreatePlayerTextDraw(playerid, anc[0] + safe_min[0], anc[1] + safe_max[1] - BUTTON_SIZE / 2.0, "Add contact");
	PlayerTextDrawFont(playerid, button, 1);
	PlayerTextDrawLetterSize(playerid, button, 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, button, anc[0] + safe_min[0] + 20.0, BUTTON_SIZE);
	PlayerTextDrawSetOutline(playerid, button, 1);
	PlayerTextDrawSetShadow(playerid, button, 0);
	PlayerTextDrawAlignment(playerid, button, 1);
	PlayerTextDrawColor(playerid, button, 0x7A7A7AFF);
	PlayerTextDrawBackgroundColor(playerid, button, 255);
	PlayerTextDrawBoxColor(playerid, button, 50);
	PlayerTextDrawUseBox(playerid, button, 1);
	PlayerTextDrawSetProportional(playerid, button, 1);
	PlayerTextDrawSetSelectable(playerid, button, 1);
	PlayerTextDrawShow(playerid, button);

	map_arr_set(m_buttonUI, key, _:button);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {
	new id = IPUI_GetPlayerPhone(playerid);
	if(id == -1) {
		return 1;
	}

	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasButtonUIKey(key)) {
		if(playertextid == PlayerText:map_arr_get(m_buttonUI, key)) {
			Dialog_Show(playerid, AppCallContactSim, DIALOG_STYLE_INPUT, "> Nhap so dien thoai ban muon luu.", ">>", "<<");
			return Y_HOOKS_BREAK_RETURN_1;
		}
	}
	return 1;
}