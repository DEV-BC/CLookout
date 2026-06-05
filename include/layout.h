#ifndef LAYOUT_H
#define LAYOUT_H

#include "app_data.h"
#include <ncurses.h>

#define NAV_WIDTH 20
#define DETAIL_WIDTH 35

typedef enum {
    SECTION_DEVICES,
    SECTION_INCIDENTS,
    SECTION_TODOS,
    SECTION_AI
} Section;

typedef struct {
    WINDOW *nav;
    WINDOW *main_panel;
    WINDOW *detail;
    Section active;
    int cursor;
} Layout;

Layout *layout_create(void);
void layout_draw(const Layout *l, const AppData *data);
void layout_refresh(const Layout *l);
void layout_free(Layout *l);
#endif