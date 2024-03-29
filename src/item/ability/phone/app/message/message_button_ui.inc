#if defined app_message_button_ui_included
	#endinput
#endif

#define app_message_button_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static
	Map:m_buttonUI, // { {Key}: PlayerText:button_textdraw }
	Map:m_toSimId; // { playerid: sim id which message will be sent to }

forward AppMsg_OnMsgSendRequest(playerid, to_sim);
public AppMsg_OnMsgSendRequest(playerid, to_sim) {
	if(!map_valid(m_toSimId)) {
		m_toSimId = map_new();
	}
	map_set(m_toSimId, playerid, to_sim);
	Dialog_Show(playerid, AppMsgContentDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap noi dung tin nhan ban muon gui.", "Send", "<<");
	return 1;
}

Dialog:AppMsgSimDialog(playerid, response, listitem, inputtext[]) {
	if(response) {
		if(isnull(inputtext)) {
			Dialog_Show(playerid, AppMsgSimDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap so dien thoai ban muon gui tin nhan.", ">>", "<<");
			return 1;
		}

		new number[SIM_NUMBER_SIZE] = "";
		strcat(number, inputtext);
		if(!ISim_ValidateNumber(number)) {
			Dialog_Show(playerid, AppMsgSimDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap so dien thoai ban muon gui tin nhan.\nSo dien thoai khong hop le.", ">>", "<<");
			return 1;
		}

		task_unblock(1);
		new id = await ISim_RetrieveByNumber(number);
		if(id == -1) {
			Dialog_Show(playerid, AppMsgSimDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap so dien thoai ban muon gui tin nhan.\nSo dien thoai khong ton tai.", ">>", "<<");
			return 1;
		}

		if(!map_valid(m_toSimId)) {
			m_toSimId = map_new();
		}
		map_set(m_toSimId, playerid, id);
		Dialog_Show(playerid, AppMsgContentDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap noi dung tin nhan ban muon gui.", "Send", "<<");
	}
	return 1;
}

Dialog:AppMsgContentDialog(playerid, response, listitem, inputtext[]) {
	if(!map_valid(m_toSimId) || !map_has_key(m_toSimId, playerid)) {
		return 1;
	}

	if(response) {
		if(isnull(inputtext)) {
			Dialog_Show(playerid, AppMsgContentDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap noi dung tin nhan ban muon gui.", "Send", "<<");
			return 1;
		} else if(strlen(inputtext) > MESSAGE_CONTENT_SIZE) {
			Dialog_Show(playerid, AppMsgContentDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap noi dung tin nhan ban muon gui.\nNoi dung tin nhan qua dai, vui long rut ngan lai.", "Send", "<<");
			return 1;
		} else {
			new
				phone = IPUI_GetPlayerPhone(playerid),
				Iter:it = IGroup_GetIter(phone),
				key[Key];
			key[ItemId] = phone;
			key[PlayerId] = playerid;
			if(iter_valid(it) && iter_release(it)) {
				for(new sim; iter_inside(it); iter_move_next(it)) {
					sim = iter_get(it);
					if(ISim_Has(sim)) {
						task_unblock(1);
						new id = await AppMsg_SendAsync(sim, map_get(m_toSimId, playerid), inputtext);
						CallLocalFunction("AppMsg_StatsOnMsgSent", "iiis", id, sim, map_get(m_toSimId, playerid), inputtext);
						SetPreciseTimer("AppMsg_UIOnMsgSent", 0, false, "iii", id, sim, map_get(m_toSimId, playerid));
						break;
					}
				}
			}
		}
	} else {
		Dialog_Show(playerid, AppMsgSimDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap so dien thoai ban muon gui tin nhan.", ">>", "<<");
	}
	map_remove(m_toSimId, playerid);
	if(!map_size(m_toSimId)) {
		map_delete(m_toSimId);
		m_toSimId = Map:-1;
	}
	return 1;
}

stock AppMsg_HasButtonUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_buttonUI) && map_has_arr_key(m_buttonUI, key, size));
}

stock AppMsg_DestroyButtonUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppMsg_HasButtonUIKey(key)) {
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

stock AppMsg_UpdateButtonUI(id, playerid) {
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
		AppMsg_DestroyButtonUI(id, playerid);
	}

	new PlayerText:button = CreatePlayerTextDraw(playerid, anc[0] + safe_min[0], anc[1] + safe_max[1] - BUTTON_SIZE / 2.0, "Send");
	PlayerTextDrawFont(playerid, button, 1);
	PlayerTextDrawLetterSize(playerid, button, 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, button, anc[0] + safe_min[0] + 20.0, BUTTON_SIZE);
	PlayerTextDrawSetOutline(playerid, button, 1);
	PlayerTextDrawSetShadow(playerid, button, 0);
	PlayerTextDrawAlignment(playerid, button, 1);
	PlayerTextDrawColor(playerid, button, 0xFCCA03FF);
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

hook IPA_OnUIHideAttempt(id, playerid) {
	AppMsg_DestroyButtonUI(id, playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {
	new id = IPUI_GetPlayerPhone(playerid);
	if(id == -1) {
		return 1;
	}
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppMsg_HasButtonUIKey(key)) {
		if(playertextid == PlayerText:map_arr_get(m_buttonUI, key)) {
			new sim = AppMsg_GetSimFromDetail(id);
			if(sim != -1) {
				if(!map_valid(m_toSimId)) {
					m_toSimId = map_new();
				}
				map_set(m_toSimId, playerid, sim);
				Dialog_Show(playerid, AppMsgContentDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap noi dung tin nhan ban muon gui.", "Send", "<<");
			} else if(AppMsg_HasListUIKey(key)) {
				Dialog_Show(playerid, AppMsgSimDialog, DIALOG_STYLE_INPUT, "App Message | Send", "> Nhap so dien thoai ban muon gui tin nhan.", ">>", "<<");
			}
			return Y_HOOKS_BREAK_RETURN_1;
		}
	}
	return 1;
}
