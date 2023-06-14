snake: snake.cpp
	g++ -Wall -DDEBUG snake.cpp -lncurses -osnake

clean:
	rm snake *.o
