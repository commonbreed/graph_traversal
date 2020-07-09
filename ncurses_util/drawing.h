#ifndef GT_NCURSES_UTIL_DRAWING
#define GT_NCURSES_UTIL_DRAWING

#include <locale.h>
#include <ncurses.h>
#include <termios.h>

// Initialise ncurses for the purposes of graph_traversal
int __init_ncurses();
int __end_ncurses();

#endif /* GT_NCURSES_UTIL_DRAWING */
