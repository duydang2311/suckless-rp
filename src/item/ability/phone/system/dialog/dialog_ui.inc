#if defined phone_system_dialog_ui_included
	#endinput
#endif

#define phone_system_dialog_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

enum _:PhoneDialogUI {
	PlayerText:uiText
}

static Map:m_dialogUI; // { {Key}: {PhoneDialogUI} }

stock IPDialog_HasUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_dialogUI) && map_has_arr_key(m_dialogUI, key, size));
}

stock IPDialog_DestroyUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!IPDialog_HasUIKey(key)) {
		return 0;
	}

	new ui[PhoneDialogUI];
	map_arr_get_arr(m_dialogUI, key, ui);
	PlayerTextDrawDestroy(playerid, ui[uiText]);
	map_arr_remove(m_dialogUI, key);
	if(!map_size(m_dialogUI)) {
		map_delete(m_dialogUI);
		m_dialogUI = Map:-1;
	}
	return 1;
}

stock IPDialog_UpdateUI(id, playerid) {
	if(!IPDialog_Has(id)) {
		return 0;
	}

	new
		key[Key],
		errorText[DIALOG_TEXT_SIZE],
		ui[PhoneDialogUI];
	IPDialog_GetText(id, errorText);
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(IPDialog_HasUIKey(key)) {
		IPDialog_DestroyUI(id, playerid);
	}

	const Float:TEXT_SIZE_Y = 5.0000;

	new
		Float:anc[2],
		Float:safe_min[2],
		Float:safe_max[2],
		Float:width = 0.0,
		Float:height = TEXT_SIZE_Y,
		Float:first_x,
		Float:first_y,
		Float:scr_width,
		Float:scr_height,
		content[DIALOG_TEXT_SIZE + 24],
		line,
		length,
		idx;
	IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
	IPUI_GetAnchor(id, playerid, anc[0], anc[1]);
	scr_width = safe_max[0] - safe_min[0] - 3.0;
	scr_height = safe_max[1] - safe_min[1];
	width = 0.0;
	line = 1;
	idx = 0;
	length = strlen(errorText);
	for(new i = 0; i != length; i++) {
		width += GetTextDrawCharacterWidth(errorText[i], 1, true) * 0.15;
		if(width >= scr_width) {
			if(height >= scr_height) {
				content[idx - 3] = '.';
				content[idx - 2] = '.';
				content[idx - 1] = '.';
				break;
			} else {
				height += TEXT_SIZE_Y;
				width = GetTextDrawCharacterWidth(errorText[i], 1, true) * 0.15;
				content[idx++] = '~';
				content[idx++] = 'n';
				content[idx++] = '~';
				line++;
			}
		}
		content[idx++] = (errorText[i] == ' ') ? '_' : errorText[i];
	}
	first_x = anc[0] + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0;
	first_y = anc[1] + safe_min[1] + (safe_max[1] - safe_min[1]) / 2.0 - height / 2.0;

	ui[uiText] = CreatePlayerTextDraw(playerid, first_x, first_y, errorText);
	PlayerTextDrawFont(playerid, ui[uiText], 1);
	PlayerTextDrawLetterSize(playerid, ui[uiText], 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, ui[uiText], TEXT_SIZE_Y, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiText], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiText], 0);
	PlayerTextDrawAlignment(playerid, ui[uiText], 2);
	PlayerTextDrawColor(playerid, ui[uiText], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiText], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiText], 50);
	PlayerTextDrawUseBox(playerid, ui[uiText], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiText], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiText], 0);

	PlayerTextDrawShow(playerid, ui[uiText]);
	if(!map_valid(m_dialogUI)) {
		m_dialogUI = map_new();
	}
	map_arr_set_arr(m_dialogUI, key, ui);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPUI_OnButtonClicked(id, playerid, button) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(IPDialog_HasUIKey(key)) {
		IPDialog_DestroyUI(id, playerid);
		if(IPDialog_Has(id)) {
			new callback[DIALOG_CALLBACK_SIZE];
			IPDialog_GetCallback(id, callback);
			if(!isnull(callback)) {
				switch(button) {
					case PhoneButtonCall: {
						CallLocalFunction(callback, "iii", id, playerid, 1);
					}
					case PhoneButtonHangup: {
						CallLocalFunction(callback, "iii", id, playerid, 0);
					}
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}