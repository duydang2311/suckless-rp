#if defined street_settings_included
    #endinput
#endif

#define street_settings_included

static 
    Map:m_playerSettingStreet;

static const string_streetSetting[] = "Street Stats\nStreet Position\nTeleporting\nDelete Street";

stock street_settings_show(playerid) {
    Dialog_Show(playerid, StreetSettingMainDialog, DIALOG_STYLE_LIST, "Street Setting", string_streetSetting, "Apply", "Close");
    return 1;
}

stock Street_GetPlayerEdittingDialog(streetid) {
    if(!map_valid(m_playerSettingStreet)) {
        return INVALID_PLAYER_ID;
    }

    for(new Iter:it = map_iter(m_playerSettingStreet); iter_inside(it); iter_move_next(it)) {
        if(iter_get_value(it) == streetid) {
            return iter_get_key(it);
        }
    }

    return INVALID_PLAYER_ID;
}

stock Street_GetByPlayerEditting(playerid) {
    return map_get(m_playerSettingStreet, playerid);
}

stock Street_SetPlayerEdittingDialog(streetid, playerid) {
    if(!map_valid(m_playerSettingStreet)) {
        m_playerSettingStreet = map_new();
    }

    map_set(m_playerSettingStreet, playerid, streetid);
    return 1;
}

stock Street_DeletePlayerEditting(playerid) {
    if(!map_valid(m_playerSettingStreet)) {
        return 0;
    }

    map_remove_deep(m_playerSettingStreet, playerid);
    if(!map_size(m_playerSettingStreet)) {
        map_delete(m_playerSettingStreet);
        m_playerSettingStreet = Map:-1;
    }

    return 1;
}

