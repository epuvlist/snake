// snake.cpp
// =========
//
// Snake game using ncurses
//
// Copyright (C) Ernold C. McPuvlist, 2023

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

// Forward declarations
void plot_food(WINDOW *w);

// Static globals
static const char snake_piece = '*',
    food = 'X';
static const int game_speed = 3; // Parameter for halfdelay function in tenths of a second

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
        direction = KEY_LEFT;
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

        wclear(win);
        
        head = 0;
        tail = 0;

        // Set the coordinates as the middle of the window
        coords[head].row = max_row / 2;
        coords[head].col = max_col / 2;

        mvwaddch(win, coords[head].row, coords[head].col, snake_piece);
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

    int advance() {
        // Advance the snake by adding a new head, and deleting old tail if 
        // not on a food piece.
        // Also update the coords array, which tracks where the snake has been.
        // Returns 1 if successful, 0 if hit another part of the snake.

        int old_row = coords[head].row, old_col = coords[head].col;
        
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

        switch(winch(win) & A_CHARTEXT) {
            case food:
                // Food piece hit - plot new head but don't erase tail
                waddch(win, snake_piece);
                plot_food(win);  // Plot another piece
                return 1;
                break;
            case snake_piece:
                return 0;  // Game Over
                break;           
            default:
                // No food here, plot head and remove the tail
                waddch(win, snake_piece);
                mvwaddch(win, coords[tail].row, coords[tail].col, ' ');
                // Advance the tail in the coordinates array
                tail = (tail + 1) % max_length;
                return 1;
        }
    };
};

void plot_food(WINDOW *win) {
    // Plot a new piece of food in a random location
    int max_row, max_col;
    getmaxyx(win, max_row, max_col);

    int row, col;
    char screen_cell;
    
    do {
        // TODO - need to add 1?
        row = rand() % max_row, col = rand() % max_col;
        screen_cell = mvwinch(win, row, col) & A_CHARTEXT;
    }
    while (screen_cell != ' ');

    waddch(win, food);
}

int main() {

    int c;  // keyboard input
    int game_over;  // When becomes 1, game over
    int gameover_col; // Starting column of "Game Over"
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
    WINDOW *gamewin = newwin(LINES - 2, COLS, 1, 0);
#ifdef DEBUG
    // While in dev - colour the background so we can see it
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    wbkgd(gamewin, COLOR_PAIR(1));
    wborder(gamewin, 0,0,0,0,0,0,0,0);
#endif

    // key handling options
    cbreak();
    // keypad(stdscr, TRUE);
    keypad(gamewin, TRUE);
    noecho();
    curs_set(0);

    nodelay(stdscr, false); // Blocking mode while in main menu loop

    // Set up the containing screen
    gameover_col = 20;
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

        // Reset game state
        // TODO - this only needs doing at start of game, not every loop
        game_over = 0;
        mvprintw(0, gameover_col, "         ");
        refresh();

        if (c == 's' or c == 'S') {
            // Start the game
            snake.start();
            // Place a piece of food
            plot_food(gamewin);
#ifndef DEBUG
            nodelay(gamewin, true);
#endif
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
                    if (!snake.advance())   // Advance the snake. If an error is returned,
                        game_over = 1;      // the snake has collided with itself, game over
                
                refresh();
                wrefresh(gamewin);
            }

            // ** Game Over **
            mvprintw(0, gameover_col, "GAME OVER");
        }
        c = getch();
    }

    // Cleanup and exit
    delwin(gamewin);
    endwin();
}
