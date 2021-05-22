#if defined app_call_incoming_ui_included
	#endinput
#endif

#define app_call_incoming_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static enum _:IncomingUI {
	PlayerText:uiIncNumberText,
	PlayerText:uiIncExtraText
}

static Map:m_incomingUI; // { {Key}: {IncomingUI} }

stock AppCall_HasIncomingUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_incomingUI) && map_has_arr_key(m_incomingUI, key, size));
}

stock AppCall_DestroyIncomingUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!AppCall_HasIncomingUIKey(key)) {
		return 0;
	}

	new ui[IncomingUI];
	map_arr_get_arr(m_incomingUI, key, ui);
	PlayerTextDrawDestroy(playerid, ui[uiIncNumberText]);
	PlayerTextDrawDestroy(playerid, ui[uiIncExtraText]);
	map_arr_remove(m_incomingUI, key);
	if(!map_size(m_incomingUI)) {
		map_delete(m_incomingUI);
		m_incomingUI = Map:-1;
	}
	return 1;
}

stock AppCall_UpdateIncomingUI(id, playerid) {
	if(!AppCall_HasIncoming(id)) {
		return 0;
	}

	new
		key[Key],
		number[SIM_NUMBER_SIZE],
		ui[IncomingUI];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasIncomingUIKey(key)) {
		AppCall_DestroyIncomingUI(id, playerid);
	}

	AppCall_GetIncomingNumber(id, number);
	const
		Float:SIM_NUMBER_TEXT_SIZE = 5.00,
		Float:SIM_NUMBER_TEXT_GAP = 3.00;

	new
		Float:anc[2],
		Float:safe_min[2],
		Float:safe_max[2],
		Float:first_x,
		Float:first_y;
	IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
	IPUI_GetAnchor(id, playerid, anc[0], anc[1]);
	first_x = anc[0] + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0;
	first_y = anc[1] + safe_min[1] + SIM_NUMBER_TEXT_SIZE;

	ui[uiIncNumberText] = CreatePlayerTextDraw(playerid, first_x, first_y, number);
	PlayerTextDrawFont(playerid, ui[uiIncNumberText], 3);
	PlayerTextDrawLetterSize(playerid, ui[uiIncNumberText], 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, ui[uiIncNumberText], SIM_NUMBER_TEXT_SIZE, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiIncNumberText], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiIncNumberText], 0);
	PlayerTextDrawAlignment(playerid, ui[uiIncNumberText], 2);
	PlayerTextDrawColor(playerid, ui[uiIncNumberText], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiIncNumberText], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiIncNumberText], 50);
	PlayerTextDrawUseBox(playerid, ui[uiIncNumberText], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiIncNumberText], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiIncNumberText], 0);
	first_y += SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;

	ui[uiIncExtraText] = CreatePlayerTextDraw(playerid, first_x, first_y, "dang goi den ban...");
	PlayerTextDrawFont(playerid, ui[uiIncExtraText], 2);
	PlayerTextDrawLetterSize(playerid, ui[uiIncExtraText], 0.120000, 0.4800000);
	PlayerTextDrawTextSize(playerid, ui[uiIncExtraText], SIM_NUMBER_TEXT_SIZE - 2.0, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiIncExtraText], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiIncExtraText], 0);
	PlayerTextDrawAlignment(playerid, ui[uiIncExtraText], 2);
	PlayerTextDrawColor(playerid, ui[uiIncExtraText], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiIncExtraText], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiIncExtraText], 50);
	PlayerTextDrawUseBox(playerid, ui[uiIncExtraText], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiIncExtraText], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiIncExtraText], 0);

	PlayerTextDrawShow(playerid, ui[uiIncNumberText]);
	PlayerTextDrawShow(playerid, ui[uiIncExtraText]);
	if(!map_valid(m_incomingUI)) {
		m_incomingUI = map_new();
	}
	map_arr_set_arr(m_incomingUI, key, ui);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasIncomingUIKey(key)) {
		switch(button) {
			case PhoneButtonCall: {
			}
			case PhoneButtonHangup: {
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnUIHideAttempt(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(AppCall_HasIncomingUIKey(key)) {
		AppCall_CancelIncoming(id);
	}
	AppCall_DestroyIncomingUI(id, playerid);
	return 1;
}

hook AppCall_OnUIShowAttempt(id, playerid, &bool:busy) {
	if(AppCall_HasIncoming(id)) {
		AppCall_UpdateIncomingUI(id, playerid);
		busy = true; // means that when app call initiates, it won't show the contact ui but show this incoming call ui instead
	}
	return 1;
}