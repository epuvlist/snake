#include <ncurses.h>
// #include <unistd.h>

// Statics
static const char snake_piece = '*',
    food = 'X';

class Snake {

    struct Coord {  // Struct that contains a pair of screen coordinates (y, x)
        int y, x;
    };

    Coord *coords;  // Pointer to an array of coordinates
                    // The array is allocated at runtime in the constructor.

    int head, tail; // Indices into the coords, identifying where the head and tail are.
    int direction;  // Direction the snake is moving in. For this we will
                    // use the constants KEY_UP, KEY_LEFT etc.
    int max_length; // Maximum length (also the size of the coords array)

public:
    // Constructor. 'size' is the maximum size the snake is allowed to grow to.
    Snake(int size) {
        coords = new Coord[size];
        head = 0;
        tail = 0;
        direction = KEY_LEFT;
        max_length = size;
    };

    ~Snake() {  // Default destructor
        delete coords;
    };

    int row() {
        // Return the snake's current row on screen
        return coords[head].y;
    }

    int col() {
        // Return the snake's current column on screen
        return coords[head].x;
        }

    void start(WINDOW *win) {
        // Display a new snake (i.e. head/tail only).
        // Set the coordinates as the middle of the window

        int rows, cols;
        getmaxyx(win, rows, cols);
        coords[0].y = rows / 2;
        coords[0].x = cols / 2;
        wmove(win, coords[0].y, coords[0].x);
        waddch(win, snake_piece);
        wrefresh(win);
    };

    void advance(WINDOW *win) {
        // Add a new head, and delete old tail if needed..
        // Also update the coords array, which tracks where the snake has been.

        int old_row = coords[head].y, old_col = coords[head].x;
        
        // Advance the head in the coordinates array.
        // If the head reaches the end of the array, then start again
        // at position 0.
        // This is implemented by using the modulo of the
        // maximum length.
        head = (head + 1) % max_length;
 
        switch(direction) {
            case KEY_LEFT:
                coords[head].y = old_row;
                coords[head].x = old_col - 1;

                if (winch(win) && A_CHARTEXT != food) {
                    // No food here, so remove the tail
                    mvwaddch(win, coords[tail].y, coords[tail].x, ' ');
                    // Advance the tail in the coordinates array
                    tail = (tail + 1) % max_length;  
                }
                break;
            case KEY_RIGHT:
                coords[head].y = old_row;
                coords[head].x = old_col + 1;
                break;
            case KEY_UP:
            case KEY_DOWN:
                break;
        };

        // Plot new head piece
        mvwaddch(win, coords[head].y, coords[head].x, snake_piece);
    };
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
    curs_set(0);

#ifndef DEBUG
    halfdelay(10);  // 1 second tick by default
#endif

    // STUB - placeholder menus
    printw("Menu goes here");
    mvprintw(LINES - 1, 0, "Status goes here");
    refresh();

    // Create the snake object.
    // Maximum length is gamewin rows x gamewin columns
    int gmrows, gmcols;
    getmaxyx(gamewin, gmrows, gmcols);
    Snake snake(gmrows * gmcols);

    snake.start(gamewin);

#ifdef DEBUG
    wgetch(gamewin);
    snake.advance(gamewin);
    wgetch(gamewin);
#endif

    // Main program loop
    int c;  // character input
/*
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
*/
    // Cleanup and exit
    delwin(gamewin);
    endwin();
}
