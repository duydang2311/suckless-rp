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
    SendClientMessage(playerid, -1, "qua roi chay di ban oi.");
    return 1;
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_streetPlayerManipulate = map_new();
    return 1;
}