#ifndef LAYOUT_H
#define LAYOUT_H

#include <ncurses.h>

#define NAV_WIDTH 20
#define DETAIL_WIDTH 35

typedef struct {
    WINDOW *nav;
    WINDOW *main_panel;
    WINDOW *detail;
} Layout;

Layout *layout_create(void);
void layout_draw(const Layout *l);
void layout_refresh(const Layout *l);
void layout_free(Layout *l);
#endif