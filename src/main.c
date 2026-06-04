#include <ncurses.h>

#define COLOR_BASE 16
#define COLOR_TEXT 17
#define COLOR_PINK 18
#define COLOR_TEAL 19

#define PAIR_TITLE 1
#define PAIR_NORMAL 2
#define PAIR_NAV 3

#define NAV_WIDTH 20



int main(void) {
    
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();

    
    init_color(COLOR_BASE, 118, 118, 180);   /* #1e1e2e — background */
    init_color(COLOR_TEXT, 804, 839, 957);   /* #cdd6f4 — bright text */
    init_color(COLOR_PINK, 953, 545, 659);   /* #f38ba8 — accent pink */
    init_color(COLOR_TEAL, 580, 886, 835);
    init_pair(PAIR_TITLE, COLOR_PINK, COLOR_BASE);
    init_pair(PAIR_NORMAL, COLOR_TEXT, COLOR_BASE);
    init_pair(PAIR_NAV,    COLOR_TEAL, COLOR_BASE);

    bkgd(COLOR_PAIR(PAIR_NORMAL));

    WINDOW *nav = newwin(LINES - 1, NAV_WIDTH, 0, 0);
    WINDOW *main_panel = newwin(LINES - 1, COLS - NAV_WIDTH, 0, NAV_WIDTH);

    wbkgd(nav, COLOR_PAIR(PAIR_NORMAL));
    wbkgd(main_panel, COLOR_PAIR(PAIR_NORMAL));

    box(nav, 0, 0);
    box(main_panel, 0, 0);
    wattron(nav, COLOR_PAIR(PAIR_NAV));
    mvwprintw(nav, 1, 2, "[D] Devices");
    mvwprintw(nav, 2, 2, "[I] Incidents");
    mvwprintw(nav, 3, 2, "[T] Todos");
    mvwprintw(nav, 4, 2, "[A] AI");
    wattroff(nav, COLOR_PAIR(PAIR_NAV));

    wattron(main_panel, COLOR_PAIR(PAIR_TITLE));
    mvwprintw(main_panel, 1, 2, "Devices");
    wattroff(main_panel, COLOR_PAIR(PAIR_TITLE));

    mvwprintw(stdscr, LINES - 1, 2, "q: quit");

    refresh();
    wrefresh(nav);
    wrefresh(main_panel);
    



    // mvprintw(LINES / 2, (COLS / 2) - 10, "Sentinel-C is alive.");
    // mvprintw(LINES / 2 + 1, (COLS / 2) - 10, "Press any key to exit.");
    // refresh();

    // int ch;
    // int row = LINES / 2;
    // int col = COLS / 2;

    // while ((ch = getch()) != 'q') {
        
    //     switch (ch) {
    //         case KEY_UP: row--; break;
    //         case KEY_DOWN: row++; break;
    //         case KEY_LEFT: col--; break;
    //         case KEY_RIGHT: col++; break;
    //     }
    //     clear();
    //     bkgd(COLOR_PAIR(PAIR_NORMAL));
    //     box(stdscr, 0, 0);
    //     attron(COLOR_PAIR(PAIR_TITLE));
    //     mvprintw(row, col, "Sentinel-C");
    //     attron(COLOR_PAIR(PAIR_TITLE));

    //     mvprintw(LINES - 2, 2, "Arrow keys move. q quits.");
    //     refresh();
    // }

    while (getch() != 'q') {
    
    }

    delwin(nav);
    delwin(main_panel);
    endwin();

    return 0;
}


