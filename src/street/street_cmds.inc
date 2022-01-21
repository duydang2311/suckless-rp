#if defined street_adm_cmds_included
    #endinput
#endif

#define street_adm_cmds_included

#include <a_zones>

CMD:acreatestreet(playerid, params[]) {
    new number, type, name[STREET_NAME_LENGTH];
    if(sscanf(params, "iis[22]", number, type, name)) {
        Player_Msg(playerid, COLOR_LIGHTYELLOW, "Syntax: /acreatestreet [number] [type] [name]");
        return 1;
    }

    if(number < 0 || number >= 9999) {
        Player_Msg(playerid, COLOR_FADE2, "Street Number is not acceptable to create street, please try a new number again.");
        return 1;
    }

    if(type < 0 || type > 1) {
        Player_Msg(playerid, COLOR_FADE2, "Street Type is not acceptable to create street, please try a new number again.");
        return 1;
    }

    if(strlen(name) > STREET_NAME_LENGTH) {
        Player_Msg(playerid, COLOR_FADE2, "Street Name is not acceptable to create street, please try a new name again.");
        return 1;
    }

    new StreetType:street_type = Avenue;
    if(type == 1) {
        street_type = Street;
    }

    StreetManipulate_InitKey(playerid, number, street_type, name);
    Player_Msg(playerid, COLOR_YELLOW, "Hay di den dia diem Min-X/Min-Y va bam nut N (hay di den dia diem truoc tien).");
    return 1;
}

CMD:streetname(playerid) {  
    new streetid = Street_GetByPlayer(playerid); 
    if(streetid == -1) {
        Player_Msg(playerid, COLOR_FADE2, "Khu pho nay chua duoc dat ten, hay lien he den Administrator de ho tro them.");
        return 1;
    }

    new 
        province[MAX_ZONE_NAME], city[MAX_ZONE_NAME], 
        number, name[STREET_NAME_LENGTH], type[STREET_TYPE_NAME_LENGTH];
        
    task_unblock(1);
    
    new Task:t = Street_GetName(streetid);
    task_await_arr(t, name, STREET_NAME_LENGTH);
    task_delete(t);

    t = Street_GetTypeName(streetid);
    task_await_arr(t, type, STREET_TYPE_NAME_LENGTH);
    task_delete(t);

    number = await Street_GetNumber(streetid);
    GetPlayer3DProvince(playerid, province, MAX_ZONE_NAME);
    GetPlayer3DCity(playerid, city, MAX_ZONE_NAME);

    new string[33 + 11 + STREET_NAME_LENGTH + MAX_ZONE_NAME + MAX_ZONE_NAME];
    format(string, sizeof(string), "Street Name: %d, %s, (%s), %s, %s.", number, name, type, province, city);
    SendClientMessage(playerid, -1, string);
    return 1;
}
alias:streetname("address", "whereami", "street")

CMD:aliststreet(playerid) {
    new street_count = Street_Count();
    if(street_count == 0) {
        Player_Msg(playerid, COLOR_FADE2, "Khong co Street nao trong he thong.");
        return 0;
    }

    new Iter:it = Street_GetIter();
    if(iter_valid(it)) {
        new bool:condition = false;
        if(street_count > MAX_LINE_STREET_LIST) {
            condition = true;
        }

        StreetLists_SetPages(playerid, 0);
        ShowPlayerStreetListsDialog(playerid, condition, it, street_count);
    }
    
    return 1;
} 
