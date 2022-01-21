#if defined street_dialogs_included
    #endinput
#endif

#define street_dialogs_included

static
    Map:m_playerPagesStreetLists,
    Map:m_playerSettingStreet;

stock ShowPlayerStreetListsDialog(playerid, bool:condition, Iter:it, street_count) {
    new pages = StreetLists_GetPages(playerid);
    if(condition == true) {
        if(iter_valid(it) && it != Iter:0) {
            iter_release(it);
        }

        it = Street_GetIter(MAX_LINE_STREET_LIST * pages);  
    }     

    if(iter_valid(it)) {
        new 
            count = 0, number = -1,
            name[STREET_NAME_LENGTH], type[STREET_TYPE_NAME_LENGTH],
            caption[21 + 16 + 33 + 1500] = "Index\tType\tNumber\tStreet Name", 
            title[15 + 17 + 11 + 11 + 1] = "Street Lists: ";

        for(; iter_inside(it); iter_move_next(it)) {
            new 
                streetid = iter_get_key(it), 
                Task:t = task_new();

            task_unblock(1);
            number = await Street_GetNumber(streetid);

            t = Street_GetName(streetid);
            task_await_arr(t, name, STREET_NAME_LENGTH);
            task_delete(t);

            t = Street_GetTypeName(streetid);
            task_await_arr(t, type, STREET_TYPE_NAME_LENGTH);
            task_delete(t);

            if(condition == true) {
                if(count == MAX_LINE_STREET_LIST) {
                    break;
                }

                count++;
            }

            format(caption, sizeof(caption), "%s\n%d\t%s\t%d\t%s", caption, streetid, type, number, name);
        }

        if(street_count > MAX_LINE_STREET_LIST) {
            format(caption, sizeof(caption), "%s\n>> Next Page", caption);
            format(caption, sizeof(caption), "%s\n<< Previous Page", caption);
        }    

        format(title, sizeof(title), "%s%d | Pages: %d", title, street_count, pages);
        Dialog_Show(playerid, StreetListDialog, DIALOG_STYLE_TABLIST_HEADERS, title, caption, "Apply", "Close");
    } 

    iter_release(it);
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

    if(!map_has_key(m_playerSettingStreet, streetid)) {
        return INVALID_PLAYER_ID;
    }

    return map_get(m_playerSettingStreet, streetid);
}

stock bool:Street_SetPlayerEdittingDialog(streetid, playerid) {
    if(!map_valid(m_playerSettingStreet)) {
        m_playerSettingStreet = map_new();
    }

    if(playerid == INVALID_PLAYER_ID) {
        map_remove(m_playerSettingStreet, streetid);
        if(!map_size(m_playerSettingStreet)) {
            map_delete(m_playerSettingStreet);
            m_playerSettingStreet = Map:0;
        }

        return true;
    }

    map_set(m_playerSettingStreet, streetid, playerid);
    return true;
}

Dialog:StreetListDialog(playerid, response, listitem, inputtext[]) {
    if(response) { 
        new 
            street_count = Street_Count(),
            pages = StreetLists_GetPages(playerid),
            bool:condition = false;

        if(street_count > MAX_LINE_STREET_LIST) {
            condition = true;
        }

        switch(listitem) {
            // Next Pages
            case MAX_LINE_STREET_LIST: {
                if(condition == true) {
                    if(pages == (street_count / MAX_LINE_STREET_LIST) - 1) {
                        Player_Msg(playerid, COLOR_FADE2, "Khong the tien toi.");
                        return 1;
                    }

                    StreetLists_SetPages(playerid, pages + 1);
                }
            }

            // Previous Pages
            case MAX_LINE_STREET_LIST + 1: {
                if(condition == true) {
                    if(pages <= 0) {
                        Player_Msg(playerid, COLOR_FADE2, "Khong the quay lai.");
                        return 1;
                    }

                    StreetLists_SetPages(playerid, pages - 1);
                }
            }
        }

        if(condition == true) {
            ShowPlayerStreetListsDialog(playerid, condition, Iter:0, street_count);
        } else {
            StreetLists_SetPages(playerid, 0);
        }
    }

    return 1;
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_playerPagesStreetLists = map_new();
    m_playerSettingStreet = map_new();
    return 1;
}
