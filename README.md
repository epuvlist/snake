McPuvlist's Ncurses snake game
==============================

This is a version of the traditional "Snake" game implemented 
using the ncurses library in C++.

This was an exercise in testing the capability of ncurses for
games programming.

How to play
-----------

Press S to start a new game, or Q to quit.

Use the arrow keys to move the snake. If you pick up a piece of 
food (displayed as '*'), the snake will add one piece to its length.
If you hit the border of the game area, or move back on yourself, then
the game will end.

The speed of the game increases as the snake gets longer.

Programming notes
-----------------

This is written in C++. Classes are used for the snake, and for a Gamespeed object
that calculates the timeout interval to use as the game speed.

The ncurses library must be installed. To get ncurses see (https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/).
Compiled on a Raspberry Pi 4 running the aarch64 version of Raspberry Pi OS Bullseye.
Compiled using:  
>g++ -Wall snake.cpp -lncurses -osnake

Only plain characters are used ('O' for the snake pieces and '*' for the food piece.
I tried using extended characters (e.g. ACS_DIAMOND) but because ncurses implements
these as macros it is difficult to use their values as constants.
