#if defined frequency_stats_included
    #endinput
#endif

#define frequency_stats_included

static Map:m_frequencyStats;

stock Frequency_Create(frequency) {
    if(!map_valid(m_frequencyStats)) {
        m_frequencyStats = map_new();
    }

    return map_size(m_frequencyStats);
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_frequencyStats = map_new();
    return 1;   
}