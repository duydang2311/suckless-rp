#if defined nokia_ui_included
	#endinput
#endif

#define nokia_ui_included

static enum _:UIStats {
	PlayerText:UIBody,
	PlayerText:UIScreen,
	PlayerText:UIBtnUp,
	PlayerText:UIBtnDown,
	PlayerText:UIBtnLeft,
	PlayerText:UIBtnRight,
	PlayerText:UIBtnCall,
	PlayerText:UIBtnHangup,
	Float:UIX,
	Float:UIY
}

static enum _:UIStatsKey {
	ItemId,
	PlayerId
}

static Map:m_uiStats;

stock NokiaUI_Has(const key[], size = sizeof(key)) {
	return (map_valid(m_uiStats) && map_has_arr_key(m_uiStats, key, size));
}

#include <YSI_Coding\y_hooks>

hook IPUI_OnAnchorGetAttempt(id, playerid, &Float:x, &Float:y) {
	if(IPhone_GetModel(id) == Nokia) {
		new key[UIStatsKey];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(NokiaUI_Has(key)) {
			new stats[UIStats];
			map_arr_get_arr(m_uiStats, key, stats);
			x = stats[UIX];
			y = stats[UIY];
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPUI_OnShowAttempt(id, playerid, Float:x, Float:y) {
	if(IPhone_GetModel(id) == Nokia) {
		new
			stats[UIStats],
			key[UIStatsKey];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(!map_valid(m_uiStats)) {
			m_uiStats = map_new();
		} else if(map_has_arr_key(m_uiStats, key)) {
			map_arr_get_arr(m_uiStats, key, stats);
			PlayerTextDrawDestroy(playerid, stats[UIBody]);
			PlayerTextDrawDestroy(playerid, stats[UIScreen]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnUp]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnDown]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnLeft]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnRight]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnCall]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnHangup]);
		}
		stats[UIX] = x;
		stats[UIY] = y;

		stats[UIScreen] = CreatePlayerTextDraw(playerid, x + 13.000000, y + 24.500000, "phone:screen");
		PlayerTextDrawFont(playerid, stats[UIScreen], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIScreen], 0.600000, 2.000000);
		PlayerTextDrawTextSize(playerid, stats[UIScreen], 84.000000, 84.000000);
		PlayerTextDrawSetOutline(playerid, stats[UIScreen], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIScreen], 0);
		PlayerTextDrawAlignment(playerid, stats[UIScreen], 1);
		PlayerTextDrawColor(playerid, stats[UIScreen], -1);
		PlayerTextDrawBackgroundColor(playerid, stats[UIScreen], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIScreen], 50);
		PlayerTextDrawUseBox(playerid, stats[UIScreen], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIScreen], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIScreen], 0);

		stats[UIBody] = CreatePlayerTextDraw(playerid, x, y, "phone:body_trans");
		PlayerTextDrawFont(playerid, stats[UIBody], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBody], 0.600000, 2.000000);
		PlayerTextDrawTextSize(playerid, stats[UIBody], 110.000000, 220.000000);
		PlayerTextDrawSetOutline(playerid, stats[UIBody], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIBody], 0);
		PlayerTextDrawAlignment(playerid, stats[UIBody], 1);
		PlayerTextDrawColor(playerid, stats[UIBody], -1);
		PlayerTextDrawBackgroundColor(playerid, stats[UIBody], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIBody], 50);
		PlayerTextDrawUseBox(playerid, stats[UIBody], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIBody], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIBody], 0);

		stats[UIBtnUp] = CreatePlayerTextDraw(playerid, x + 47.000000, y + 116.500000, "ld_drv:blkdot");
		PlayerTextDrawFont(playerid, stats[UIBtnUp], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBtnUp], 0.600000, -0.149996);
		PlayerTextDrawTextSize(playerid, stats[UIBtnUp], 16.500000, 4.500000);
		PlayerTextDrawSetOutline(playerid, stats[UIBtnUp], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIBtnUp], 0);
		PlayerTextDrawAlignment(playerid, stats[UIBtnUp], 2);
		PlayerTextDrawColor(playerid, stats[UIBtnUp], 0x00000000);
		PlayerTextDrawBackgroundColor(playerid, stats[UIBtnUp], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIBtnUp], -1);
		PlayerTextDrawUseBox(playerid, stats[UIBtnUp], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIBtnUp], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIBtnUp], 1);

		stats[UIBtnDown] = CreatePlayerTextDraw(playerid, x + 47.000000, y + 130.000000, "LD_DRV:BLKDOT");
		PlayerTextDrawFont(playerid, stats[UIBtnDown], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBtnDown], 0.600000, -0.299997);
		PlayerTextDrawTextSize(playerid, stats[UIBtnDown], 16.500000, 4.500000);
		PlayerTextDrawSetOutline(playerid, stats[UIBtnDown], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIBtnDown], 0);
		PlayerTextDrawAlignment(playerid, stats[UIBtnDown], 2);
		PlayerTextDrawColor(playerid, stats[UIBtnDown], 0x00000000);
		PlayerTextDrawBackgroundColor(playerid, stats[UIBtnDown], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIBtnDown], -1);
		PlayerTextDrawUseBox(playerid, stats[UIBtnDown], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIBtnDown], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIBtnDown], 1);

		stats[UIBtnLeft] = CreatePlayerTextDraw(playerid, x + 42.000000, y + 121.000000, "LD_DRV:BLKDOT");
		PlayerTextDrawFont(playerid, stats[UIBtnLeft], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBtnLeft], 0.554166, 0.100000);
		PlayerTextDrawTextSize(playerid, stats[UIBtnLeft], 5.000000, 8.500000);
		PlayerTextDrawSetOutline(playerid, stats[UIBtnLeft], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIBtnLeft], 0);
		PlayerTextDrawAlignment(playerid, stats[UIBtnLeft], 2);
		PlayerTextDrawColor(playerid, stats[UIBtnLeft], 0x00000000);
		PlayerTextDrawBackgroundColor(playerid, stats[UIBtnLeft], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIBtnLeft], -1);
		PlayerTextDrawUseBox(playerid, stats[UIBtnLeft], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIBtnLeft], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIBtnLeft], 1);

		stats[UIBtnRight] = CreatePlayerTextDraw(playerid, x + 63.500000, y + 121.000000, "LD_DRV:BLKDOT");
		PlayerTextDrawFont(playerid, stats[UIBtnRight], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBtnRight], 0.554166, 0.100000);
		PlayerTextDrawTextSize(playerid, stats[UIBtnRight], 5.000000, 8.500000);
		PlayerTextDrawSetOutline(playerid, stats[UIBtnRight], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIBtnRight], 0);
		PlayerTextDrawAlignment(playerid, stats[UIBtnRight], 2);
		PlayerTextDrawColor(playerid, stats[UIBtnRight], 0x00000000);
		PlayerTextDrawBackgroundColor(playerid, stats[UIBtnRight], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIBtnRight], -1);
		PlayerTextDrawUseBox(playerid, stats[UIBtnRight], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIBtnRight], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIBtnRight], 1);

		stats[UIBtnCall] = CreatePlayerTextDraw(playerid, x + 18.500000, y + 115.500000, "ld_drv:blkdot");
		PlayerTextDrawFont(playerid, stats[UIBtnCall], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBtnCall], 0.150000, 1.200000);
		PlayerTextDrawTextSize(playerid, stats[UIBtnCall], 14.000000, 14.000000);
		PlayerTextDrawSetOutline(playerid, stats[UIBtnCall], 0);
		PlayerTextDrawSetShadow(playerid, stats[UIBtnCall], 0);
		PlayerTextDrawAlignment(playerid, stats[UIBtnCall], 2);
		PlayerTextDrawColor(playerid, stats[UIBtnCall], 0x00000000);
		PlayerTextDrawBackgroundColor(playerid, stats[UIBtnCall], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIBtnCall], -1);
		PlayerTextDrawUseBox(playerid, stats[UIBtnCall], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIBtnCall], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIBtnCall], 1);

		stats[UIBtnHangup] = CreatePlayerTextDraw(playerid, x + 78.000000, y + 115.500000, "LD_DRV:BLKDOT");
		PlayerTextDrawFont(playerid, stats[UIBtnHangup], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBtnHangup], 0.554166, 1.250000);
		PlayerTextDrawTextSize(playerid, stats[UIBtnHangup], 14.000000, 14.000000);
		PlayerTextDrawSetOutline(playerid, stats[UIBtnHangup], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIBtnHangup], 0);
		PlayerTextDrawAlignment(playerid, stats[UIBtnHangup], 2);
		PlayerTextDrawColor(playerid, stats[UIBtnHangup], 0x00000000);
		PlayerTextDrawBackgroundColor(playerid, stats[UIBtnHangup], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIBtnHangup], -1);
		PlayerTextDrawUseBox(playerid, stats[UIBtnHangup], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIBtnHangup], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIBtnHangup], 1);

		PlayerTextDrawShow(playerid, stats[UIBody]);
		PlayerTextDrawShow(playerid, stats[UIScreen]);
		PlayerTextDrawShow(playerid, stats[UIBtnUp]);
		PlayerTextDrawShow(playerid, stats[UIBtnDown]);
		PlayerTextDrawShow(playerid, stats[UIBtnLeft]);
		PlayerTextDrawShow(playerid, stats[UIBtnRight]);
		PlayerTextDrawShow(playerid, stats[UIBtnCall]);
		PlayerTextDrawShow(playerid, stats[UIBtnHangup]);
		map_arr_set_arr(m_uiStats, key, stats);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPUI_OnHideAttempt(id, playerid) {
	if(IPhone_GetModel(id) == Nokia) {
		new key[UIStatsKey];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(NokiaUI_Has(key)) {
			new stats[UIStats];
			map_arr_get_arr(m_uiStats, key, stats);
			PlayerTextDrawDestroy(playerid, stats[UIBody]);
			PlayerTextDrawDestroy(playerid, stats[UIScreen]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnUp]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnDown]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnLeft]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnRight]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnCall]);
			PlayerTextDrawDestroy(playerid, stats[UIBtnHangup]);
			map_arr_remove(m_uiStats, key);
			if(!map_size(m_uiStats)) {
				map_delete(m_uiStats);
				m_uiStats = Map:-1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPUI_OnScreenColorSet(id, playerid, color) {
	if(IPhone_GetModel(id) == Nokia) {
		new key[UIStatsKey];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(NokiaUI_Has(key)) {
			new ui[UIStats];
			map_arr_get_arr(m_uiStats, key, ui);
			PlayerTextDrawColor(playerid, ui[UIScreen], color);
			PlayerTextDrawShow(playerid, ui[UIScreen]);
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid) {
	new phone = IPUI_GetPlayerPhone(playerid);
	if(phone != -1) {
		new key[UIStatsKey];
		key[ItemId] = phone;
		key[PlayerId] = playerid;
		if(NokiaUI_Has(key)) {
			new
				stats[UIStats];
			map_arr_get_arr(m_uiStats, key, stats);
			if(playertextid == stats[UIBtnUp]) {
				CallLocalFunction("IPUI_OnButtonClicked", "iii", phone, playerid, PhoneButtonUp);
				return Y_HOOKS_BREAK_RETURN_1;
			} else if(playertextid == stats[UIBtnDown]) {
				CallLocalFunction("IPUI_OnButtonClicked", "iii", phone, playerid, PhoneButtonDown);
				return Y_HOOKS_BREAK_RETURN_1;
			} else if(playertextid == stats[UIBtnLeft]) {
				CallLocalFunction("IPUI_OnButtonClicked", "iii", phone, playerid, PhoneButtonLeft);
				return Y_HOOKS_BREAK_RETURN_1;
			} else if(playertextid == stats[UIBtnRight]) {
				CallLocalFunction("IPUI_OnButtonClicked", "iii", phone, playerid, PhoneButtonRight);
				return Y_HOOKS_BREAK_RETURN_1;
			} else if(playertextid == stats[UIBtnCall]) {
				CallLocalFunction("IPUI_OnButtonClicked", "iii", phone, playerid, PhoneButtonCall);
				return Y_HOOKS_BREAK_RETURN_1;
			} else if(playertextid == stats[UIBtnHangup]) {
				CallLocalFunction("IPUI_OnButtonClicked", "iii", phone, playerid, PhoneButtonHangup);
				return Y_HOOKS_BREAK_RETURN_1;
			}
		}
	}
	return 1;
}
