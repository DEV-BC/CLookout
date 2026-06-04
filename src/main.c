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
    if (l == NULL) { endwin(); return 1; }

    layout_draw(l);
    layout_refresh(l);

    int ch;
    while ((ch = getch()) != 'q') {
        switch (ch) {
            case 'd': case 'D': l->active = SECTION_DEVICES;   l->cursor = 0; break;
            case 'i': case 'I': l->active = SECTION_INCIDENTS; l->cursor = 0; break;
            case 't': case 'T': l->active = SECTION_TODOS;     l->cursor = 0; break;
            case 'a': case 'A': l->active = SECTION_AI;        l->cursor = 0; break; 
            case KEY_UP:
                if (l->cursor > 0) l->cursor--;
                break;
            case KEY_DOWN:
                if (l->cursor < 5) l->cursor++;
                break;
        }
        layout_draw(l);
        layout_refresh(l);
    }

    layout_free(l);
    endwin();
    return 0;
}