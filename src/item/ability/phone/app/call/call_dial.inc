#if defined app_call_dial_included
	#endinput
#endif

#define app_call_dial_included

stock Task:AppCall_Dial(from_sim, to_sim) {
	new
		Task:t = task_new(),
		from_number[SIM_NUMBER_SIZE],
		to_number[SIM_NUMBER_SIZE];
	if(!ISim_GetNumber(from_sim, from_number)) {
		task_unblock(1);
		await_arr(from_number) ISim_RetrieveNumber(from_sim);
	}
	if(!ISim_GetNumber(to_sim, to_number)) {
		task_unblock(1);
		await_arr(to_number) ISim_RetrieveNumber(to_sim);
	}
	CallLocalFunction("AppCall_OnDialed", "ii", from_sim, to_sim);
	return t;
}