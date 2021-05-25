#if defined phone_system_error_ui_included
	#endinput
#endif

#define phone_system_error_ui_included

#define IPError_Show(%0,%1,%2,%3) _prv_IPError_Show(%0,#%1,%2,%3)
#define PhoneError:%0(%1) \
	forward phErr_%0(%1); \
	public phErr_%0(%1)

static enum _:Key {
	ItemId,
	PlayerId
}

enum _:PhoneErrorUI {
	PlayerText:uiText
}

static Map:m_errorUI; // { {Key}: {PhoneErrorUI} }

stock IPError_HasUIKey(const key[], size = sizeof(key)) {
	return (map_valid(m_errorUI) && map_has_arr_key(m_errorUI, key, size));
}

stock IPError_UpdateUI(id, playerid) {
	if(!IPError_Has(id)) {
		return 0;
	}

	new
		key[Key],
		number[SIM_NUMBER_SIZE],
		errorText[ERROR_TEXT_SIZE],
		ui[PhoneErrorUI];
	IPError_GetText(id, errorText);
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(IPError_HasUIKey(key)) {
		IPError_DestroyUI(id, playerid);
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
		line,
		length;
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
				msg_cnt--;
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
	PlayerTextDrawTextSize(playerid, ui[uiText], SIM_NUMBER_TEXT_SIZE, (safe_max[0] - safe_min[0]));
	PlayerTextDrawSetOutline(playerid, ui[uiText], 1);
	PlayerTextDrawSetShadow(playerid, ui[uiText], 0);
	PlayerTextDrawAlignment(playerid, ui[uiText], 2);
	PlayerTextDrawColor(playerid, ui[uiText], -1);
	PlayerTextDrawBackgroundColor(playerid, ui[uiText], 255);
	PlayerTextDrawBoxColor(playerid, ui[uiText], 50);
	PlayerTextDrawUseBox(playerid, ui[uiText], 0);
	PlayerTextDrawSetProportional(playerid, ui[uiText], 1);
	PlayerTextDrawSetSelectable(playerid, ui[uiText], 0);
	first_y += SIM_NUMBER_TEXT_SIZE + SIM_NUMBER_TEXT_GAP;

	PlayerTextDrawShow(playerid, ui[uiText]);
	if(!map_valid(m_errorUI)) {
		m_errorUI = map_new();
	}
	map_arr_set_arr(m_errorUI, key, ui);
	return 1;
}