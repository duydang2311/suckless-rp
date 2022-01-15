#if defined street_adm_cmds_included
    #endinput
#endif

#define street_adm_cmds_included

CMD:acreatestreet(playerid, params[]) {
    new number, type, name[STREET_NAME_LENGTH];
    if(sscanf(params, "iis[22]", number, type, name)) {
        printf("number: %d, type: %d, name: %s", number, type, name);
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
    Player_Msg(playerid, COLOR_YELLOW, "Hay di den dia diem Min-X/Min-Y va bam chuoc trai (hay di den dia diem truoc tien).");
    return 1;
}