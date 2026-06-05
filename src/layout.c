#include <stdlib.h>
#include <ncurses.h>
#include "layout.h"
#include "app_data.h"
#include "colors.h"

static const char *section_names[] = {
    "Devices",
    "Incidents",
    "Todos",
    "AI"
};

static int severity_pair(Severity s) {
    switch (s) {
        case SEVERITY_LOW:      return PAIR_STATUS;
        case SEVERITY_MEDIUM:   return PAIR_NORMAL;
        case SEVERITY_HIGH:     return PAIR_TITLE;
        case SEVERITY_CRITICAL: return PAIR_TITLE;
        default:                return PAIR_NORMAL;
    }
}

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

void layout_draw(const Layout *l, const AppData *data) {
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

    if (l->active == SECTION_DEVICES) {
        for (int i = 0; i < data->device_count; i++) {
            int pair = data->devices[i]->online ? PAIR_NAV : PAIR_STATUS;
            if (i == l->cursor)
                wattron(l->main_panel, COLOR_PAIR(pair) | A_REVERSE);
            else
                wattron(l->main_panel, COLOR_PAIR(pair));
            mvwprintw(l->main_panel, i + 3, 2, "%-20s [%s]",
                      data->devices[i]->name,
                      data->devices[i]->online ? "online" : "offline");
            wattroff(l->main_panel, COLOR_PAIR(pair) | A_REVERSE);
        }
    
    } else if (l->active == SECTION_INCIDENTS) {
        for (int i = 0; i < data->incident_count; i++) {
            int pair = severity_pair(data->incidents[i]->severity);
            if (i == l->cursor)
                wattron(l->main_panel, COLOR_PAIR(pair) | A_REVERSE);
            else
                wattron(l->main_panel, COLOR_PAIR(pair));
            mvwprintw(l->main_panel, i + 3, 2, "%-32s [%s]",
                      data->incidents[i]->title,
                      data->incidents[i]->status == STATUS_RESOLVED
                          ? "resolved" : "open");
            wattroff(l->main_panel, COLOR_PAIR(pair) | A_REVERSE);
        }
    } else if (l->active == SECTION_TODOS) {
        for (int i = 0; i < data->todo_count; i++) {
            int pair = data->todos[i]->done ? PAIR_STATUS : PAIR_NORMAL;
            if (i == l->cursor)
                wattron(l->main_panel, COLOR_PAIR(pair) | A_REVERSE);
            else
                wattron(l->main_panel, COLOR_PAIR(pair));
            mvwprintw(l->main_panel, i + 3, 2, "[%c] P%d  %s",
                      data->todos[i]->done ? 'x' : ' ',
                      data->todos[i]->priority,
                      data->todos[i]->title);
            wattroff(l->main_panel, COLOR_PAIR(pair) | A_REVERSE);
        }
    }

   werase(l->detail);
    box(l->detail, 0, 0);
    wattron(l->detail, COLOR_PAIR(PAIR_TITLE));
     const char *detail_titles[] = { "Device Detail", "Incident Detail", "Todo Detail", "AI Chat" };
    mvwprintw(l->detail, 1, 2, "%s", detail_titles[l->active]);
    wattroff(l->detail, COLOR_PAIR(PAIR_TITLE));

     if (l->active == SECTION_DEVICES && data->device_count > 0) {
        Device *sel = data->devices[l->cursor];
        int pair = sel->online ? PAIR_NAV : PAIR_STATUS;
        mvwprintw(l->detail, 3, 2, "ID:     %d", sel->id);
        wattron(l->detail, COLOR_PAIR(PAIR_TITLE));
        mvwprintw(l->detail, 4, 2, "Name:   %s", sel->name);
        wattroff(l->detail, COLOR_PAIR(PAIR_TITLE));
        wattron(l->detail, COLOR_PAIR(pair));
        mvwprintw(l->detail, 5, 2, "Status: %s",
                  sel->online ? "ONLINE" : "OFFLINE");
        wattroff(l->detail, COLOR_PAIR(pair));

    } else if (l->active == SECTION_INCIDENTS && data->incident_count > 0) {
        Incident *sel = data->incidents[l->cursor];
        int pair = severity_pair(sel->severity);
        const char *sev[] = { "LOW", "MEDIUM", "HIGH", "CRITICAL" };
        const char *sta[] = { "OPEN", "INVESTIGATING", "RESOLVED" };
        mvwprintw(l->detail, 3, 2, "ID:       %d", sel->id);
        wattron(l->detail, COLOR_PAIR(PAIR_TITLE));
        mvwprintw(l->detail, 4, 2, "Title:");
        mvwprintw(l->detail, 5, 2, "  %.28s", sel->title);
        wattroff(l->detail, COLOR_PAIR(PAIR_TITLE));
        wattron(l->detail, COLOR_PAIR(pair));
        mvwprintw(l->detail, 6, 2, "Severity: %s", sev[sel->severity]);
        wattroff(l->detail, COLOR_PAIR(pair));
        mvwprintw(l->detail, 7, 2, "Status:   %s", sta[sel->status]);
    } else if (l->active == SECTION_TODOS && data->todo_count > 0) {
        Todo *sel = data->todos[l->cursor];
        int pair = sel->done ? PAIR_STATUS : PAIR_NORMAL;
        mvwprintw(l->detail, 3, 2, "ID:       %d", sel->id);
        wattron(l->detail, COLOR_PAIR(PAIR_TITLE));
        mvwprintw(l->detail, 4, 2, "Title:");
        mvwprintw(l->detail, 5, 2, "  %.28s", sel->title);
        wattroff(l->detail, COLOR_PAIR(PAIR_TITLE));
        mvwprintw(l->detail, 6, 2, "Priority: %d", sel->priority);
        wattron(l->detail, COLOR_PAIR(pair));
        mvwprintw(l->detail, 7, 2, "Status:   %s", sel->done ? "DONE" : "PENDING");
        wattroff(l->detail, COLOR_PAIR(pair));
    }
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