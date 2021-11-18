#if defined item_radio_included
	#endinput
#endif

#define item_radio_included

static m_radioStats;

stock Task:IRadio_Create(Float:reliability = 1.0, occupied_space = 2) {
    new Task:t = task_new();
    task_detech();

    return t;
}