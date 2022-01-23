#if defined street_dialogs_included
    #endinput
#endif

#define street_dialogs_included

static
    Map:m_playerPagesStreetLists,
    Map:m_playerSettingStreet;

static const string_streetSetting[] = "Street Stats\nStreet Position\nTeleporting\nDelete Street";

stock street_lists_show(playerid) {
    new 
        pages = StreetLists_GetPages(playerid),
        Iter:it = Street_GetIter(MAX_LINE_STREET_LIST * pages); 
    
    if(iter_valid(it)) {
        new 
            count = 0, number = -1,
            name[STREET_NAME_LENGTH], type[STREET_TYPE_NAME_LENGTH],
            caption[21 + 16 + 33 + 1500] = "Index\tType\tNumber\tStreet Name", 
            title[15 + 17 + 11 + 11 + 1] = "Street Lists: ";

        new bool:isStreetGreater = false;
        if(Street_Count() > MAX_LINE_STREET_LIST) {
            isStreetGreater = true;
        }

        for(; iter_inside(it); iter_move_next(it)) {
            new 
                streetid = iter_get_key(it),    
                Task:task[3];

            task[0] = Street_GetNumber(streetid); task[1] = Street_GetName(streetid); task[2] = Street_GetTypeName(streetid);
            await task_all(task[0], task[1], task[2]);
            task_delete(task[0]); task_delete(task[1]); task_delete(task[1]);

            if(isStreetGreater) {
                if(count == MAX_LINE_STREET_LIST) {
                    break;
                }

                count++;
            }

            format(caption, sizeof(caption), "%s\n%d\t%s\t%d\t%s", caption, streetid, type, number, name);
        }

        if(isStreetGreater) {
            format(caption, sizeof(caption), "%s\n>> Next Page", caption);
            format(caption, sizeof(caption), "%s\n<< Previous Page", caption);
        }    

        format(title, sizeof(title), "%s%d | Pages: %d", title, Street_Count(), pages);
        Dialog_Show(playerid, StreetListDialog, DIALOG_STYLE_TABLIST_HEADERS, title, caption, "Apply", "Close");
    } 

    iter_release(it);
    return 1;
}

stock street_settings_show(playerid) {
    Dialog_Show(playerid, StreetSettingMainDialog, DIALOG_STYLE_LIST, "Street Setting", string_streetSetting, "Apply", "Close");
    return 1;
}

stock bool:StreetLists_SetPages(playerid, const pages) {
    if(!map_valid(m_playerPagesStreetLists)) {
        m_playerPagesStreetLists = map_new();
    }
    
    if(pages == 0) {
        if(!map_has_key(m_playerPagesStreetLists, playerid)) {
            return false;
        }

        map_remove(m_playerPagesStreetLists, playerid);
        if(!map_size(m_playerPagesStreetLists)) {
            map_delete(m_playerPagesStreetLists);
            m_playerPagesStreetLists = Map:0;
        }

        return true;
    }

    map_set(m_playerPagesStreetLists, playerid, pages);    
    return true;
}

stock StreetLists_GetPages(playerid) {
    if(!map_valid(m_playerPagesStreetLists)) {
        return 0;
    } 

    if(!map_has_key(m_playerPagesStreetLists, playerid)) {
        return 0;
    }

    return map_get(m_playerPagesStreetLists, playerid);
}

stock Street_GetPlayerEdittingDialog(streetid) {
    if(!map_valid(m_playerPagesStreetLists)) {
        return INVALID_PLAYER_ID;
    }

    for(new Iter:it = map_iter(m_playerPagesStreetLists); iter_inside(it); iter_move_next(it)) {
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

    map_remove(m_playerSettingStreet, playerid);
    if(!map_size(m_playerSettingStreet)) {
        map_delete(m_playerSettingStreet);
        m_playerSettingStreet = Map:0;
    }

    return 1;
}

Dialog:StreetListDialog(playerid, response, listitem, inputtext[]) {
    if(response) { 
        new 
            street_count = Street_Count(),
            pages = StreetLists_GetPages(playerid),
            bool:hasGreater = false;

        if(street_count > MAX_LINE_STREET_LIST) {
            hasGreater = true;
        }

        switch(listitem) {
            // Next Pages
            case MAX_LINE_STREET_LIST: {
                if(hasGreater == true) {
                    if(pages == (street_count / MAX_LINE_STREET_LIST) - 1) {
                        Player_Msg(playerid, COLOR_FADE2, "Khong the tien toi.");
                        return 1;
                    }

                    StreetLists_SetPages(playerid, pages + 1);
                }
            }

            // Previous Pages
            case MAX_LINE_STREET_LIST + 1: {
                if(hasGreater == true) {
                    if(pages <= 0) {
                        Player_Msg(playerid, COLOR_FADE2, "Khong the quay lai.");
                        return 1;
                    }

                    StreetLists_SetPages(playerid, pages - 1);
                }
            }
        }

        if(hasGreater == true) {
            street_lists_show(playerid);
        } else {
            StreetLists_SetPages(playerid, 0);
        }
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
                    name[STREET_NAME_LENGTH + 1], type[STREET_TYPE_NAME_LENGTH],
                    number = await Street_GetNumber(streetid);

                new Task:t = Street_GetName(streetid);
                task_await_arr(t, name, STREET_NAME_LENGTH);
                task_delete(t);

                t = Street_GetTypeName(streetid);
                task_await_arr(t, type, STREET_TYPE_NAME_LENGTH);
                task_delete(t);

                new caption[39 + 1 + 11 + STREET_NAME_LENGTH + 1 + STREET_TYPE_NAME_LENGTH - 6 + 1];
                format(caption, sizeof(caption), "Number: %d\nType: %s\nName: %s\n<< Back", number, type, name);
                Dialog_Show(playerid, StreetSettingStatsDialog, DIALOG_STYLE_LIST, "Street Stats - Editing", caption, "Apply", "Close");
                return 1;
            }
        }
    } 

    Street_SetPlayerEdittingDialog(streetid, INVALID_PLAYER_ID);
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
        new     
            streetid = Street_GetByPlayerEditting(playerid),
            valid = await Street_Has(streetid);

        if(!valid) {
            Street_DeletePlayerEditting(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Street Index nay khong ton tai nua, vui long thu lai.");
            return 1;
        }

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
        new     
            streetid = Street_GetByPlayerEditting(playerid),
            valid = await Street_Has(streetid);

        if(!valid) {
            Street_DeletePlayerEditting(playerid);
            Player_Msg(playerid, COLOR_FADE2, "Street Index nay khong ton tai nua, vui long thu lai.");
            return 1;
        }

        new condition = 0;
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
    m_playerPagesStreetLists = map_new();
    m_playerSettingStreet = map_new();
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if(map_valid(m_playerSettingStreet)) {
        for(new Iter:it = map_iter(m_playerSettingStreet); iter_inside(it); iter_move_next(it)) {
            if(iter_get(it) == playerid) {
                Street_SetPlayerEdittingDialog(iter_get_key(it), INVALID_PLAYER_ID);
                return Y_HOOKS_BREAK_RETURN_1;
            }
        }
    }

    return 1;
}   
