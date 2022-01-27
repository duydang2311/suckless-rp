#if defined street_lists_included
    #endinput
#endif

#define street_lists_included

static Map:m_playerPagesStreetLists;

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

            task[0] = Street_GetName(streetid); task[1] = Street_GetTypeName(streetid); task[2] = Street_GetNumber(streetid);
            task_keep(task[0]); task_keep(task[1]); task_keep(task[2]);
            task_unblock(1);
            await task_all(task[0], task[1], task[2]);
            task_get_result_arr(task[0], name, STREET_NAME_LENGTH); 
            task_get_result_arr(task[1], type, STREET_TYPE_NAME_LENGTH);
            number = task_get_result(task[2]);
            task_delete(task[0]); task_delete(task[1]); task_delete(task[2]);
            
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
            m_playerPagesStreetLists = Map:-1;
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
            new cmdtext[17 + 11 - 2 + 1];
            StreetLists_SetPages(playerid, 0);
            format(cmdtext, sizeof(cmdtext), "/streetsetting %d", Street_GetKeyAt(listitem));
            PC_EmulateCommand(playerid, cmdtext);
        }
    }

    return 1;
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_playerPagesStreetLists = map_new();
    return 1;
}
