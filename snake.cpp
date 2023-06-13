// snake.cpp
// =========
//
// Snake game using ncurses
//
// Copyright (C) Ernold C. McPuvlist, 2023

// TODO
// 1. Make 'GAME OVER' have its own window - don't notice it otherwise.
// 2. Choose better snake piece and food characters
// 3. Improve colour scheme

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

// Forward declarations
void plot_food(WINDOW *w);
int random_range(int min, int max);

// Static globals
static const char snake_piece = '*',
    food = 'X';
static const int game_speed = 3; // Parameter for halfdelay function in tenths of a second
static const int gamewin_height = 20, gamewin_width = 30;  // Game window size

// Class definition
class Snake {

    // Define Coord structure, to hold row & col coordinates
    struct Coord {  // Struct that contains a pair of screen coordinates (y, x)
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
    // Constructor. 'w' is the ncurses window that the snake will be drawn in.
    // The snake is allowed to fill the entire window, so max_length is
    // max no. of rows times max no. of columns.
    Snake(WINDOW *w) {
        win = w;
        getmaxyx(w, max_row, max_col);
        max_length = max_row * max_col;
        coords = new Coord[max_length];
        head = 0;
        tail = 0;
        // 'direction' property not set - it will be set in the 'start()' method
    };

    ~Snake() {  // Destructor
        delete coords;
    };

    int get_direction() {
        return direction;
    }

    void set_direction(int d) {
        direction = d;
    }

    void start() {
        // Reset the snake and plot to window (i.e. head/tail only).

        const int margin = 5;

        wclear(win);
        
        tail = head;  // Tail and head the same coord index, so snake is 1 unit long

        // Randomise starting coordinates. Keep (margin) cells away from the edge,
        // to avoid an immediate crash.

// min row = margin; max row = max_row - margin
        coords[head].row = random_range(margin, max_row - margin);
        // coords[head].row = rand() % (max_row - margin * 2) + margin - 1;
        coords[head].col = random_range(margin, max_col - margin);
        // coords[head].col = rand() % (max_col - margin * 2) + margin - 1;
        mvwaddch(win, coords[head].row, coords[head].col, snake_piece);

        // Randomise starting direction
        direction = random_range(KEY_DOWN, KEY_RIGHT);
        // direction = rand() % (KEY_RIGHT - KEY_DOWN + 1) + KEY_DOWN;
    };

    int can_advance() {
        // Check if snake has a clear path ahead.
        // Returns 1 if clear ahead, 0 otherwise
        int row = coords[head].row, col = coords[head].col;

        // Check if hitting the edge of the window
        if ((row == 0 and direction == KEY_UP) or
            (row == max_row-1 and direction == KEY_DOWN) or
            (col == 0 and direction == KEY_LEFT) or
            (col == max_col-1 and direction == KEY_RIGHT))
            return 0;
        else return 1;
    }

