// snake.cpp
// =========
//
// Snake game using ncurses
//
// Copyright (C) Ernold C. McPuvlist, 2023

// TODO
//

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

// Forward declarations
static void plot_food(WINDOW *w);
static int random_range(int min, int max);

// Constants for character display
enum : chtype { SNAKE_PIECE = 'O', FOOD_PIECE = '*' };

// Class definitions

class GameSpeed {  // Controls for the game speed (window timeout)
    private:
    int min_timeout;    // minimum value for wtimeout() (slowest speed)
    int max_timeout;    // maximum value for wtimeout() (fastest speed)
    int max_score;      // highest the score could get

    public:
    // Constructor
    GameSpeed(int mint, int maxt, int mscore) {
        min_timeout = mint;
        max_timeout = maxt;
        max_score = mscore;
    }

    void set_timeout(WINDOW *w, int score) {
        // Set the timeout on window w.
        // This is calculated as the maximum timeout,
        // minus the timeout range divided by the score range.
        wtimeout(w, max_timeout - score * (max_timeout - min_timeout) / max_score);
    }
};

class Snake {

    // Define Coord structure, to hold row & col coordinates
    struct Coord {  // Struct that contains a pair of screen coordinates (row, col)
        int row, col;
    };

    WINDOW *win;    // The window to display the snake in.
    Coord *coords;  // Pointer to an array of coordinates.
                    // The array is allocated at runtime in the constructor.

    int head, tail; // Indices into the coords, identifying where the head and tail are.
    int max_length; // Maximum length (also the size of the coords array)

    int max_row, max_col; // Maximum row & col position

    int direction;  // Direction the snake is moving in. For this we will
                    // use the constants KEY_UP, KEY_LEFT etc.

public:
    // Constructor
    Snake(WINDOW *w) {
        win = w; // The ncurses window that the snake will be drawn in.

        getmaxyx(w, max_row, max_col);
        // Window max row ranges from 0 to max_row, same for max col.
        // The snake's allowed area will be between 1 and
        // (window max - 1), to allow for the inner border.
        // Therefore subtract 2 to get max_row & max_col.
        max_row -= 2; max_col -= 2;

        max_length = max_row * max_col;
        coords = new Coord[max_length];
        head = 0;
        tail = 0;
        // 'direction' property not explicitly set - 
        // it will be set in the 'init()' method
    };

    ~Snake() {  // Destructor
        delete coords;
    };

    int get_max_length() {
        return max_length;
    }

    int get_direction() {
        return direction;
    }

    void set_direction(int d) {
        direction = d;
    }

    void init() {
        // Reset the snake to one piece, and plot to window.

        enum {MARGIN=5};

        // Reset the game window appearance
        wclear(win);
        wbkgd(win, COLOR_PAIR(2));
        wattrset(win, COLOR_PAIR(2) | A_BOLD);
        box(win, 0, 0);
        wattrset(win, COLOR_PAIR(3) | A_BOLD);

        tail = head;  // Tail and head the same coord index, so snake is 1 unit long

        // Randomise starting coordinates. Keep [margin] cells away from the edge,
        // to avoid an immediate crash.
        coords[head].row = random_range(MARGIN, max_row - MARGIN);
        coords[head].col = random_range(MARGIN, max_col - MARGIN);

        // Add the piece
        mvwaddch(win, coords[head].row, coords[head].col, SNAKE_PIECE);

        // Randomise starting direction
        direction = random_range(KEY_DOWN, KEY_RIGHT);
    };

    int can_advance() {
        // Check if snake has a clear path ahead.
        // Returns 1 if clear ahead, 0 otherwise
        int row = coords[head].row, col = coords[head].col;

        // Check if hitting the edge of the window
        if ((row == 1 and direction == KEY_UP) or
            (row == max_row and direction == KEY_DOWN) or
            (col == 1 and direction == KEY_LEFT) or
            (col == max_col and direction == KEY_RIGHT))
            return 0;
        else return 1;
    }

