#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "layout.h"
#include "colors.h"

Layout *layout_create(void) {
    Layout *l = malloc(sizeof(Layout));
    if (l == NULL) {
        return NULL;
    }

    int main_width = COLS - NAV_WIDTH - DETAIL_WIDTH;

    l->nav        = newwin(LINES - 1, NAV_WIDTH,    0, 0);
    l->main_panel = newwin(LINES - 1, main_width,   0, NAV_WIDTH);
    l->detail     = newwin(LINES - 1, DETAIL_WIDTH, 0, NAV_WIDTH + main_width);

    wbkgd(l->nav,        COLOR_PAIR(PAIR_NORMAL));
    wbkgd(l->main_panel, COLOR_PAIR(PAIR_NORMAL));
    wbkgd(l->detail,     COLOR_PAIR(PAIR_NORMAL));

    return l;
}

void layout_draw(const Layout *l) {
    box(l->nav,        0, 0);
    box(l->main_panel, 0, 0);
    box(l->detail,     0, 0);

    wattron(l->nav, COLOR_PAIR(PAIR_NAV));
    mvwprintw(l->nav, 1, 2, "[D] Devices");
    mvwprintw(l->nav, 2, 2, "[I] Incidents");
    mvwprintw(l->nav, 3, 2, "[T] Todos");
    mvwprintw(l->nav, 4, 2, "[A] AI");
    wattroff(l->nav, COLOR_PAIR(PAIR_NAV));

    wattron(l->main_panel, COLOR_PAIR(PAIR_TITLE));
    mvwprintw(l->main_panel, 1, 2, "Devices");
    wattroff(l->main_panel, COLOR_PAIR(PAIR_TITLE));

    wattron(l->detail, COLOR_PAIR(PAIR_TITLE));
    mvwprintw(l->detail, 1, 2, "Detail");
    wattroff(l->detail, COLOR_PAIR(PAIR_TITLE));

    attron(COLOR_PAIR(PAIR_STATUS));
    mvprintw(LINES - 1, 2, " D:Devices  I:Incidents  T:Todos  A:AI  q:Quit ");
    attroff(COLOR_PAIR(PAIR_STATUS));
}


void layout_refresh(const Layout *l) {
    refresh();
    wrefresh(l->nav);
    wrefresh(l->main_panel);
    wrefresh(l->detail);
}

void layout_free(Layout *l) {
    delwin(l->nav);
    delwin(l->main_panel);
    delwin(l->detail);
    free(l);
}