    int advance(int *score_ptr) {
        // Advance the snake by adding a new head, and deleting old tail if 
        // not on a food piece.
        // Also update the coords array, which tracks where the snake has been.
        // Returns 1 if successful, 0 if hit another part of the snake.
        // Updates the score using score_ptr if necessary

        int old_row = coords[head].row, old_col = coords[head].col;
        int retval;  // Return value
        
        // Advance the head in the coordinates array.
        // If the head reaches the end of the array, then start again
        // at position 0.
        // This is implemented by using the modulo of the
        // maximum length.

        // TODO: Only need to increase the head index if the snake has grown?
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

        switch(winch(win) & A_CHARTEXT) {
            case food:
                // Food piece hit - plot new head but don't erase tail
                (*score_ptr)++;  // Increase the score
                waddch(win, snake_piece);
                plot_food(win);  // Plot another food piece
                retval = 1;
                break;
            case snake_piece:
                retval = 0;  // Game Over
                break;           
            default:
                // No food here, plot head and remove the tail
                waddch(win, snake_piece);
                mvwaddch(win, coords[tail].row, coords[tail].col, ' ');
                // Advance the tail in the coordinates array
                tail = (tail + 1) % max_length;
                retval = 1;
        }
#ifdef DEBUG
        // Display the head and tail indices
        mvprintw(0, 30, "H: %05d", head);
        mvprintw(0, 40, "T: %05d", tail);
#endif
    return retval;
    };
};

void plot_food(WINDOW *win) {
    // Plot a new piece of food in a random location

    int row, col;
    char screen_cell;
    
    do {
        // TODO - need to add 1?
        // row = rand() % win->_maxy, col = rand() % win->_maxx;
        row = random_range(0, win->_maxy);
        col = random_range(0, win->_maxx);
        screen_cell = mvwinch(win, row, col) & A_CHARTEXT;
    }
    while (screen_cell != ' ');

    waddch(win, food);
}

int random_range(int min, int max) {
    // Return a random integer in the range min-max
    return rand() % (max - min + 1) + min; 

}

int main() {

    int c;  // for keyboard input
    int game_over;  // When becomes 1, game over
    int gameover_col = 20; // Starting column of "Game Over"
    int direction;
    int score;

    // Create overall window. This will display menu and status bar,
    // and also contain the game animation window 'gamewin'
    initscr();
    start_color();

    // Seed the random generator
    srand(time(NULL));

    // Create gameplay window 'gamewin'. Same width as stdscr
    // but two rows shorter to allow for menu and status bars.
    if (LINES < gamewin_height-2 or COLS < gamewin_width) {
        fprintf(stderr, "Your terminal is not large enough to display the game window. ");
        fprintf(stderr, "Please re-size your terminal to at least %d x %d ", gamewin_width, gamewin_height);
        fprintf(stderr, "characters before re-launching the program\n");
        endwin();
        return 1;
    }
    WINDOW *gamewin = newwin(gamewin_height, gamewin_width, (LINES-gamewin_height) / 2, (COLS-gamewin_width) / 2);
    if (!gamewin) {
        fprintf(stderr, "Cannot initialise game window\n");
        endwin();
        return 1;
    }

    // While in dev - colour the background so we can see it
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    wbkgd(gamewin, COLOR_PAIR(1));
    wborder(gamewin, 0,0,0,0,0,0,0,0);

    // key handling options
    cbreak();
    // keypad(stdscr, TRUE);
    keypad(gamewin, TRUE);
    noecho();
    curs_set(0);

    nodelay(stdscr, false); // Blocking mode while in main menu loop

    // Set up the containing screen
    mvprintw(0, 0, "Score:");  // TODO - move to right hand side
    mvprintw(LINES - 1, 0, "S - Start New Game | Q - Quit");
    refresh();

    // Create the snake object in gamewin.
    Snake snake(gamewin);

    // ===================
    // Main control loop
    // ===================

    c = getch();  // Wait for key
    while (c != 'q' and c !='Q') {

        if (c == 's' or c == 'S') {
        // Start a new game

            // Reset game state
            game_over = 0;
            score = 0;
            mvprintw(0, gameover_col, "         ");

            snake.start();
            // Place a piece of food
            plot_food(gamewin);

            wtimeout(gamewin, 500);  // 500 ms timeout while debugging

            // Main gameplay loop

            while(!game_over) {
                c = wgetch(gamewin);    // Read keystroke into buffer

                direction = snake.get_direction();

                switch(c) {
                    // Update the direction based on what key has been pressed
                    case KEY_UP:
                        if (direction == KEY_DOWN) { // Crashed back onto yourself
                            game_over = 1;
                            break;
                        }
                        snake.set_direction(KEY_UP);
                        break;
                    case KEY_DOWN:
                        if (direction == KEY_UP) { // Crashed back onto yourself
                            game_over = 1;
                            break;
                        }
                        snake.set_direction(KEY_DOWN);
                        break;
                    case KEY_LEFT:
                        if (direction == KEY_RIGHT) { // Crashed back onto yourself
                            game_over = 1;
                            break;
                        }
                        snake.set_direction(KEY_LEFT);
                        break;
                    case KEY_RIGHT:
                        if (direction == KEY_LEFT) { // Crashed back onto yourself
                            game_over = 1;
                            break;
                        }
                        snake.set_direction(KEY_RIGHT);
                    default:
                        // Ignore other key presses
                        break;
                }

                // Check for moving out of bounds
                if (!snake.can_advance())
                    game_over = 1;

                if (!game_over)
                    if (!snake.advance(&score))   // Advance the snake. If an error is returned,
                        game_over = 1;      // the snake has collided with itself, game over

                // Display score
                mvprintw(0, 7, "%d", score); // TODO - only needs re-display if it has changed

                refresh();
                wrefresh(gamewin);
            }

            // ** Game Over **
            mvprintw(0, gameover_col, "GAME OVER");
            // Reset keyboard input to blocking
            wtimeout(gamewin, -1);
        }
        // More 'if's can go here if other menu options wanted
        c = getch();
    }

    // Cleanup and exit
    delwin(gamewin);
    endwin();

    return 0;
}