    int advance(int *score_ptr, GameSpeed *gs) {

        // Advance the snake by adding a new head, and deleting old tail if 
        // not on a food piece.
        // Also update the coords array, which tracks where the snake has been.
        // Returns 1 if successful, 0 if hit another part of the snake.
        // Arguments:
        // score_ptr - pointer to the score, so that it can be updated.
        // gs - GameSpeed object

        int old_row = coords[head].row, old_col = coords[head].col;
        int retval;  // Return value

        // Advance the head in the coordinates array.
        // If the head reaches the end of the array, then start again
        // at position 0.
        // This is implemented by using the modulo of the
        // maximum length.
        head = (head + 1) % max_length;

        switch(direction) {
            // Update the coords array with new head position
            case KEY_LEFT:
                coords[head].row = old_row;
                coords[head].col = old_col - 1;
                break;
            case KEY_RIGHT:
                coords[head].row = old_row;
                coords[head].col = old_col + 1;
                break;
            case KEY_UP:
                coords[head].row = old_row - 1;
                coords[head].col = old_col;
                break;
            case KEY_DOWN:
                coords[head].row = old_row + 1;
                coords[head].col = old_col;
                break;
        };

        // Move cursor to new head position
        wmove(win, coords[head].row, coords[head].col);

        char char_in_situ = winch(win) & A_CHARTEXT;

        // Inspect what character is currently at the current screen position
        switch (char_in_situ) {
            case FOOD_PIECE:
                // Food piece hit - plot new head but don't erase tail
                (*score_ptr)++;  // Increase and display the score
                mvprintw(0,8, "%5d", *score_ptr);
                refresh();
                // Increase game speed
                gs->set_timeout(win, *score_ptr);

                waddch(win, SNAKE_PIECE);
                plot_food(win);  // Plot another food piece
                retval = 1;
                break;
        case SNAKE_PIECE:
            // We've hit the snake - Game Over
            retval = 0;
            break;
        default:
            // No food here, plot head and remove the tail
            waddch(win, SNAKE_PIECE);
            mvwaddch(win, coords[tail].row, coords[tail].col, ' ');
            // Advance the tail in the coordinates array
            tail = (tail + 1) % max_length;
            retval = 1;
        }
#ifdef DEBUG
        // Display the head and tail indices
        mvprintw(1, 0, "H: %05d", head);
        mvprintw(1, 10, "T: %05d", tail);
#endif
    return retval;
    };
};

static void plot_food(WINDOW *win) {
    // Plot a new piece of food in a random location

    int maxx, maxy, row, col;
    char screen_cell;

    getmaxyx(win, maxx, maxy);

    // Keep hunting for a blank cell to put the food in
    do {
        row = random_range(0, maxy);
        col = random_range(0, maxx);
        screen_cell = mvwinch(win, row, col) & A_CHARTEXT;
    }
    while (screen_cell != ' ');

    waddch(win, FOOD_PIECE);
}

static int random_range(int min, int max) {
    // Return a random integer in the range min-max
    return rand() % (max - min + 1) + min; 
}

