#include <ncurses.h>
#include <unistd.h>

// Proof of concept of a timed running loop with keyboard reading

int main() {

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    int counter = 0;
    int c;

    while(TRUE) {
        c = getch();
        switch(c) {
            case KEY_UP:
                printw("UP ");
                break;
            case KEY_DOWN:
                printw("DOWN ");
                break;
            case KEY_LEFT:
                printw("LEFT ");
                break;
            case KEY_RIGHT:
                printw("RIGHT ");
        }
        if (c == 'q' or c == 'Q')
            break;
        printw("%d ", counter++);
        refresh();
        usleep(500000); // 500 millisecs
    }

    endwin();
}
