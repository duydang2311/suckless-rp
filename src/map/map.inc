#include "./farm/farm_map.inc"
#include "./farm/farm_reagan_fern.inc"
#include "./farm/TheFarm.inc"

#include <YSI_Coding\y_hooks>
hook OnGameModeInit() {
    // Farm Mapping
    TheFarm_OnGameModeInit();
    return 1;
}

hook OnPlayerConnect(playerid) {
    TheFarm_OnPlayerConnect(playerid);
    return 1;
} 