int main() {

    int c;  // for keyboard input
    int game_over;  // When becomes 1, game over
    int direction;
    int score;
    int old_score;

    enum {  // game window size
        GAMEWIN_HEIGHT = 15, GAMEWIN_WIDTH = 30
    };

    // Create overall window. This will display menu and status bar,
    // and also contain the game animation window 'gamewin'
    initscr();
    start_color();

    // Seed the random generator
    srand(time(NULL));

    // Create the gameplay window 'gamewin'.
    if (LINES < GAMEWIN_HEIGHT+1 or COLS < GAMEWIN_WIDTH) {
        fprintf(stderr, "Your terminal is not large enough to display the game window. ");
        fprintf(stderr, "Please re-size your terminal to at least %d x %d ", GAMEWIN_WIDTH, GAMEWIN_HEIGHT);
        fprintf(stderr, "characters before re-launching the program\n");
        endwin();
        return 1;
    }
    WINDOW *gamewin = newwin(GAMEWIN_HEIGHT, GAMEWIN_WIDTH, (LINES-GAMEWIN_HEIGHT) / 2, (COLS-GAMEWIN_WIDTH) / 2);

    // Create the pop-up "Game Over" messagebox window
    WINDOW *gameoverwin = newwin(4, 29, (LINES-4) / 2, (COLS-29) / 2);

    // Initialise the colour pairs that will be used
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Default, and for menu key choices
    init_pair(2, COLOR_WHITE, COLOR_BLUE);  // For gameplay window and border
    init_pair(3, COLOR_YELLOW, COLOR_BLUE); // For gameplay window and snake
    init_pair(4, COLOR_BLACK, COLOR_CYAN);  // For menu item text
    init_pair(5, COLOR_BLACK, COLOR_RED);   // For 'Game Over' text
    init_pair(6, COLOR_BLACK, COLOR_WHITE); // For the score display

    // Set up the 'Game Over' popup window
    wattrset(gameoverwin, COLOR_PAIR(1) | A_BOLD);
    box(gameoverwin, 0, 0);
    wattrset(gameoverwin, COLOR_PAIR(5) | A_BOLD);
    mvwprintw(gameoverwin, 1, 6, " ** GAME OVER **");
    wattrset(gameoverwin, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gameoverwin, 2, 2, "Press any key to continue");

    // key handling options
    cbreak();
    keypad(gamewin, TRUE);
    noecho();
    curs_set(0);

    nodelay(stdscr, false); // Blocking mode while in main menu loop

    // Set up the containing screen
    attrset(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 1, "Score:");
    mvprintw(0, 23, "S");
    mvprintw(0, 40, "Q");
    attrset(COLOR_PAIR(4));
    mvprintw(0, 25, "Start new game");
    mvprintw(0, 42, "Quit");
    attrset(COLOR_PAIR(6)); // for remaining display of score

    // Create the snake object.
    Snake snake(gamewin);

    // Create the game speed object
    // Range from 300 ms (starting) to 50 ms (fastest)
    GameSpeed gamespeed(50, 300, snake.get_max_length());

    // ===================
    // Main control loop
    // ===================

    c = getch();  // Wait for key. This will also call a refresh on stdscr
    while (c != 'q' and c !='Q') {

        if (c == 's' or c == 'S') {
        // Start a new game

            // Reset game state
            game_over = 0;
            score = old_score = 0;

            snake.init();
            mvprintw(0,8, "%5d", score);
            refresh();

            // Place a piece of food
            plot_food(gamewin);

            // Set starting speed
            gamespeed.set_timeout(gamewin, 0);

            // Main gameplay loop

            while(!game_over) {
                c = wgetch(gamewin);    // Read keystroke into buffer

                direction = snake.get_direction();

                switch(c) {
                    // Update the direction based on what key has been pressed
                    case KEY_UP:
                        if (direction == KEY_DOWN) { // Crashed up into yourself
                            game_over = 1;
                            break;
                        }
                        snake.set_direction(KEY_UP);
                        break;
                    case KEY_DOWN:
                        if (direction == KEY_UP) { // Crashed down into yourself
                            game_over = 1;
                            break;
                        }
                        snake.set_direction(KEY_DOWN);
                        break;
                    case KEY_LEFT:
                        if (direction == KEY_RIGHT) { // Crashed left into yourself
                            game_over = 1;
                            break;
                        }
                        snake.set_direction(KEY_LEFT);
                        break;
                    case KEY_RIGHT:
                        if (direction == KEY_LEFT) { // Crashed right into yourself
                            game_over = 1;
                            break;
                        }
                        snake.set_direction(KEY_RIGHT);
                    // Ignore all other key presses
                }

                // Check for moving out of bounds
                if (!snake.can_advance())
                    game_over = 1;

                if (!game_over)
                    if (!snake.advance(&score, &gamespeed))   // Advance the snake. If an error is returned,
                        game_over = 1;      // the snake has collided with itself, game over
            }

            // ** Game Over **
            touchwin(gameoverwin);
            wrefresh(gameoverwin); // Display 'Game Over' popup and wait for key
            wgetch(gameoverwin);
            touchwin(gamewin);
            wrefresh(gamewin);

            // Reset keyboard input to blocking
            wtimeout(gamewin, -1);
        }
        // More 'if's can go here if other menu options wanted
        c = getch();
    }

    // Cleanup and exit
    delwin(gamewin);
    delwin(gameoverwin);
    endwin();

    return 0;
}
