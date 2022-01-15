#if defined street_p_manipulation_defined
    #endinput
#endif

#define street_p_manipulation_defined

new Map:m_streetPlayerManipulate;

static enum _:TempStreetData {
    temp_strCount,
    temp_strNumber,
    temp_strType,
    temp_strName[STREET_NAME_LENGTH],
    Float:temp_strMinPos[2],
    Float:temp_strMaxPos[2]
};

stock StreetManipulate_InitKey(playerid, number, type, const name[STREET_NAME_LENGTH]) {
    if(map_has_key(m_streetPlayerManipulate, playerid)) {
        return 0;
    }

    new temp_array[TempStreetData];
    temp_array[temp_strCount] = 0;
    
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

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_streetPlayerManipulate = map_new();
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(map_has_key(m_streetPlayerManipulate, playerid)) {
        if(newkeys == KEY_FIRE) {
            new temp_array[TempStreetData];
            map_get_arr(m_streetPlayerManipulate, playerid, temp_array);
            if(temp_array[temp_strCount] == 2) {
                new streetid = await Street_Create(temp_array[temp_strNumber], temp_array[temp_strType], temp_array[temp_strName]);
                new condition = await StreetManipulate_CreateArea(streetid, temp_array[temp_strMinPos][0], temp_array[temp_strMinPos][1], temp_array[temp_strMaxPos][0], temp_array[temp_strMaxPos][1], GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
                if(condition) {
                    Player_Msg(playerid, COLOR_LIGHTYELLOW, "Ban da tao thanh cong co so cua Street, hay di den buoc tiep theo.");
                    return 1;
                }

                printf("[debug player-manipulation] bi out ra day roi dev oi, cuu toi.");
                return 0;
            } 
                
            temp_array[temp_strCount]++;
                
            new Float:x, Float:y, Float:z;
            GetPlayerPos(playerid, x, y, z);
            
            if(temp_array[temp_strCount] == 1) {
                temp_array[temp_strMinPos][0] = x;
                temp_array[temp_strMinPos][1] = y;
            } else {
                temp_array[temp_strMaxPos][0] = x;
                temp_array[temp_strMaxPos][1] = y;
            }

            map_set_arr(m_streetPlayerManipulate, playerid, temp_array);
        }
    }

    return 1;
}