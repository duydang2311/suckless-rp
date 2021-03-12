#if defined session_included
	#endinput
#endif

#define session_included

static __increment = 0;

stock Session_GenerateId() {
	return ++__increment;
}