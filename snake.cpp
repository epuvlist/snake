#include <ncurses.h>
#include <unistd.h>

class Snake {

    struct Coord {  // Struct that contains a pair of screen coordinates (y, x)
        int y, x;
    };

    Coord *coords;  // Pointer to an array of coordinates
                    // The array is allocated at runtime in the constructor.

    int head, tail; // Indices into the coords, identifying where the head and tail are.
    int direction;  // Direction the snake is moving in. For this we will
                    // use the constants KEY_UP, KEY_LEFT etc.

public:
    // Constructor. 'size' is the maximum size the snake is allowed to grow to.
    Snake(int size) {
        coords = new Coord[size];
        head = 0;
        tail = 0;
        direction = KEY_RIGHT;
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

    // Create game animation window 'gamewin'. Same width as stdscr
    // but two rows shorter to allow for menu and status bars.
    WINDOW *gamewin = newwin(LINES - 2, COLS, 1, 0);
    // STUB - colour the background so we can see it
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    wbkgd(gamewin, COLOR_PAIR(1));
    wborder(gamewin, 0,0,0,0,0,0,0,0);
    mvwprintw(gamewin, 2, 2, "TESTING");

    // key handling options
    cbreak();
    keypad(stdscr, TRUE);
    keypad(gamewin, TRUE);
    noecho();
    halfdelay(10);  // 1 second tick by default

    // STUB - placeholder menus
    printw("Menu goes here");
    mvprintw(LINES - 1, 0, "Status goes here");

    // Main program loop
    int c;  // character input

    while(TRUE) {
        c = getch();    // Read any keystroke into buffer
        switch(c) {
            case KEY_UP:
                mvprintw(0, 0, "UP   ");
                break;
            case KEY_DOWN:
                mvprintw(0, 0, "DOWN ");
                break;
            case KEY_LEFT:
                mvprintw(0, 0, "LEFT ");
                break;
            case KEY_RIGHT:
                mvprintw(0, 0, "RIGHT");
        }
        if (c == 'q' or c == 'Q')
            break;
        refresh();
        wrefresh(gamewin);
    }

    // Cleanup and exit
    delwin(gamewin);
    endwin();
}
