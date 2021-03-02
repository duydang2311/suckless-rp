/*

   _____ .     . __    _ .___       .    _      _      .___  .___    ___    _______ .____    ___   _______
  (      /     / |\   |  /   `     /|     `.   /       /   \ /   \ .'   `. '   /    /      .'   \ '   /
   `--.  |     | | \  |  |    |   /  \      `./        |,_-' |__-' |     |     |    |__.   |          |
      |  |     | |  \ |  |    |  /---'\     ,'         |     |  \  |     |     |    |      |          |
 \___.'   `._.'  |   \|  /---/ ,'      \ _-'           /     /   \  `.__.'  `--/    /----/  `.__,     /


	Founder:
		- Duy Dang
		- Thom

	Gameplay Developer:
		- Duy Dang

	Gameplay Designer:
		- Thom

	This tiny project started on Sunday 01/03/2021.
*/

#define PP_SYNTAX_AWAIT
#define YSI_YES_HEAP_MALLOC

#define task_unblock(%0) \
	task_detach(); \
	task_yield(%0)

#include <mysql>
#include <player>

main() {}