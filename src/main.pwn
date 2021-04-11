/*

   _____ .     . __    _ .___       .    _      _      .___  .___    ___    _______ .____    ___   _______
  (      /     / |\   |  /   `     /|     `.   /       /   \ /   \ .'   `. '   /    /      .'   \ '   /
   `--.  |     | | \  |  |    |   /  \      `./        |,_-' |__-' |     |     |    |__.   |          |
      |  |     | |  \ |  |    |  /---'\     ,'         |     |  \  |     |     |    |      |          |
 \___.'   `._.'  |   \|  /---/ ,'      \ _-'           /     /   \  `.__.'  `--/    /----/  `.__,     /


	Founder:
		- Duy Dang
		- Thom

	Server Developer:
		- Duy Dang

	Gameplay Designer:
		- Thom
		- Brian
		- lillil

	Graphic Designer:
		- Faye

	This tiny project started on Sunday 01/03/2021.
*/

#define PP_SYNTAX_AWAIT
#define YSI_YES_HEAP_MALLOC
#define YSI_NO_MODE_CACHE

#define task_unblock(%0) \
	task_detach(); \
	task_yield(%0)

#include <a_samp>
#include <YSI_Coding\y_hooks>
#include <ColAndreas>

DEFINE_HOOK_REPLACEMENT__(Destination, Dest);

#include <mysql>
#include <vehicle>
#include <item>
#include <inventory>
#include <weapon>
#include <player>
#include <admin>
#include <door>
#include <house>
#include <faction>
#include <farmer>

main() {}

forward pp_on_error(source[], message[], error_level:level, &retval);
public pp_on_error(source[], message[], error_level:level, &retval) {
	print(source);
	print(message);
	printf("%d %d", _:level, retval);
	return 1;
}

hook OnGameModeInit() {
	CA_Init();
	return 1;
}