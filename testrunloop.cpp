#include <ncurses.h>
// #include <unistd.h>  // Not needed if not using sleep() or usleep()

// Proof of concept of a timed running loop with keyboard reading

int main() {

    initscr();
    cbreak();   // Allow system break
    noecho();   // Do not echo keypresses
//    nodelay(stdscr, TRUE);  // Read key presses into buffer without waiting
    halfdelay(10);
    keypad(stdscr, TRUE);   // Enable function and arrow keys

    int counter = 0;
    int c;

    while(TRUE) {
        c = getch();    // Read any keystroke into buffer
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
//        usleep(750000); // sleep for 0.75 seconds
    }

    endwin();
}
