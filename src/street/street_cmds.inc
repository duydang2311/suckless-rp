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

CMD:aliststreet(playerid) {
    if(Street_Count() == 0) {
        Player_Msg(playerid, COLOR_FADE2, "Khong co Street nao trong he thong.");
        return 0;
    }

    new Iter:it = Street_GetIter();
    if(iter_valid(it)) {
        new 
            string[33 + 1024] = "Index\tType\tNumber\tStreet Name",
            str[15 + 11] = "Street Lists: ";

        for(; iter_inside(it); iter_move_next(it)) {
            new 
                name[STREET_NAME_LENGTH], type[STREET_TYPE_NAME_LENGTH],
                streetid = iter_get_key(it), number, Task:t = task_new();

            task_unblock(1);
            number = await Street_GetNumber(streetid);

            t = Street_GetName(streetid);
            task_await_arr(t, name, STREET_NAME_LENGTH);
            task_delete(t);

            t = Street_GetTypeName(streetid);
            task_await_arr(t, type, STREET_TYPE_NAME_LENGTH);
            task_delete(t);

            format(string, sizeof(string), "%s\n%d\t%s\t%d\t%s", string, streetid, type, number, name);
        }

        format(str, sizeof(str), "%s%d", str, Street_Count());
        Dialog_Show(playerid, StreetListDialog, DIALOG_STYLE_TABLIST_HEADERS, str, string, "Close", "Apply");
    }

    iter_release(it);
    return 1;
} 
