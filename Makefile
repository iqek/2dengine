build:
	g++ -Wall -std=c++17 -I"./libs/" src/*.cpp \
	-I/opt/homebrew/include \
	-L/opt/homebrew/lib \
	`pkg-config --libs --cflags sdl3` \
       	-lSDL3_image -lSDL3_ttf -lSDL3_mixer \
	-llua \
	-o 2dengine

run:
	./2dengine

clean:
	rm -f 2dengine
