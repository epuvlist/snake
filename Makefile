snake: snake.cpp
	g++ -Wall -std=c++11 -DDEBUG snake.cpp -lncurses -osnake

clean:
	rm snake *.o
