#ifndef COLORS_H
#define COLORS_H

#include <ncurses.h>

#define C_BASE    16
#define C_TEXT    17
#define C_PINK    18
#define C_TEAL    19
#define C_YELLOW  20
#define C_MUTED   21

#define PAIR_NORMAL   1
#define PAIR_TITLE    2
#define PAIR_NAV      3
#define PAIR_STATUS   4

void colors_init(void);


#endif