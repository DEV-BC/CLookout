#include <ncurses.h>
#include "colors.h"

void colors_init(void) {
    start_color();

    init_color(C_BASE,   118, 118, 180);
    init_color(C_TEXT,   804, 839, 957);
    init_color(C_PINK,   953, 545, 659);
    init_color(C_TEAL,   580, 886, 835);
    init_color(C_YELLOW, 976, 886, 686);
    init_color(C_MUTED,  424, 439, 525);

    init_pair(PAIR_NORMAL, C_TEXT,  C_BASE);
    init_pair(PAIR_TITLE,  C_PINK,  C_BASE);
    init_pair(PAIR_NAV,    C_TEAL,  C_BASE);
    init_pair(PAIR_STATUS, C_MUTED, C_BASE);
}