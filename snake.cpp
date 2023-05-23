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

    initscr();
    cbreak();

    Snake s(4);

    getch();
    endwin();

}