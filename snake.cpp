// snake.cpp
// =========
//
// Snake game using ncurses

#include <ncurses.h>
// #include <unistd.h> // needed if using sleep() or usleep()

// Statics
static const char snake_piece = '*',
    food = 'X';
const int game_speed = 10; // Parameter for halfdelay function in tenths of a second

// Define Coord structure, to hold row & col coordinates
struct Coord {  // Struct that contains a pair of screen coordinates (y, x)
        int row, col;
};

// Class definition
class Snake {

    WINDOW *win;    // The window to display the snake in
    Coord *coords;  // Pointer to an array of coordinates
                    // The array is allocated at runtime in the constructor.

    int head, tail; // Indices into the coords, identifying where the head and tail are.
    int max_length; // Maximum length (also the size of the coords array)

    int max_row, max_col; // Maximum row & col position

    int direction;  // Direction the snake is moving in. For this we will
                    // use the constants KEY_UP, KEY_LEFT etc.

public:
    // Constructor. 'size' is the maximum size the snake is allowed to grow to.
    Snake(WINDOW *w) {
        win = w;
        getmaxyx(w, max_row, max_col);
        max_length = max_row * max_col;
        coords = new Coord[max_length];
        head = 0;
        tail = 0;
        direction = KEY_LEFT;
    };

    ~Snake() {  // Default destructor
        delete coords;
    };
/* Not needed?
    Coord *get_position() {
        // Return the snake head's current position on screen
        return &coords[head];
    }
*/
    int get_direction() {
        return direction;
    }

    void set_direction(int d) {
        direction = d;
    }

    void start() {
        // Display a new snake (i.e. head/tail only).
        // Set the coordinates as the middle of the window

        coords[0].row = max_row / 2;
        coords[0].col = max_col / 2;
        mvwaddch(win, coords[0].row, coords[0].col, snake_piece);
        wrefresh(win);
    };

    int out_of_bounds() {
        // Check if snake is about to go out of bounds
        int row = coords[head].row, col = coords[head].col;

        if ((row == 0 and direction == KEY_UP) or
            (row == max_row-1 and direction == KEY_DOWN) or
            (col == 0 and direction == KEY_LEFT) or
            (col == max_col-1 and direction == KEY_RIGHT))
            return 1;
        else return 0;
    }

    void advance() {
        // Advance the snake by adding a new head, and deleting old tail if needed.
        // Also update the coords array, which tracks where the snake has been.

        int old_row = coords[head].row, old_col = coords[head].col;
        
        // Advance the head in the coordinates array.
        // If the head reaches the end of the array, then start again
        // at position 0.
        // This is implemented by using the modulo of the
        // maximum length.
        head = (head + 1) % max_length;
 
        switch(direction) {
            case KEY_LEFT:
                coords[head].row = old_row;
                coords[head].col = old_col - 1;

                if ((winch(win) & A_CHARTEXT) != food) {
                    // No food here, so remove the tail
                    mvwaddch(win, coords[tail].row, coords[tail].col, ' ');
                    // Advance the tail in the coordinates array
                    tail = (tail + 1) % max_length;  
                }
                break;
            case KEY_RIGHT:

                break;
            case KEY_UP:
            case KEY_DOWN:
                break;
        };

        // Plot new head piece
        mvwaddch(win, coords[head].row, coords[head].col, snake_piece);
    };
};

int main() {

    // Create overall window. This will display menu and status bar,
    // and also contain the game animation window 'gamewin'
    initscr();
    start_color();

    // Create gameplay window 'gamewin'. Same width as stdscr
    // but two rows shorter to allow for menu and status bars.
    WINDOW *gamewin = newwin(LINES - 2, COLS, 1, 0);
#ifdef DEBUG
    // While in dev - colour the background so we can see it
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    wbkgd(gamewin, COLOR_PAIR(1));
    wborder(gamewin, 0,0,0,0,0,0,0,0);
    mvwprintw(gamewin, 2, 2, "TESTING");
#endif

    // key handling options
    cbreak();
    keypad(stdscr, TRUE);
    keypad(gamewin, TRUE);
    noecho();
    curs_set(0);

#ifndef DEBUG  // Use blocking mode while debugging
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
    Snake snake(gamewin);

    snake.start();

    // Main program loop

    int c;  // keyboard input
    int game_over = 0;  // When becomes 1, game over
    // Coord *rowcol;
    int direction;

    while(!game_over) {
        c = getch();    // Read keystroke into buffer

        direction = snake.get_direction();

        switch(c) {
            case KEY_UP:
                if (direction == KEY_DOWN) { // Crashed back onto yourself
                    game_over = 1;
                    break;
                }
                snake.set_direction(KEY_UP);
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

        // Check for moving out of bounds
        if (snake.out_of_bounds())
            game_over = 1;

        if (!game_over)
            snake.advance();
        
        refresh();
        wrefresh(gamewin);
    }

/* STUB - 'Game Over' point here */

    // Cleanup and exit
    delwin(gamewin);
    endwin();
}
