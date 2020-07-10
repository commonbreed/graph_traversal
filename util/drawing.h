#ifndef GT_NCURSES_UTIL_DRAWING
#define GT_NCURSES_UTIL_DRAWING

//We require XSI compliance for the ONLCR terminal flag
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#include <unistd.h>
#if (_XOPEN_UNIX > -1)
#define XSI_SUPPORTED
#endif /* _XOPEN_UNIX */
#endif /* unix */

// Initialise ncurses for the purposes of graph_traversal
int __init_ncurses();
int __end_ncurses();

#endif /* GT_NCURSES_UTIL_DRAWING */
