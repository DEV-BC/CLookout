#include <stdlib.h>
#include <ncurses.h>
#include "layout.h"
#include "colors.h"

static const char *section_names[] = {
    "Devices",
    "Incidents",
    "Todos",
    "AI"
};

static const char *nav_items[] = {
    "[D] Devices",
    "[I] Incidents",
    "[T] Todos",
    "[A] AI"
};

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

    l->active = SECTION_DEVICES;
    l->cursor = 0;

    return l;
}

void layout_draw(const Layout *l) {
    werase(l->nav);
    box(l->nav,        0, 0);
    for (int i = 0; i < 4; i++) {
        if (i == (int)l->active) {
            wattron(l->nav, COLOR_PAIR(PAIR_NAV) | A_REVERSE);
        } else {
            wattron(l->nav, COLOR_PAIR(PAIR_NAV));
        }
         mvwprintw(l->nav, i + 1, 2, "%s", nav_items[i]);
        wattroff(l->nav, COLOR_PAIR(PAIR_NAV) | A_REVERSE);
    }
     werase(l->main_panel);
    box(l->main_panel, 0, 0);
    wattron(l->main_panel, COLOR_PAIR(PAIR_TITLE));
    mvwprintw(l->main_panel, 1, 2, "%s", section_names[l->active]);
    wattroff(l->main_panel, COLOR_PAIR(PAIR_TITLE));

    const char *placeholder[] = {
        "item-01", "item-02", "item-03",
        "item-04", "item-05", "item-06"
    };
    for (int i = 0; i < 6; i++) {
        if (i == l->cursor) {
            wattron(l->main_panel, COLOR_PAIR(PAIR_NAV) | A_REVERSE);
        } else {
            wattron(l->main_panel, COLOR_PAIR(PAIR_NORMAL));
        }
        mvwprintw(l->main_panel, i + 3, 2, "%s", placeholder[i]);
        wattroff(l->main_panel, COLOR_PAIR(PAIR_NAV) | A_REVERSE);
    }

    werase(l->detail);
    box(l->detail, 0, 0);
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