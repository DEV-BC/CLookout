#include <ncurses.h>
#include "colors.h"
#include "layout.h"

int main(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    colors_init();

    Layout *l = layout_create();
    if (l == NULL) {
        endwin();
        return 1;
    }

    layout_draw(l);
    layout_refresh(l);

    while (getch() != 'q') {}

    layout_free(l);
    endwin();
    return 0;
}