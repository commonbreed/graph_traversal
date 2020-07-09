#include "ncurses_util/drawing.h"

int __init_ncurses() {
	setlocale(LC_CTYPE, "");
	initscr(); // General init
	raw();     // Pass control characters directly to application
	noecho();  // Don't relay typed characters

	// Use termios to re-enable automatic carriage returns
	// This places ncurses in an unstable state, so revisit this and make sure nothing weird happens FIXME
	struct termios settings;

	tcgetattr(0, &settings);
	settings.c_oflag |= ONLCR;
	tcsetattr(0, TCSADRAIN, &settings);

	return 0;  // Always succeed
}

int __end_ncurses() {
	endwin();
}