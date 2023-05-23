#include <ncurses.h>

class Snake {

    struct Coord {  // Pair of screen coordinates (x,y)
        int x, y;
    };

    Coord *coords;  // pointer to the array of coordinates
                    // The array is allocated at runtime using the constructor

public:
    // Constructor. size is the maximum size the snake is allowed to grow to.
    Snake(int size) {
        coords = new Coord[size];
    };

    ~Snake() {  // Default destructor
        delete coords;
    }

};

int main() {

    // Create overall window. This will display menu and status bar,
    // and also contain the game animation window 'gamewin'
    initscr();
    start_color();

    // Create game animation window 'gamewin'. Same width as main window, 
    // but two rows shorter to allow for menu and status bar.
    int h, w;
    getmaxyx(stdscr, h, w);
    WINDOW *gamewin = newwin(h-2, w, 1, 0);
    // STUB - colour the background so we can see it
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    wbkgd(gamewin, COLOR_PAIR(1));

    // key handling options
    cbreak();

    // STUB - placeholder menus
    printw("Menu goes here");
    mvprintw(h-1, 0, "Status goes here");

    refresh();
    wrefresh(gamewin);

    getch();
    delwin(gamewin);
    endwin();
}