build: 
	g++ -Wall -std=c++17 src/*.cpp -lSDL2 -llua5.3 -o gameengine

run:
	./gameengine

clean:
	rm gameengine