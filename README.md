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

The ncurses library must be installed. To get ncurses see [NCURSES Programming HOWTO](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/).

Written and compiled on a Raspberry Pi 4 running Raspberry Pi OS Bullseye (64-bit).
Compiled using the command:
>g++ -Wall -std=c++11 snake.cpp -lncurses -osnake

C++11 is the minimum version required. Also compiles OK on Mac OS Catalina using g++ and clang++ compilers. Not tested on a Windows system.

Only plain characters are used ('O' for the snake pieces and '*' for the food piece).
I tried using extended characters (e.g. ACS_DIAMOND) but because ncurses implements
these as macros it is difficult to use their values as constants.
