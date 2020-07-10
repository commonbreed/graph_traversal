#include "ncurses_util/drawing.h"

#include <locale.h>
#include <ncurses.h>
#include <termios.h>

int __init_ncurses() {
	setlocale(LC_CTYPE, "");
	initscr(); // General init
	raw();     // Pass control characters directly to application
	noecho();  // Don't relay typed characters

#ifdef XSI_SUPPORTED
	// Use termios to re-enable automatic carriage returns
	// This is an alternative to using -DONLCR when compiling ncurses
	struct termios settings;

	tcgetattr(0, &settings);
	settings.c_oflag |= ONLCR;
	tcsetattr(0, TCSADRAIN, &settings);
#endif /* XSI_SUPPORTED */
	return 0;  // Always succeed
}

int __end_ncurses() {
	endwin();
}
