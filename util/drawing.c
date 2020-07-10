#include "util/drawing.h"

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
	//struct termios settings;

	//tcgetattr(0, &settings);
	//settings.c_oflag |= ONLCR;
	//tcsetattr(0, TCSADRAIN, &settings);
#endif /* XSI_SUPPORTED */

	clear();
	refresh();
	return 0;  // Always succeed
}

// This is SUPER ugly... in fact (n)curses should probably be committed to hell too...
int __before_place_next_node(size_t nlen) {
	int x;
	getcurx(stdscr);
	x+=nlen+5; // Edges: 2, arrow: 3
	move(0, x);
	refresh();

	return 0; // Always succeed
}

int __node_cr(size_t nlen) {
	int x, y;
	getyx(stdscr, y, x);
	x-=nlen+5;
	y++;
	move(y, x);
	refresh();

	return 0; // Always succeed
}

int __end_ncurses() {
	endwin();
}
