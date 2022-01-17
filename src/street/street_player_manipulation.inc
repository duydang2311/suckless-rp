#if defined street_p_manipulation_defined
    #endinput
#endif

#define street_p_manipulation_defined

static Map:m_streetPlayerManipulate;
new Map:m_playerInStreet;

static enum _:TempStreetData {
    temp_strCount,
    temp_strNumber,
    StreetType:temp_strType,
    temp_strName[STREET_NAME_LENGTH],
    Float:temp_strMinPos[2],
    Float:temp_strMaxPos[2]
};

stock StreetManipulate_InitKey(playerid, number, StreetType:type, const name[STREET_NAME_LENGTH]) {
    if(!map_valid(m_streetPlayerManipulate)) {
        m_streetPlayerManipulate = map_new();
    }

    if(map_has_key(m_streetPlayerManipulate, playerid)) {
        return 0;
    }

    new temp_array[TempStreetData];
    temp_array[temp_strCount] = 1;
    
    temp_array[temp_strNumber] = number;
    temp_array[temp_strType] = type;

    temp_array[temp_strMinPos][0] = 0.0000;
    temp_array[temp_strMinPos][1] = 0.0000;
    temp_array[temp_strMaxPos][0] = 0.0000;
    temp_array[temp_strMaxPos][1] = 0.0000;
    
    strcat(temp_array[temp_strName], name, STREET_NAME_LENGTH);
    map_set_arr(m_streetPlayerManipulate, playerid, temp_array);
    return 1;
}

stock StreetManipulate_RemoveKey(playerid) {
    if(!map_has_key(m_streetPlayerManipulate, playerid)) {
        return 0;
    }

    map_remove_deep(m_streetPlayerManipulate, playerid);
    if(!map_size(m_streetPlayerManipulate)) {
        map_delete(m_streetPlayerManipulate);
        m_streetPlayerManipulate = Map:-1;
    }
    return 1;
}

stock Street_GetByPlayer(playerid) {
    if(!map_valid(m_playerInStreet)) {
        return (-1);
    }

    if(!map_has_key(m_playerInStreet, playerid)) {
        return (-1);
    }

    return map_get(m_playerInStreet, playerid);
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_streetPlayerManipulate = map_new();
    m_playerInStreet = map_new();
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(map_valid(m_streetPlayerManipulate)) {
        if(map_has_key(m_streetPlayerManipulate, playerid)) {
            if(newkeys == KEY_NO) {
                new temp_array[TempStreetData];
                map_get_arr(m_streetPlayerManipulate, playerid, temp_array);
                
                new Float:x, Float:y, Float:z;
                GetPlayerPos(playerid, x, y, z);

                if(temp_array[temp_strCount] == 2) {
                    Player_Msg(playerid, COLOR_LIGHTYELLOW, "He thong dang khoi tao, vui long doi trong giay lat...");

                    task_unblock(Y_HOOKS_BREAK_RETURN_1);
                    new streetid = await Street_Create(temp_array[temp_strNumber], temp_array[temp_strType], temp_array[temp_strName]);
                    new condition = await Street_CreateArea(streetid, temp_array[temp_strMinPos][0], temp_array[temp_strMinPos][1], x, y, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
                    
                    if(condition >= 0) {
                        Player_Msg(playerid, COLOR_LIGHTYELLOW, "Ban da tao thanh cong co so cua Street, hay di den buoc tiep theo.");
                        StreetManipulate_RemoveKey(playerid);
                    } else if(condition == -1) {
                        StreetManipulate_RemoveKey(playerid);
                        Player_Msg(playerid, COLOR_FADE2, "He thong khong tim thay ket qua.");
                    }

                    return Y_HOOKS_BREAK_RETURN_1;
                }

                temp_array[temp_strCount]++;

                temp_array[temp_strMinPos][0] = x;
                temp_array[temp_strMinPos][1] = y;
        
                map_set_arr(m_streetPlayerManipulate, playerid, temp_array);
                Player_Msg(playerid, COLOR_LIGHTYELLOW, "Hay di den dia diem Max-X/Max-Y va bam N de thuc hien buoc tiep theo.");
            }
        }
    }
    
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    StreetManipulate_RemoveKey(playerid);
    return 1;   
}

hook OnPlayerEnterDynArea(playerid, areaid) {
    if(Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID) == StreetExtraID) {
        if(!map_valid(m_playerInStreet)) {
            m_playerInStreet = map_new();
        }

        map_set(m_playerInStreet, playerid, Street_GetIDByArea(areaid));
    }

    return 1;
}

hook OnPlayerLeaveDynArea(playerid, areaid) {
    if(Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID) == StreetExtraID) {
        if(!map_valid(m_playerInStreet)) {
            return 0;
        }

        map_remove(m_playerInStreet, playerid);
        if(!map_size(m_playerInStreet)) {
            map_delete(m_playerInStreet);
            m_playerInStreet = Map:-1;
        }
    }

    return 1;
}