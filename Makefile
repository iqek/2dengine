CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I"./libs/" -I"./libs/spdlog/include"
SRC_FILES = ./src/*.cpp ./src/Game/*.cpp
LINKER_FLAGS = `pkg-config --libs --cflags sdl3` -lSDL3_image -lSDL3_ttf -lSDL3_mixer -llua 
OBJ_NAME = 2dengine

build:
	$(CC) $(COMPILER_FLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) \
	-I/opt/homebrew/include \
	-L/opt/homebrew/lib \
	$(LINKER_FLAGS) \
	-o $(OBJ_NAME)

run:
	./$(OBJ_NAME)

clean:
	rm $(OBJ_NAME)
