#if defined frequency_stats_included
    #endinput
#endif

#define frequency_stats_included

static Map:m_frequencyStats;

stock Frequency_Create(playerid, frequency) {
    if(!map_valid(m_frequencyStats)) {
        m_frequencyStats = map_new();
    }

    new LinkedList:ll_frequency_players = linked_list_new();
    if(!map_has_key(m_frequencyStats, frequency)) {
        linked_list_add(ll_frequency_players, playerid);
        map_set(m_frequencyStats, frequency, ll_frequency_players);
    } else {
        new bool:isHasPlayers = true;
        if(map_get_safe(m_frequencyStats, ll_frequency_players)) {
            for(new Iter:it = linked_list_iter(ll_frequency_players); iter_inside(it); iter_move_next(it)) {
                if(iter_get_value(it) == playerid) {
                    return (-1);
                } else {
                    isHasPlayers = false;
                }
            }

            if(isHasPlayers == false) {
                linked_list_add(ll_frequency_players, playerid);
            }
        }
    }

    return map_size(m_frequencyStats);
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_frequencyStats = map_new();
    return 1;   
}