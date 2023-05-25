snake: snake.cpp
	g++ -Wall snake.cpp -lncurses -osnake

testrunloop: testrunloop.cpp
	g++ -Wall testrunloop.cpp -lncurses -otestrunloop

clean:
	rm snake testrunloop *.o
