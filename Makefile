snake: snake.cpp
	g++ -Wall -DDEBUG snake.cpp -lncurses -osnake

testrunloop: testrunloop.cpp
	g++ -Wall testrunloop.cpp -lncurses -otestrunloop

clean:
	rm snake testrunloop *.o