Dialog:StreetSettingMainDialog(playerid, response, listitem, inputtext[]) {
    new streetid = Street_GetByPlayerEditting(playerid);
    if(response) {
        new hasGreater = await Street_Has(streetid);
        if(!hasGreater) {
            Street_DeletePlayerEditting(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Street Index nay khong ton tai nua, vui long thu lai.");
            return 1;
        }

        switch(listitem) {
            // Street Stats
            case 0: {
                new 
                    Task:tasks[3],
                    number, name[STREET_NAME_LENGTH + 1], type[STREET_TYPE_NAME_LENGTH];

                tasks[0] = Street_GetName(streetid); tasks[1] = Street_GetTypeName(streetid); tasks[2] = Street_GetNumber(streetid);
                task_keep(tasks[0]); task_keep(tasks[1]); task_keep(tasks[2]);
                task_unblock(Y_HOOKS_BREAK_RETURN_1);
                await task_all(tasks[0], tasks[1], tasks[2]);
                task_get_result_arr(tasks[0], name, STREET_NAME_LENGTH + 1); 
                task_get_result_arr(tasks[1], type, STREET_TYPE_NAME_LENGTH);
                number = task_get_result(tasks[2]);
                task_delete(tasks[0]); task_delete(tasks[1]); task_delete(tasks[2]);

                new caption[39 + 1 + 11 + STREET_NAME_LENGTH + 1 + STREET_TYPE_NAME_LENGTH - 6 + 1];
                format(caption, sizeof(caption), "Number: %d\nType: %s\nName: %s\n<< Back", number, type, name);
                Dialog_Show(playerid, StreetSettingStatsDialog, DIALOG_STYLE_LIST, "Street Stats - Editing", caption, "Apply", "Close");
                return 1;
            }
        }
    } 

    Street_DeletePlayerEditting(playerid);
    return 1;
}

Dialog:StreetSettingStatsDialog(playerid, response, listitem, inputtext[]) {
    if(response) {
        switch(listitem) {
            // Number
            case 0: {
                Dialog_Show(playerid, StreetSettingNumberInput, DIALOG_STYLE_INPUT, "Input New Number", "Nhap vao day Number ma ban muon cho Street", "Apply", "Close");
                return 1;
            }
            case 1: {
                Dialog_Show(playerid, StreetSettingTypeSelect, DIALOG_STYLE_LIST, "Select New Type", "Avenue\nStreet", "Apply", "Close");
                return 1;
            }
            case 2: {
                Dialog_Show(playerid, StreetSettingNameInput, DIALOG_STYLE_INPUT, "Input New Name", "Nhap vao day Name moi ma ban muon cho Street", "Apply", "Close");
                return 1;
            }
        }
    } 
    
    street_settings_show(playerid);
    Player_Msg(playerid, COLOR_FADE2, "<< Ban da quay tro lai.");
    return 1;
}

Dialog:StreetSettingNumberInput(playerid, response, listitem, inputtext[]) {
    if(response) {
        task_unblock(Y_HOOKS_BREAK_RETURN_1);
        new     
            streetid = Street_GetByPlayerEditting(playerid),
            valid = await Street_Has(streetid);

        if(!valid) {
            Street_DeletePlayerEditting(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Street Index nay khong ton tai nua, vui long thu lai.");
            return 1;
        }

        task_unblock(1);
        new 
            number = strval(inputtext),
            condition = await Street_SetNumber(streetid, number);

        if(condition == 1) {
            street_settings_show(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Ban da dieu chinh thanh cong Number cua Street.");
            return 1;
        } else {
            Player_Msg(playerid, COLOR_FADE2, "Dieu chinh khong thanh cong, vui long thu lai.");
        }
    }

    street_settings_show(playerid);
    Player_Msg(playerid, COLOR_FADE2, "<< Ban da quay tro lai.");
    return 1;
}

Dialog:StreetSettingTypeSelect(playerid, response, listitem, inputtext[]) {
    if(response) {
        task_unblock(Y_HOOKS_BREAK_RETURN_1);
        new     
            streetid = Street_GetByPlayerEditting(playerid),
            valid = await Street_Has(streetid);

        if(!valid) {
            Street_DeletePlayerEditting(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Street Index nay khong ton tai nua, vui long thu lai.");
            return 1;
        }

        new condition = 0;
        task_unblock(1);
        switch(listitem) {
            case 0: {
                condition = await Street_SetType(streetid, Avenue);
            }
            case 1: {
                condition = await Street_SetType(streetid, Street);
            }
        }

        if(condition == 1) {
            street_settings_show(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Ban da dieu chinh thanh cong Type cua Street.");
            return 1;
        } else {
            Player_Msg(playerid, COLOR_FADE2, "Dieu chinh khong thanh cong, vui long thu lai.");
        }
    }

    street_settings_show(playerid);
    Player_Msg(playerid, COLOR_FADE2, "<< Ban da quay tro lai.");
    return 1;
}

Dialog:StreetSettingNameInput(playerid, response, listitem, inputtext[]) {
    if(response) {
        task_unblock(Y_HOOKS_BREAK_RETURN_1);
        new     
            streetid = Street_GetByPlayerEditting(playerid),
            valid = await Street_Has(streetid);

        if(!valid) {
            Street_DeletePlayerEditting(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Street Index nay khong ton tai nua, vui long thu lai.");
            return 1;
        }

        if(strlen(inputtext) > STREET_NAME_LENGTH) {
            street_settings_show(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Ten nay khong hop le, vui long thu lai.");
            return 1;
        }

        new condition = await Street_SetName(streetid, inputtext);
        if(condition == 1) {
            street_settings_show(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Ban da dieu chinh thanh cong Name cua Street.");
            return 1;
        } else {
            Player_Msg(playerid, COLOR_FADE2, "Dieu chinh khong thanh cong, vui long thu lai.");
        }
    }

    street_settings_show(playerid);
    Player_Msg(playerid, COLOR_FADE2, "<< Ban da quay tro lai.");
    return 1;
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_playerSettingStreet = map_new();
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if(map_valid(m_playerSettingStreet)) {
        if(map_has_key(m_playerSettingStreet, playerid)) {
            Street_DeletePlayerEditting(playerid);
        }
    }   
    return 1;
}   
